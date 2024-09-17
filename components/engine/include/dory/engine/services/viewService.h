#pragma once

#include "dory/generics/typeTraits.h"
#include "dory/engine/resources/entity.h"
#include "pipelineService.h"

namespace dory::engine::services
{
    template<typename TImplementation, typename TDataContext>
    class IViewService: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        auto createView(TDataContext& dataContext, resources::entity::IdType windowId, resources::entity::IdType parentPipelineNodeId)
        {
            this->toImplementation()->createViewImpl(dataContext, windowId, parentPipelineNodeId);
        }

        void destroyView(resources::entity::IdType windowId)
        {
            this->toImplementation()->destroyViewImpl(windowId);
        }
    };

    template<typename TDataContext,
            typename TViewRepository,
            typename TPipelineRepository,
            typename TCameraRepository,
            typename TViewControllerFactory>
    struct ViewManagerDependencies
    {
        using DataContextType = TDataContext;
        using ViewRepositoryType = TViewRepository;
        using PipelineRepositoryType = TPipelineRepository;
        using CameraRepositoryType = TCameraRepository;
        using ViewControllerFactoryType = TViewControllerFactory;
    };

    template<typename T>
    requires(is_instance_v<T, ViewManagerDependencies>)
    class ViewService: public IViewService<ViewService<T>, typename T::DataContextType>
    {
    private:
        using DataContextType = typename T::DataContextType;

        using ViewRepositoryType = repositories::IEntityRepository<typename T::ViewRepositoryType, resources::entity::View, resources::entity::IdType>;
        ViewRepositoryType& viewRepository;

        using PipelineRepositoryType = repositories::IPipelineRepository<typename T::PipelineRepositoryType, typename T::DataContextType>;
        PipelineRepositoryType& pipelineRepository;

        using CameraRepositoryType = repositories::IEntityRepository<typename T::CameraRepositoryType, resources::entity::Camera, resources::entity::IdType>;
        CameraRepositoryType&cameraRepository;

        using ViewControllerFactoryType = IServiceFactory<typename T::ViewControllerFactoryType>;
        ViewControllerFactoryType& viewControllerFactory;

    public:
        explicit ViewService(ViewRepositoryType& viewRepository,
                             PipelineRepositoryType& pipelineRepository,
                             CameraRepositoryType& cameraRepository,
                             ViewControllerFactoryType& viewControllerFactory):
                viewRepository(viewRepository),
                pipelineRepository(pipelineRepository),
                cameraRepository(cameraRepository),
                viewControllerFactory(viewControllerFactory)
        {}

        auto createViewImpl(DataContextType& dataContext, resources::entity::IdType windowId, resources::entity::IdType parentPipelineNodeId)
        {
            auto camera = cameraRepository.store(resources::entity::Camera{resources::entity::nullId});

            auto viewController = viewControllerFactory.createInstance();
            auto viewControllerNode = pipelineRepository.store(resources::entity::PipelineNode<DataContextType>{resources::entity::nullId,
                                                                                    viewController,
                                                                                    resources::entity::PipelineNodePriority::Default,
                                                                                    parentPipelineNodeId});

            auto view = viewRepository.store(resources::entity::View(resources::entity::nullId,
                                                          windowId,
                                                          viewControllerNode.id,
                                                          camera.id,
                                                          resources::entity::Viewport{0, 0, 0, 0}));

            viewController->initialize(viewControllerNode.id, dataContext);

            return view;
        }

        void destroyViewImpl(resources::entity::IdType windowId)
        {
            auto view = viewRepository.find([&windowId](const resources::entity::View& view)
            {
                return view.windowId == windowId;
            });

            if(view)
            {
                pipelineRepository.remove(view->controllerNodeId);
                cameraRepository.remove(view->cameraId);
                viewRepository.remove(view->id);
            }
        }
    };
}
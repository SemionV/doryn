#pragma once

#include "base/typeComponents.h"
#include "base/domain/entity.h"
#include "pipelineService.h"

namespace dory::domain::services
{
    template<typename TImplementation, typename TDataContext>
    class IViewService: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        auto createView(TDataContext& dataContext, entity::IdType windowId, entity::IdType parentPipelineNodeId)
        {
            this->toImplementation()->createViewImpl(dataContext, windowId, parentPipelineNodeId);
        }

        void destroyView(entity::IdType windowId)
        {
            this->toImplementation()->destroyViewImpl(windowId);
        }
    };

    template<typename TDataContext,
            typename TViewRepository,
            typename TPipelineRepository,
            typename TCameraRepository,
            typename TViewControllerFactory>
    struct ViewServiceDependencies
    {
        using DataContextType = TDataContext;
        using ViewRepositoryType = TViewRepository;
        using PipelineRepositoryType = TPipelineRepository;
        using CameraRepositoryType = TCameraRepository;
        using ViewControllerFactoryType = TViewControllerFactory;
    };

    template<typename T>
    requires(is_instance_v<T, ViewServiceDependencies>)
    class ViewService: public IViewService<ViewService<T>, typename T::DataContextType>
    {
    private:
        using DataContextType = typename T::DataContextType;

        using ViewRepositoryType = IEntityRepository<typename T::ViewRepositoryType, entity::View, entity::IdType>;
        ViewRepositoryType& viewRepository;

        using PipelineRepositoryType = IPipelineRepository<typename T::PipelineRepositoryType, typename T::DataContextType>;
        PipelineRepositoryType& pipelineRepository;

        using CameraRepositoryType = IEntityRepository<typename T::CameraRepositoryType, entity::Camera, entity::IdType>;
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

        auto createViewImpl(DataContextType& dataContext, entity::IdType windowId, entity::IdType parentPipelineNodeId)
        {
            auto camera = cameraRepository.store(entity::Camera{entity::nullId});

            auto viewController = viewControllerFactory.createInstance();
            auto viewControllerNode = pipelineRepository.store(entity::PipelineNode<DataContextType>{entity::nullId,
                                                                                    viewController,
                                                                                    entity::PipelineNodePriority::Default,
                                                                                    parentPipelineNodeId});

            auto view = viewRepository.store(entity::View(entity::nullId,
                                                          windowId,
                                                          viewControllerNode.id,
                                                          camera.id,
                                                          dory::domain::entity::Viewport{0, 0, 0, 0}));

            viewController->initialize(viewControllerNode.id, dataContext);

            return view;
        }

        void destroyViewImpl(entity::IdType windowId)
        {
            auto view = viewRepository.find([&windowId](const entity::View& view)
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
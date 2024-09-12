#pragma once

#include "dory/generics/typeTraits.h"
#include "dory/engine/entity.h"
#include "pipelineManager.h"

namespace dory::domain::managers
{
    template<typename TImplementation, typename TDataContext>
    class IViewManager: NonCopyable, public StaticInterface<TImplementation>
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
    class ViewManager: public IViewManager<ViewManager<T>, typename T::DataContextType>
    {
    private:
        using DataContextType = typename T::DataContextType;

        using ViewRepositoryType = IEntityRepository<typename T::ViewRepositoryType, entity::View, entity::IdType>;
        ViewRepositoryType& viewRepository;

        using PipelineRepositoryType = repositories::IPipelineRepository<typename T::PipelineRepositoryType, typename T::DataContextType>;
        PipelineRepositoryType& pipelineRepository;

        using CameraRepositoryType = IEntityRepository<typename T::CameraRepositoryType, entity::Camera, entity::IdType>;
        CameraRepositoryType&cameraRepository;

        using ViewControllerFactoryType = IServiceFactory<typename T::ViewControllerFactoryType>;
        ViewControllerFactoryType& viewControllerFactory;

    public:
        explicit ViewManager(ViewRepositoryType& viewRepository,
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
#include <dory/engineObjects/dataContext.h>
#include <dory/engineObjects/controllers/pipelineController.h>

namespace dory::engine::controllers
{
    template<typename TPipelineRepository>
    PipelineController<TPipelineRepository>::PipelineController(events::mainController::Dispatcher<DataContextType>& mainControllerEventHub,
                                repositories::IPipelineRepository<TPipelineRepository, DataContextType>& pipelineRepository):
            _mainControllerEventHub(mainControllerEventHub),
            _pipelineRepository(pipelineRepository)
    {}

    template<typename TPipelineRepository>
    void PipelineController<TPipelineRepository>::updateImpl(DataContextType& context, const resources::TimeSpan& timeStep)
    {
        auto pipelineNodes = _pipelineRepository.getPipeline();

        touchPipelineNodes(pipelineNodes, context, [](const std::shared_ptr<resources::object::PipelineNode<DataContextType>>& node, DataContextType& context, const resources::TimeSpan& timeStep)
        {
            auto controller = node->nodeEntity.attachedController ?
                              std::static_pointer_cast<Controller<DataContextType>>(node->nodeEntity.attachedController)
                                                                  : nullptr;
            if(controller)
            {
                controller->update(node->nodeEntity.id, timeStep, context);
            }
            else if(node->nodeEntity.update)
            {
                node->nodeEntity.update(node->nodeEntity.id, timeStep, context);
            }
        }, timeStep);
    }

    template<typename TPipelineRepository>
    void PipelineController<TPipelineRepository>::initializeImpl(DataContextType& context)
    {
        _mainControllerEventHub.fire(context, events::mainController::Initialize{});
    };

    template<typename TPipelineRepository>
    void PipelineController<TPipelineRepository>::stopImpl(DataContextType& context)
    {
        _mainControllerEventHub.fire(context, events::mainController::Stop{});

        auto pipelineNodes = _pipelineRepository.getPipeline();
        auto expiredNodes = std::vector<typename repositories::IPipelineRepository<TPipelineRepository, DataContextType>::IdType>{};

        touchPipelineNodes(pipelineNodes, context, [&expiredNodes](const std::shared_ptr<resources::object::PipelineNode<DataContextType>>& node, DataContextType& context)
        {
            auto controller = node->nodeEntity.attachedController;
            if(controller)
            {
                std::static_pointer_cast<Controller<DataContextType>>(controller)->stop(node->nodeEntity.id, context);
            }
        });

        for(auto id : expiredNodes)
        {
            _pipelineRepository.remove(id);
        }
    }
}
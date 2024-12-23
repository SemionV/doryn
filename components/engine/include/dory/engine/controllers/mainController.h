#pragma once

#include "dory/generics/typeTraits.h"
#include "dory/module.h"

#include "dory/engine/controllers/controller.h"
#include "dory/engine/events/eventTypes.h"
#include "dory/engine/resources/object.h"
#include "dory/engine/services/pipelineService.h"

namespace dory::engine::controllers
{
    template<typename TImplementation>
    class IMainController: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        void update(DataContextType& context, const resources::TimeSpan& timeStep)
        {
            this->toImplementation()->updateImpl(context, timeStep);
        }

        void initialize(DataContextType& context)
        {
            this->toImplementation()->initializeImpl(context);
        };

        void stop(DataContextType& context)
        {
            this->toImplementation()->stopImpl(context);
        }
    };

    template<typename TPipelineRepository>
    class MainController: public IMainController<MainController<TPipelineRepository>>
    {
    private:
        events::mainController::Dispatcher<DataContextType>& _mainControllerEventHub;
        repositories::IPipelineRepository<TPipelineRepository, DataContextType>& _pipelineRepository;

    public:
        explicit MainController(events::mainController::Dispatcher<DataContextType>& mainControllerEventHub,
                                repositories::IPipelineRepository<TPipelineRepository, DataContextType>& pipelineRepository):
                _mainControllerEventHub(mainControllerEventHub),
                _pipelineRepository(pipelineRepository)
        {}

        void updateImpl(DataContextType& context, const resources::TimeSpan& timeStep)
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

        void initializeImpl(DataContextType& context)
        {
            _mainControllerEventHub.fire(context, events::mainController::Initialize{});
        };

        void stopImpl(DataContextType& context)
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

    private:
        template<typename F, typename... Ts>
        static void touchPipelineNodes(std::list<std::shared_ptr<resources::object::PipelineNode<DataContextType>>> pipelineNodes, DataContextType& context, F functor, Ts... arguments)
        {
            auto end = pipelineNodes.end();

            for(auto i = pipelineNodes.begin(); i != end; ++i)
            {
                touchNode(*i, context, functor, arguments...);
            }
        }

        template<typename F, typename... Ts>
        static void touchNode(std::shared_ptr<resources::object::PipelineNode<DataContextType>> node, DataContextType& context, F functor, Ts... arguments)
        {
            functor(node, context, arguments...);

            auto end = node->children.end();

            for(auto i = node->children.begin(); i != end; ++i)
            {
                touchNode(*i, context, functor, arguments...);
            }
        }
    };
}
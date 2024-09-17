#pragma once

#include "dory/generics/typeTraits.h"
#include "dory/module.h"

#include "dory/engine/controllers/controller.h"
#include <dory/engine/resources/eventTypes.h>
#include "dory/engine/resources/object.h"
#include "dory/engine/services/pipelineService.h"

namespace dory::engine::controllers
{
    template<typename TImplementation, typename TDataContext>
    class IMainController: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        void update(TDataContext& context, const resources::TimeSpan& timeStep)
        {
            this->toImplementation()->updateImpl(context, timeStep);
        }

        void initialize(TDataContext& context)
        {
            this->toImplementation()->initializeImpl(context);
        };

        void stop(TDataContext& context)
        {
            this->toImplementation()->stopImpl(context);
        }
    };

    template<typename TDataContext, typename TPipelineRepository>
    class MainController: public IMainController<MainController<TDataContext, TPipelineRepository>, TDataContext>
    {
    private:
        resources::eventTypes::mainController::Dispatcher<TDataContext>& _mainControllerEventHub;
        repositories::IPipelineRepository<TPipelineRepository, TDataContext>& _pipelineRepository;

    public:
        explicit MainController(resources::eventTypes::mainController::Dispatcher<TDataContext>& mainControllerEventHub,
                                repositories::IPipelineRepository<TPipelineRepository, TDataContext>& pipelineRepository):
                _mainControllerEventHub(mainControllerEventHub),
                _pipelineRepository(pipelineRepository)
        {}

        void updateImpl(TDataContext& context, const resources::TimeSpan& timeStep)
        {
            auto pipelineNodes = _pipelineRepository.getPipeline();

            touchPipelineNodes(pipelineNodes, context, [](const std::shared_ptr<resources::object::PipelineNode<TDataContext>>& node, TDataContext& context, const resources::TimeSpan& timeStep)
            {
                auto controller = node->nodeEntity.attachedController ?
                                  std::static_pointer_cast<Controller<TDataContext>>(node->nodeEntity.attachedController)
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

        void initializeImpl(TDataContext& context)
        {
            _mainControllerEventHub.fire(context, resources::eventTypes::mainController::Initialize{});
        };

        void stopImpl(TDataContext& context)
        {
            _mainControllerEventHub.fire(context, resources::eventTypes::mainController::Stop{});

            auto pipelineNodes = _pipelineRepository.getPipeline();
            auto expiredNodes = std::vector<typename repositories::IPipelineRepository<TPipelineRepository, TDataContext>::IdType>{};

            touchPipelineNodes(pipelineNodes, context, [&expiredNodes](const std::shared_ptr<resources::object::PipelineNode<TDataContext>>& node, TDataContext& context)
            {
                auto controller = node->nodeEntity.attachedController;
                if(controller)
                {
                    std::static_pointer_cast<Controller<TDataContext>>(controller)->stop(node->nodeEntity.id, context);
                }
            });

            for(auto id : expiredNodes)
            {
                _pipelineRepository.remove(id);
            }
        }

    private:
        template<typename F, typename... Ts>
        static void touchPipelineNodes(std::list<std::shared_ptr<resources::object::PipelineNode<TDataContext>>> pipelineNodes, TDataContext& context, F functor, Ts... arguments)
        {
            auto end = pipelineNodes.end();

            for(auto i = pipelineNodes.begin(); i != end; ++i)
            {
                touchNode(*i, context, functor, arguments...);
            }
        }

        template<typename F, typename... Ts>
        static void touchNode(std::shared_ptr<resources::object::PipelineNode<TDataContext>> node, TDataContext& context, F functor, Ts... arguments)
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
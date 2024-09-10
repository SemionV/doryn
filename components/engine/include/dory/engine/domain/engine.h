#pragma once

#include <dory/generics/typeComponents.h>
#include <dory/engine/module.h>

#include "controller.h"
#include "events/hub.h"
#include "object.h"
#include "managers/pipelineManager.h"

namespace dory::domain
{
    template<typename TImplementation, typename TDataContext>
    class IEngine: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        void update(TDataContext& context, const TimeSpan& timeStep)
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
    class Engine: public IEngine<Engine<TDataContext, TPipelineRepository>, TDataContext>
    {
    private:
        events::engine::Dispatcher<TDataContext>& engineEventHub;
        repositories::IPipelineRepository<TPipelineRepository, TDataContext>& pipelineRepository;

    public:
        explicit Engine(events::engine::Dispatcher<TDataContext>& engineEventHub,
                        repositories::IPipelineRepository<TPipelineRepository, TDataContext>& pipelineRepository):
                engineEventHub(engineEventHub),
                pipelineRepository(pipelineRepository)
        {}

        void updateImpl(TDataContext& context, const TimeSpan& timeStep)
        {
            auto pipelineNodes = pipelineRepository.getPipeline();

            touchPipelineNodes(pipelineNodes, context, [](const std::shared_ptr<object::PipelineNode<TDataContext>>& node, TDataContext& context, const TimeSpan& timeStep)
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
            engineEventHub.fire(context, events::engine::Initialize{});
        };

        void stopImpl(TDataContext& context)
        {
            engineEventHub.fire(context, events::engine::Stop{});

            auto pipelineNodes = pipelineRepository.getPipeline();
            auto expiredNodes = std::vector<typename repositories::IPipelineRepository<TPipelineRepository, TDataContext>::IdType>{};

            touchPipelineNodes(pipelineNodes, context, [&expiredNodes](const std::shared_ptr<object::PipelineNode<TDataContext>>& node, TDataContext& context)
            {
                auto controller = node->nodeEntity.attachedController;
                if(controller)
                {
                    std::static_pointer_cast<Controller<TDataContext>>(controller)->stop(node->nodeEntity.id, context);
                }
            });

            for(auto id : expiredNodes)
            {
                pipelineRepository.remove(id);
            }
        }

    private:
        template<typename F, typename... Ts>
        static void touchPipelineNodes(std::list<std::shared_ptr<object::PipelineNode<TDataContext>>> pipelineNodes, TDataContext& context, F functor, Ts... arguments)
        {
            auto end = pipelineNodes.end();

            for(auto i = pipelineNodes.begin(); i != end; ++i)
            {
                touchNode(*i, context, functor, arguments...);
            }
        }

        template<typename F, typename... Ts>
        static void touchNode(std::shared_ptr<object::PipelineNode<TDataContext>> node, TDataContext& context, F functor, Ts... arguments)
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
#pragma once

#include "controller.h"
#include "events/engineEventHub.h"
#include "base/typeComponents.h"
#include "object.h"
#include "services/pipelineService.h"

namespace dory::domain
{
    template<typename TImplementation, typename TDataContext>
    class IEngine: Uncopyable, public StaticInterface<TImplementation>
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
        events::EngineEventHubDispatcher<TDataContext>& engineEventHub;
        services::IPipelineRepository<TPipelineRepository, TDataContext>& pipelineRepository;

    public:
        explicit Engine(events::EngineEventHubDispatcher<TDataContext>& engineEventHub,
                        services::IPipelineRepository<TPipelineRepository, TDataContext>& pipelineRepository):
                engineEventHub(engineEventHub),
                pipelineRepository(pipelineRepository)
        {}

        void updateImpl(TDataContext& context, const TimeSpan& timeStep)
        {
            auto pipelineNodes = pipelineRepository.getPipeline();

            touchPipelineNodes(pipelineNodes, context, [](const std::shared_ptr<object::PipelineNode<TDataContext>>& node, TDataContext& context, const TimeSpan& timeStep)
            {
                auto controller = node->nodeEntity.attachedController;
                if(controller)
                {
                    std::static_pointer_cast<Controller<TDataContext>>(controller)->update(node->nodeEntity.id, timeStep, context);
                }
                else if(node->nodeEntity.update)
                {
                    node->nodeEntity.update(node->nodeEntity.id, timeStep, context);
                }
            }, timeStep);
        }

        void initializeImpl(TDataContext& context)
        {
            engineEventHub.fire(context, events::InitializeEngineEventData{});
        };

        void stopImpl(TDataContext& context)
        {
            engineEventHub.fire(context, events::StopEngineEventData{});

            auto pipelineNodes = pipelineRepository.getPipeline();

            touchPipelineNodes(pipelineNodes, context, [](const std::shared_ptr<object::PipelineNode<TDataContext>>& node, TDataContext& context)
            {
                auto controller = node->nodeEntity.attachedController;
                if(controller)
                {
                    std::static_pointer_cast<Controller<TDataContext>>(controller)->stop(node->nodeEntity.id, context);
                }
            });
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
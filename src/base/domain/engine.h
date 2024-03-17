#pragma once

#include "controller.h"
#include "events/engineEventHub.h"
#include "base/typeComponents.h"
#include "object.h"
#include "services/pipelineService.h"

namespace dory::domain
{
    template<class TDataContext, typename TServiceLocator>
    class Engine: Service<TServiceLocator>
    {
    public:
        explicit Engine(TServiceLocator& serviceLocator):
                Service<TServiceLocator>(serviceLocator)
        {}

        void update(TDataContext& context, const TimeSpan& timeStep)
        {
            auto pipelineNodes = this->services.pipelineService.getPipeline();

            touchPipelineNodes(pipelineNodes, context, [](const std::shared_ptr<object::PipelineNode>& node, TDataContext& context, const TimeSpan& timeStep)
            {
                auto controller = node->nodeEntity.attachedController;
                if(controller)
                {
                    std::static_pointer_cast<Controller<TDataContext, TServiceLocator>>(controller)->update(node->nodeEntity.id, timeStep, context);
                }
            }, timeStep);
        }

        void initialize(TDataContext& context)
        {
            this->services.engineEventHub.fire(context, events::InitializeEngineEventData());
        };

        void stop(TDataContext& context)
        {
            this->services.engineEventHub.fire(context, events::StopEngineEventData());
        }

    private:
        template<typename F, typename... Ts>
        static void touchPipelineNodes(std::list<std::shared_ptr<object::PipelineNode>> pipelineNodes, TDataContext& context, F functor, Ts... arguments)
        {
            auto end = pipelineNodes.end();

            for(auto i = pipelineNodes.begin(); i != end; ++i)
            {
                touchNode(*i, context, functor, arguments...);
            }
        }

        template<typename F, typename... Ts>
        static void touchNode(std::shared_ptr<object::PipelineNode> node, TDataContext& context, F functor, Ts... arguments)
        {
            functor(node, context, arguments...);

            auto end = node->children.end();

            for(auto i = node->children.begin(); i != end; ++i)
            {
                touchNode(*i, context, functor, arguments...);
            }
        }
    };

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
    class Engine2: public IEngine<Engine2<TDataContext, TPipelineRepository>, TDataContext>
    {
    private:
        events::EngineEventHubDispatcher<TDataContext>& engineEventHub;
        services::IPipelineRepository<TPipelineRepository>& pipelineRepository;

    public:
        explicit Engine2(events::EngineEventHubDispatcher<TDataContext>& engineEventHub,
                         services::IPipelineRepository<TPipelineRepository>& pipelineRepository):
                engineEventHub(engineEventHub),
                pipelineRepository(pipelineRepository)
        {}

        void updateImpl(TDataContext& context, const TimeSpan& timeStep)
        {
            auto pipelineNodes = pipelineRepository.getPipeline();

            touchPipelineNodes(pipelineNodes, context, [](const std::shared_ptr<object::PipelineNode>& node, TDataContext& context, const TimeSpan& timeStep)
            {
                auto controller = node->nodeEntity.attachedController;
                if(controller)
                {
                    std::static_pointer_cast<Controller2<TDataContext>>(controller)->update(node->nodeEntity.id, timeStep, context);
                }
            }, timeStep);
        }

        void initializeImpl(TDataContext& context)
        {
            engineEventHub.fire(context, events::InitializeEngineEventData());
        };

        void stopImpl(TDataContext& context)
        {
            engineEventHub.fire(context, events::StopEngineEventData());
        }

    private:
        template<typename F, typename... Ts>
        static void touchPipelineNodes(std::list<std::shared_ptr<object::PipelineNode>> pipelineNodes, TDataContext& context, F functor, Ts... arguments)
        {
            auto end = pipelineNodes.end();

            for(auto i = pipelineNodes.begin(); i != end; ++i)
            {
                touchNode(*i, context, functor, arguments...);
            }
        }

        template<typename F, typename... Ts>
        static void touchNode(std::shared_ptr<object::PipelineNode> node, TDataContext& context, F functor, Ts... arguments)
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
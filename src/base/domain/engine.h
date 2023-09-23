#pragma once

#include "controller.h"
#include "services/pipelineService.h"
#include "events/engineEventHub.h"

namespace dory::domain
{
    template<class TDataContext>
    class Engine
    {
        private:
            std::shared_ptr<services::PipelineService> pipelineService;
            std::shared_ptr<events::EngineEventHubDispatcher<TDataContext>> engineEventHub;

        public:
            Engine(std::shared_ptr<services::PipelineService> pipelineService,
                std::shared_ptr<events::EngineEventHubDispatcher<TDataContext>> engineEventHub):
                    pipelineService(pipelineService),
                    engineEventHub(engineEventHub)
            {};

            bool update(TDataContext& context, const TimeSpan& timeStep)
            {
                auto pipelineNodes = pipelineService->getPipeline();

                touchPipelineNodes(pipelineNodes, context, [](std::shared_ptr<object::PipelineNode> node, TDataContext& context, const TimeSpan& timeStep)
                {
                    auto controller = node->nodeEntity.attachedController;
                    if(controller)
                    {
                        std::static_pointer_cast<Controller<TDataContext>>(controller)->update(node->nodeEntity.id, timeStep, context);
                    }
                }, timeStep);

                return context.isStop;
            }
            
            void initialize(TDataContext& context)
            {
                engineEventHub->fire(context, events::InitializeEngineEventData());
            };

            void stop(TDataContext& context)
            {
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
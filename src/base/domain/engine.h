#pragma once

#include "controller.h"
#include "services/pipelineService.h"
#include "events/engineEventHub.h"

namespace dory::domain
{
    class DORY_API Engine
    {
        private:
            std::shared_ptr<services::PipelineService> pipelineService;
            std::shared_ptr<events::EngineEventHubDispatcher> engineEventHub;

        public:
            Engine(std::shared_ptr<services::PipelineService> pipelineService,
                std::shared_ptr<events::EngineEventHubDispatcher> engineEventHub):
                    pipelineService(pipelineService),
                    engineEventHub(engineEventHub)
            {};

            bool update(DataContext& context, const TimeSpan& timeStep);
            void initialize(DataContext& context);
            void stop(DataContext& context);

        private:
            template<typename F, typename... Ts>
            static void touchPipelineNodes(std::list<std::shared_ptr<object::PipelineNode>> pipelineNodes, DataContext& context, F functor, Ts... arguments)
            {
                auto end = pipelineNodes.end();

                for(auto i = pipelineNodes.begin(); i != end; ++i)
                {
                    touchNode(*i, context, functor, arguments...);
                }
            }

            template<typename F, typename... Ts>
            static void touchNode(std::shared_ptr<object::PipelineNode> node, DataContext& context, F functor, Ts... arguments)
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
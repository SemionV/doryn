#pragma once

#include <utility>
#include "controller.h"
#include "events/engineEventHub.h"

namespace dory::domain
{
    template<class TDataContext, typename TServiceLocator>
    class Engine: Service<TServiceLocator>
    {
    public:
        void update(TDataContext& context, const TimeSpan& timeStep)
        {
            auto pipelineNodes = this->services.pipelineService.getPipeline();

            touchPipelineNodes(pipelineNodes, context, [](const std::shared_ptr<object::PipelineNode>& node, TDataContext& context, const TimeSpan& timeStep)
            {
                auto controller = node->nodeEntity.attachedController;
                if(controller)
                {
                    std::static_pointer_cast<Controller<TDataContext>>(controller)->update(node->nodeEntity.id, timeStep, context);
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
}
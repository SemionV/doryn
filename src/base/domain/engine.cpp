#include "base/dependencies.h"
#include "engine.h"

namespace dory::domain
{
    void Engine::initialize(DataContext& context)
    {
        engineEventHub->fire(context, events::InitializeEngineEventData());
    }

    bool Engine::update(DataContext& context, const TimeSpan& timeStep)
    {
        auto pipelineNodes = pipelineService->getPipeline();

        touchPipelineNodes(pipelineNodes, context, [](std::shared_ptr<object::PipelineNode> node, DataContext& context, const TimeSpan& timeStep)
        {
            auto controller = node->nodeEntity->attachedController;
            if(controller)
            {
                std::static_pointer_cast<Controller>(controller)->update(node->nodeEntity->id, timeStep, context);
            }
        }, timeStep);

        return context.isStop;
    }

    void Engine::stop(DataContext& context)
    {

    }
}
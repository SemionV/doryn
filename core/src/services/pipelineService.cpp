#include <dory/core/registry.h>
#include <dory/core/services/pipelineService.h>
#include <dory/core/events/eventTypes.h>
#include <dory/core/repositories/iPipelineRepository.h>
#include "dory/core/iController.h"
#include <vector>

namespace dory::core::services
{
    using namespace core;
    using namespace resources;

    PipelineService::PipelineService(Registry& registry):
            _registry(registry)
    {}

    void PipelineService::update(resources::DataContext& context, const generic::model::TimeSpan& timeStep)
    {
        _registry.get<repositories::IPipelineRepository>([&context, &timeStep](repositories::IPipelineRepository* repository){
            auto nodes = repository->getPipelineNodes();
            auto stack = std::vector<IdType> {};

            for(auto& node : nodes)
            {
                //Remove sibling branches from the stack
                if(!stack.empty())
                {
                    if(node.parentNodeId == nullId)
                    {
                        stack.clear();
                    }
                    else
                    {
                        for(std::size_t i = stack.size(); i > 0; --i)
                        {
                            if(stack[i - 1] == node.parentNodeId)
                            {
                                for(std::size_t j = stack.size() - 1; j > i - 1; --j)
                                {
                                    stack.pop_back();
                                }

                                break;
                            }
                        }
                    }
                }
                else
                {
                    //Should not happen: empty stack, but a node which has a parent(the parent must be on the stack)
                    assert(node.parentNodeId == nullId);
                }

                //if this is not a top level node, and it's parent node was rejected from update,
                //its children will be rejected as well
                if(!stack.empty() && stack.back() != node.parentNodeId)
                {
                    continue;
                }

                if(node.updateTrigger)
                {
                    auto updateTrigger = node.updateTrigger->lock();
                    //If dll with the trigger impl is unloaded or the trigger is false,
                    //continue to the next node in the pipeline
                    if(!updateTrigger || !(*updateTrigger)(node.id, timeStep, context))
                    {
                        continue;
                    }
                }

                stack.push_back(node.id);

                if(node.attachedController)
                {
                    if(auto controllerRef = node.attachedController->lock())
                    {
                        if(const auto controller = std::static_pointer_cast<IController>(*controllerRef))
                        {
                            controller->update(node.id, timeStep, context);
                        }
                    }
                }
                else if(node.updateFunction)
                {
                    if(auto updateFunctionRef = node.updateFunction->lock())
                    {
                        (*updateFunctionRef)(node.id, timeStep, context);
                    }
                }
            }
        });
    }

    void PipelineService::initialize(resources::DataContext& context)
    {
        _registry.get<events::pipeline::Bundle::IDispatcher>([&context](events::pipeline::Bundle::IDispatcher* dispatcher){
            dispatcher->fire(context, events::pipeline::Initialize{});
        });
    }

    void PipelineService::stop(resources::DataContext& context)
    {
        _registry.get<repositories::IPipelineRepository>([&context](repositories::IPipelineRepository* repository){
            auto nodes = repository->getPipelineNodes();

            for(auto& node : nodes)
            {
                if(node.attachedController)
                {
                    auto controllerRef = node.attachedController->lock();
                    if(controllerRef)
                    {
                        auto controller = std::static_pointer_cast<IController>(*controllerRef);
                        if(controller)
                        {
                            controller->stop(node.id, context);
                        }
                    }
                }
            }
        });
    }
}
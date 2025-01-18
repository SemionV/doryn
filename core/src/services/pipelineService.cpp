#include <dory/core/registry.h>
#include <dory/core/services/pipelineService.h>
#include <dory/core/events/eventTypes.h>
#include <dory/core/repositories/iPipelineRepository.h>
#include "dory/core/iController.h"
#include <vector>
#include <set>
#include <spdlog/fmt/fmt.h>

namespace dory::core::services
{
    using namespace core;
    using namespace resources;

    PipelineService::PipelineService(Registry& registry):
            _registry(registry)
    {}

    struct QueueItem
    {
        IdType id { nullId };
        std::size_t pipelineIndex {};
    };

    void PipelineService::update(resources::DataContext& context, const generic::model::TimeSpan& timeStep)
    {
        _registry.get<repositories::IPipelineRepository>([this, &context, &timeStep](repositories::IPipelineRepository* repository){
            auto nodes = repository->getPipelineNodes();
            auto stack = std::vector<QueueItem> { { nullId } }; //put nullId item on top of the tree
            auto nodeUpdates = std::unordered_map<IdType, std::size_t>{};

            for(std::size_t i = 0; i < nodes.size();)
            {
                auto& node = nodes[i];

                //Find how many levels of hierarchy the parent node is higher the current node
                std::size_t levels = 0;
                for(std::size_t j = stack.size(); j > 0; --j)
                {
                    if(stack[j - 1].id == node.parentNodeId)
                    {
                        break;
                    }

                    levels++;
                }

                //if the parent node is not on the stack, skip to the next node
                if(levels == stack.size())
                {
                    continue;
                }

                //Pop previous nodes from the stack until the parent node is on the top
                bool repeatBranchUpdate = false;
                for(std::size_t j = 0; j < levels; j++)
                {
                    const auto& [id, pipelineIndex] = stack.back();
                    stack.pop_back();

                    std::size_t updatesLeft { 0 };
                    if(auto it = nodeUpdates.find(node.id); it != nodeUpdates.end())
                    {
                        updatesLeft = it->second;
                    }

                    //rollback the iterator back to the node(branch), which required more than one update
                    if(updatesLeft > 0)
                    {
                        i = pipelineIndex;
                        repeatBranchUpdate = true;
                        break;
                    }
                }

                if(repeatBranchUpdate)
                {
                    continue;
                }

                std::size_t updatesLeft {};
                if(auto it = nodeUpdates.find(node.id); it != nodeUpdates.end())
                {
                    updatesLeft = it->second;
                }

                if(!updatesLeft)
                {
                    if(node.updateTrigger)
                    {
                        //If dll with the trigger impl is unloaded or the trigger is false,
                        //continue to the next node in the pipeline
                        if(auto updateTrigger = node.updateTrigger->lock())
                        {
                            updatesLeft = (*updateTrigger)(node.id, timeStep, context);
                        }
                    }
                    else
                    {
                        updatesLeft = 1;
                    }
                }

                if(updatesLeft)
                {
                    stack.emplace_back( node.id, i );
                    nodeUpdates[node.id] = --updatesLeft;

                    try
                    {
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
                    catch(const std::exception& e)
                    {
                        if(auto logger = _registry.get<ILogService>())
                        {
                            logger->error(fmt::format("Updating node {}({}): {}", node.name, node.id, e.what()));
                        }
                    }
                }

                ++i;
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

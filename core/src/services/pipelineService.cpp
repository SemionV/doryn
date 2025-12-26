#include <dory/core/registry.h>
#include <dory/core/services/pipelineService.h>
#include <dory/core/events/eventTypes.h>
#include <dory/core/repositories/iPipelineRepository.h>
#include "dory/core/iController.h"
#include "dory/core/iTrigger.h"
#include <vector>
#include <set>
#include <spdlog/fmt/fmt.h>
#include <dory/containers/hashId.h>

namespace dory::core::services
{
    using namespace core;
    using namespace resources;
    using namespace repositories;

    PipelineService::PipelineService(Registry& registry):
            _registry(registry)
    {}

    struct QueueItem
    {
        IdType id { nullId };
        std::size_t pipelineIndex {};
    };

    void PipelineService::update(DataContext& context, const generic::model::TimeSpan& timeStep)
    {
        _registry.get<IPipelineRepository>([this, &context, &timeStep](IPipelineRepository* repository){
            auto nodes = repository->getPipelineNodes();
            auto stack = std::vector<QueueItem> { { nullId } }; //put nullId item on top of the tree
            auto nodeUpdates = std::unordered_map<IdType, entities::NodeUpdateCounter>{};

            entities::PipelineNode lastNode {};
            lastNode.skipUpdate = true;
            lastNode.parentNodeId = nullId;
            const size_t size = nodes.size();

            for(std::size_t i = 0; i <= size; ++i)
            {
                auto& node = i < size ? nodes[i] : lastNode;

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
                    if(auto it = nodeUpdates.find(id); it != nodeUpdates.end())
                    {
                        updatesLeft = it->second.count;
                    }

                    //rollback the iterator back to the node(branch), which required more than one update
                    if(updatesLeft > 0)
                    {
                        i = pipelineIndex - 1;
                        repeatBranchUpdate = true;
                        break;
                    }

                    nodeUpdates.erase(id);
                }

                if(repeatBranchUpdate)
                {
                    continue;
                }

                entities::NodeUpdateCounter updateCounter { 0, timeStep };

                if(auto it = nodeUpdates.find(node.parentNodeId); it != nodeUpdates.end())
                {
                    updateCounter.deltaTime = it->second.deltaTime;
                }

                if(auto it = nodeUpdates.find(node.id); it != nodeUpdates.end())
                {
                    updateCounter = it->second;
                }

                if(updateCounter.count == 0 && !node.skipUpdate)
                {
                    try
                    {
                        if(node.triggerFunction)
                        {
                            //If dll with the trigger impl is unloaded or the trigger is false,
                            //continue to the next node in the pipeline
                            if(auto updateTrigger = node.triggerFunction->lock())
                            {
                                updateCounter = (*updateTrigger)(node.id, updateCounter.deltaTime, context);
                            }
                        }
                        else if(node.trigger)
                        {
                            if(auto trigger = node.trigger->lock())
                            {
                                updateCounter = trigger->check(node.id, updateCounter.deltaTime, context);
                            }
                        }
                        else
                        {
                            updateCounter.count = 1;
                        }
                    }
                    catch(const std::exception& e)
                    {
                        if(auto logger = _registry.get<ILogService, Logger::App>())
                        {
                            logger->error(fmt::format("Triggering node {}({}): {}", node.name, node.id, e.what()));
                        }
                    }
                }

                if(updateCounter.count > 0)
                {
                    stack.emplace_back( node.id, i );
                    --updateCounter.count;
                    nodeUpdates[node.id] = updateCounter;

                    try
                    {
                        if(node.controller)
                        {
                            if(auto controllerRef = node.controller->lock())
                            {
                                if(const auto controller = std::static_pointer_cast<IController>(*controllerRef))
                                {
                                    controller->update(node.id, updateCounter.deltaTime, context);
                                }
                            }
                        }
                        else if(node.updateFunction)
                        {
                            if(auto updateFunctionRef = node.updateFunction->lock())
                            {
                                (*updateFunctionRef)(node.id, updateCounter.deltaTime, context);
                            }
                        }
                    }
                    catch(const std::exception& e)
                    {
                        if(auto logger = _registry.get<ILogService, Logger::App>())
                        {
                            logger->error(fmt::format("Updating node {}({}): {}", node.name, node.id, e.what()));
                        }
                    }
                }
            }
        });
    }

    void PipelineService::initialize(DataContext& context)
    {
        _registry.get<events::pipeline::Bundle::IDispatcher>([&context](events::pipeline::Bundle::IDispatcher* dispatcher){
            dispatcher->fire(context, events::pipeline::Initialize{});
        });
    }

    void PipelineService::stop(DataContext& context)
    {
        _registry.get<IPipelineRepository>([&context](IPipelineRepository* repository){
            for(auto nodes = repository->getPipelineNodes(); auto& node : nodes)
            {
                if(node.controller)
                {
                    if(auto controllerRef = node.controller->lock())
                    {
                        if(const auto controller = std::static_pointer_cast<IController>(*controllerRef))
                        {
                            controller->stop(node.id, context);
                        }
                    }
                }
            }
        });
    }

    void PipelineService::buildPipeline(const scene::Scene& scene, scene::configuration::Pipeline& pipeline, DataContext& context)
    {
        auto logger = _registry.get<ILogService, Logger::App>();

        if(auto pipelineRepo = _registry.get<IPipelineRepository>())
        {
            using EntryType = std::tuple<const IdType, const std::string*, scene::configuration::Node*>;
            auto stack = std::stack<EntryType>{};

            for(auto& [rootNodeName, rootNode] : pipeline.nodes)
            {
                auto parentId = nullId;
                if(!rootNode.parent.empty())
                {
                    if(const auto node = pipelineRepo->getNode(containers::hash::hash(rootNode.parent)))
                    {
                        parentId = node->id;
                    }
                    else if(logger)
                    {
                        logger->error(fmt::format(R"(Parent node "{}" is not found for node "{}")", rootNode.parent, rootNodeName));
                    }
                }

                stack.emplace(parentId, &rootNodeName, &rootNode);

                while(!stack.empty())
                {
                    auto& [parentId, nodeName, node] = stack.top();
                    stack.pop();

                    auto pipelineNode = entities::PipelineNode {};
                    pipelineNode.name = containers::hash::hash(*nodeName);
                    pipelineNode.sceneId = scene.id;
                    pipelineNode.parentNodeId = parentId;

                    if(auto trigger = node->trigger.instance.lock())
                    {
                        pipelineNode.trigger = node->trigger.instance;
                    }

                    if(auto controller = node->controllerInstance.instance.lock())
                    {
                        pipelineNode.controller = node->controllerInstance.instance;
                    }
                    else if(!node->controller.empty())
                    {
                        if(auto factory = _registry.get<IObjectFactory<IController>>(containers::hash::hash(node->controller)))
                        {
                            pipelineNode.controller = factory->createObject(nullptr);
                        }
                    }

                    const auto id = pipelineRepo->addNode(pipelineNode);
                    node->pipelineNodeId = id;

                    if(pipelineNode.trigger)
                    {
                        if(auto trigger = pipelineNode.trigger->lock())
                        {
                            trigger->initialize(id, context);
                        }
                    }

                    if(pipelineNode.controller)
                    {
                        if(auto controller = pipelineNode.controller->lock())
                        {
                            controller->initialize(id, context);
                        }
                    }

                    for(auto& [childName, childNode] : node->children)
                    {
                        stack.emplace(id, &childName, &childNode);
                    }
                }
            }
        }
    }

    void PipelineService::destroyPipeline(const scene::Scene& scene, const scene::configuration::Pipeline& pipeline, DataContext& context)
    {
        if(auto pipelineRepo = _registry.get<IPipelineRepository>())
        {
            std::stack<const scene::configuration::Node*> hierarchy;
            std::stack<const scene::configuration::Node*> stack;

            //Build the hierarchy of nodes
            for(const auto& [key, node] : pipeline.nodes)
            {
                stack.push(&node);

                while(!stack.empty())
                {
                    auto currentNode = stack.top();
                    stack.pop();

                    hierarchy.push(currentNode);

                    for(const auto& [childKey, childNode] : currentNode->children)
                    {
                        stack.push(&childNode);
                    }
                }
            }

            //Traverse the tree from bottom up
            while(!hierarchy.empty())
            {
                const auto currentNode = hierarchy.top();
                hierarchy.pop();

                pipelineRepo->removeNode(currentNode->pipelineNodeId);
            }
        }
    }
}

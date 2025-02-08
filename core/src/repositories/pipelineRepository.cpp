#include <dory/core/repositories/pipelineRepository.h>
#include <stack>

namespace dory::core::repositories
{
    std::span<PipelineRepository::EntityType> PipelineRepository::getPipelineNodes()
    {
        return std::span<PipelineRepository::EntityType>{ _nodes };
    }

    PipelineRepository::IdType PipelineRepository::addNode(const EntityType& pipelineNode)
    {
        auto nodeId = IdType {};

        if(pipelineNode.parentNodeId != resources::nullId)
        {
            auto parentId = pipelineNode.parentNodeId;
            std::stack<IdType> tree;

            auto it = _nodes.begin();
            const auto end = _nodes.end();
            while (it != end)
            {
                if(tree.empty())
                {
                    if(it->id == parentId)
                    {
                        tree.emplace(parentId);
                    }
                }
                else
                {
                    while(!tree.empty())
                    {
                        if(tree.top() == it->parentNodeId)
                        {
                            tree.emplace(it->id);
                            break;
                        }

                        tree.pop();
                    }

                    if(tree.empty())
                    {
                        nodeId = insertNode(pipelineNode, it);
                        break;
                    }
                }

                ++it;
            }

            if(!tree.empty())
            {
                nodeId = insertNode(pipelineNode);
            }
        }
        else
        {
            nodeId = insertNode(pipelineNode);
        }

        return nodeId;
    }

    IPipelineRepository::IdType PipelineRepository::addTriggerNode(IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, const EntityType::UpdateTriggerType& updateTrigger)
    {
        const auto triggerHandle = generic::extension::ResourceHandle{ libraryHandle, updateTrigger };
        return addNode(EntityType{ {}, triggerHandle, parentNode });
    }

    IPipelineRepository::IdType PipelineRepository::addNode(const IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, const EntityType::ControllerPointerType controller)
    {
        const auto controllerHandle = generic::extension::ResourceHandle{ libraryHandle, controller };
        return addNode(EntityType{ controllerHandle, parentNode });
    }

    IPipelineRepository::IdType PipelineRepository::addNode(const IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, const EntityType::ControllerPointerType controller, const EntityType::UpdateTriggerType& updateTrigger)
    {
        const auto controllerHandle = generic::extension::ResourceHandle{ libraryHandle, controller };
        const auto triggerHandle = generic::extension::ResourceHandle{ libraryHandle, updateTrigger };
        return addNode(EntityType{ controllerHandle, triggerHandle, parentNode });
    }

    IPipelineRepository::IdType PipelineRepository::addNode(const IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, const EntityType::UpdateFunctionType& updateFunction)
    {
        const auto updateHandle = generic::extension::ResourceHandle{ libraryHandle, updateFunction };
        return addNode(EntityType{ updateHandle, parentNode });
    }

    IPipelineRepository::IdType PipelineRepository::addNode(const IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, const EntityType::UpdateFunctionType& updateFunction, const EntityType::UpdateTriggerType& updateTrigger)
    {
        const auto updateHandle = generic::extension::ResourceHandle{ libraryHandle, updateFunction };
        const auto triggerHandle = generic::extension::ResourceHandle{ libraryHandle, updateTrigger };
        return addNode(EntityType{ updateHandle, triggerHandle, parentNode });
    }

    PipelineRepository::IdType PipelineRepository::insertNode(const EntityType& node, const NodeListType::iterator& position)
    {
        auto newNode = _nodes.emplace(position, node);
        if(newNode != _nodes.end())
        {
            return newNode->id = _counter++;
        }

        return {};
    }

    PipelineRepository::IdType PipelineRepository::insertNode(const EntityType& node)
    {
        auto& newNode = _nodes.emplace_back(node);
        return newNode.id = _counter++;
    }

    void PipelineRepository::removeNode(IdType id)
    {
        std::stack<IdType> tree;

        auto it = _nodes.begin();
        auto end = _nodes.end();
        std::optional<NodeListType::iterator> firstNode {};
        while (it != end)
        {
            if(tree.empty())
            {
                if(it->id == id)
                {
                    tree.emplace(id);
                    firstNode = it;
                }
            }
            else
            {
                while(!tree.empty())
                {
                    if(tree.top() == it->parentNodeId)
                    {
                        tree.emplace(it->id);
                        break;
                    }

                    tree.pop();
                }

                if(tree.empty())
                {
                    _nodes.erase(*firstNode, it);
                    break;
                }
            }

            it++;
        }

        if(!tree.empty() && firstNode)
        {
            _nodes.erase(*firstNode, end);
        }
    }

    PipelineRepository::EntityType* PipelineRepository::getNode(const resources::Name& name)
    {
        for(auto& node : _nodes)
        {
            if(node.name == name)
            {
                return &node;
            }
        }

        return nullptr;
    }
}

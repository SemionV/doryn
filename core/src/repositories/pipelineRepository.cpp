#include <dory/core/repositories/pipelineRepository.h>
#include <stack>

namespace dory::core::repositories
{
    std::span<PipelineRepository::EntityType> PipelineRepository::getPipelineNodes()
    {
        return std::span<PipelineRepository::EntityType>{ _nodes };
    }

    PipelineRepository::IdType PipelineRepository::addNode(const PipelineRepository::EntityType& pipelineNode)
    {
        auto nodeId = IdType {};

        if(pipelineNode.parentNodeId != resources::entity::nullId)
        {
            auto parentId = pipelineNode.parentNodeId;
            std::stack<IdType> tree;

            auto it = _nodes.begin();
            auto end = _nodes.end();
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

                it++;
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

    PipelineRepository::IdType PipelineRepository::insertNode(const PipelineRepository::EntityType& node, const NodeListType::iterator& position)
    {
        auto newNode = _nodes.emplace(position, node);
        if(newNode != _nodes.end())
        {
            return newNode->id = _counter++;
        }

        return {};
    }

    PipelineRepository::IdType PipelineRepository::insertNode(const PipelineRepository::EntityType& node)
    {
        auto& newNode = _nodes.emplace_back(node);
        return newNode.id = _counter++;
    }

    PipelineRepository::IdType PipelineRepository::removeNode(IdType id)
    {

    }
}
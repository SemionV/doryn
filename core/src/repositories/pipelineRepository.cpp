#include <dory/core/repositories/pipelineRepository.h>
#include <stack>

namespace dory::core::repositories
{
    std::span<resources::entity::PipelineNode> PipelineRepository::getPipelineNodes()
    {
        return std::span<resources::entity::PipelineNode>{ _nodes };
    }

    PipelineRepository::IdType PipelineRepository::addNode(const resources::entity::PipelineNode& pipelineNode)
    {
        auto nodeId = IdType {};

        if(pipelineNode.parentNodeId != resources::entity::nullId)
        {
            auto parentId = pipelineNode.parentNodeId;
            std::stack<IdType> tree;
            std::optional<NodeListType::iterator> precedingNode {};

            auto it = _nodes.begin();
            auto end = _nodes.end();
            while (it != end)
            {
                if(!precedingNode)
                {
                    if(it->id == parentId)
                    {
                        precedingNode = it;
                        tree.emplace(parentId);
                    }
                }
                else
                {
                    while(!tree.empty())
                    {
                        if(tree.top() == it->parentNodeId)
                        {
                            precedingNode = it;
                            tree.emplace(it->id);
                            break;
                        }

                        tree.pop();
                    }

                    if(tree.empty())
                    {
                        break;
                    }
                }
            }

            if(precedingNode)
            {
                nodeId = insertNode(pipelineNode, *precedingNode);
            }
        }
        else
        {
            nodeId = insertNode(pipelineNode, {});
        }

        return nodeId;
    }

    PipelineRepository::IdType PipelineRepository::insertNode(const resources::entity::PipelineNode& node, const NodeListType::iterator& after)
    {
        auto nodeId = IdType {};

        auto newNode = _nodes.emplace(after, node);
        if(newNode != _nodes.end())
        {
            newNode->id = nodeId = _counter++;
        }

        return nodeId;
    }

    PipelineRepository::IdType PipelineRepository::insertNode(const resources::entity::PipelineNode& node)
    {
        auto newNode = _nodes.emplace_back(node);
        return newNode.id = _counter++;
    }

    PipelineRepository::IdType PipelineRepository::removeNode(IdType id)
    {

    }
}
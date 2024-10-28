#include <dory/core/repositories/pipelineRepository.h>

namespace dory::core::repositories
{
    std::span<resources::entity::PipelineNode> PipelineRepository::getPipelineNodes()
    {
        return std::span<resources::entity::PipelineNode>{ _nodes };
    }

    bool PipelineRepository::addNode(const resources::entity::PipelineNode& pipelineNode)
    {
        auto parentId = pipelineNode.parentNodeId;
        std::optional<decltype(_nodes)::iterator> precedingNode {};

        if(pipelineNode.parentNodeId != resources::entity::nullId)
        {
            auto it = _nodes.begin();
            auto end = _nodes.end();
            while (it != end)
            {
                if(!precedingNode)
                {
                    if(it->id == parentId)
                    {
                        precedingNode = it;
                    }
                }
                else if(it->parentNodeId == parentId)
                {
                    precedingNode = it;
                }
                else
                {
                    break;
                }
            }

            if(precedingNode)
            {
                _nodes.emplace(*precedingNode, pipelineNode);
                return true;
            }
        }
        else
        {
            _nodes.emplace_back(pipelineNode);
            return true;
        }

        return false;
    }
}
#include "pipelineService.h"

namespace dory::domain::services
{
    bool compareNodes(std::shared_ptr<object::PipelineNode> a, std::shared_ptr<object::PipelineNode> b)
    {
        return a->nodeEntity.priority < b->nodeEntity.priority;
    }

    std::list<std::shared_ptr<object::PipelineNode>> PipelineService::getPipeline()
    {
        std::list<std::shared_ptr<object::PipelineNode>> nodes;

        nodeReader->getTraverseIterator().forEach([this, &nodes](const entity::PipelineNode& nodeEntity)
        {
            if(nodeEntity.parentNodeId == dory::entity::nullId)
            {
                nodes.emplace_back(loadNode(nodeEntity));
            }
        });

        nodes.sort(compareNodes);

        return nodes;
    }

    std::shared_ptr<object::PipelineNode> PipelineService::loadNode(const entity::PipelineNode& nodeEntity)
    {
        auto node = std::make_shared<object::PipelineNode>(nodeEntity);

        nodeReader->getTraverseIterator().forEach([this, &node](const auto& nodeEntity)
        {
            if(nodeEntity.parentNodeId == node->nodeEntity.id)
            {
                node->children.emplace_back(loadNode(nodeEntity));
            }
        });

        node->children.sort(compareNodes);

        return node;
    }
}
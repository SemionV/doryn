#pragma once

#include "base/domain/object.h"
#include "base/domain/entityRepository.h"
#include "base/doryExport.h"

namespace dory::domain::services
{
    bool compareNodes(const std::shared_ptr<object::PipelineNode>& a, const std::shared_ptr<object::PipelineNode>& b)
    {
        return a->nodeEntity.priority < b->nodeEntity.priority;
    }

    template<typename TServiceLocator>
    class PipelineService: Service<TServiceLocator>
    {
    public:
        std::list<std::shared_ptr<object::PipelineNode>> getPipeline()
        {
            std::list<std::shared_ptr<object::PipelineNode>> nodes;

            this->services.pipelineNodeRepository.forEach([this, &nodes](const entity::PipelineNode& nodeEntity)
            {
                if(nodeEntity.parentNodeId == dory::entity::nullId)
                {
                    nodes.emplace_back(loadNode(nodeEntity));
                }
            });

            nodes.sort(compareNodes);

            return nodes;
        }

    private:
        std::shared_ptr<object::PipelineNode> loadNode(const entity::PipelineNode& nodeEntity)
        {
            auto node = std::make_shared<object::PipelineNode>(nodeEntity);

            this->services.pipelineNodeRepository.forEach([this, &node](const auto& nodeEntity)
            {
                if(nodeEntity.parentNodeId == node->nodeEntity.id)
                {
                    node->children.emplace_back(loadNode(nodeEntity));
                }
            });

            node->children.sort(compareNodes);

            return node;
        }
    };
}
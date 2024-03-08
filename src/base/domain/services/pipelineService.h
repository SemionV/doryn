#pragma once

#include "base/domain/object.h"
#include "base/domain/entityRepository.h"
#include "base/doryExport.h"

namespace dory::domain::services
{
    template<typename T>
    bool compareNodes(const std::shared_ptr<T>& a, const std::shared_ptr<T>& b)
    {
        return a->nodeEntity.priority < b->nodeEntity.priority;
    }

    template<typename TServiceLocator>
    class PipelineService: Service<TServiceLocator>
    {
    public:
        explicit PipelineService(TServiceLocator& serviceLocator):
                Service<TServiceLocator>(serviceLocator)
        {}

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

            nodes.sort(compareNodes<object::PipelineNode>);

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

            node->children.sort(compareNodes<object::PipelineNode>);

            return node;
        }
    };
}
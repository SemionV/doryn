#include <dory/core/repositories/pipelineRepository.h>

namespace dory::core::repositories
{
    template<typename T>
    bool compareNodes(const std::shared_ptr<T>& a, const std::shared_ptr<T>& b)
    {
        return a->nodeEntity.priority < b->nodeEntity.priority;
    }

    std::list<std::shared_ptr<resources::object::PipelineNode>> PipelineRepository::getPipeline()
    {
        std::list<std::shared_ptr<resources::object::PipelineNode>> nodes;

        for(const auto& nodeEntity : this->container)
        {
            if(nodeEntity.parentNodeId == resources::entity::nullId)
            {
                nodes.emplace_back(loadNode(nodeEntity));
            }
        }

        nodes.sort(compareNodes<resources::object::PipelineNode>);

        return nodes;
    }

    std::shared_ptr<resources::object::PipelineNode> PipelineRepository::loadNode(const PipelineRepository::EntityType& nodeEntity)
    {
        auto node = std::make_shared<resources::object::PipelineNode>(nodeEntity);

        for(const auto& entity : this->container)
        {
            if(entity.parentNodeId == node->nodeEntity.id)
            {
                node->children.emplace_back(loadNode(entity));
            }
        }

        node->children.sort(compareNodes<resources::object::PipelineNode>);

        return node;
    }
}
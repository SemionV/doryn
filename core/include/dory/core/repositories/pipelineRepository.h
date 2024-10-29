#pragma once

#include <dory/core/repositories/iPipelineRepository.h>
#include <dory/core/repository.h>

namespace dory::core::repositories
{
    class PipelineRepository: public IPipelineRepository, public repository::Repository<resources::entity::PipelineNode>
    {
    private:
        using EntityType = resources::entity::PipelineNode;
        using IdType = EntityType::IdType;
        using NodeListType = std::vector<EntityType>;
        NodeListType _nodes;
        IdType _counter {};

        IdType insertNode(const resources::entity::PipelineNode& node, const NodeListType::iterator& after);
        IdType insertNode(const resources::entity::PipelineNode& node);

    public:
        PipelineRepository() = default;

        PipelineRepository(std::initializer_list<EntityType>&& entities):
                _nodes(entities)
        {}

        std::span<resources::entity::PipelineNode> getPipelineNodes() override;
        resources::entity::PipelineNode::IdType addNode(const resources::entity::PipelineNode& pipelineNode) override;
        resources::entity::PipelineNode::IdType removeNode(IdType id) override;
    };
}
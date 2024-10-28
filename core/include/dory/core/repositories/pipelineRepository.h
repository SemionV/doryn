#pragma once

#include <dory/core/repositories/iPipelineRepository.h>
#include <dory/core/repository.h>

namespace dory::core::repositories
{
    class PipelineRepository: public IPipelineRepository, public repository::Repository<resources::entity::PipelineNode>
    {
    private:
        using EntityType = resources::entity::PipelineNode;
        std::vector<EntityType> _nodes;

    public:
        PipelineRepository() = default;

        PipelineRepository(std::initializer_list<EntityType>&& entities):
                _nodes(entities)
        {}

        std::span<resources::entity::PipelineNode> getPipelineNodes() override;
        bool addNode(const resources::entity::PipelineNode& pipelineNode) override;
    };
}
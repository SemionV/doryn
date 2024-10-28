#pragma once

#include <dory/core/repositories/iPipelineRepository.h>
#include <dory/core/repository.h>

namespace dory::core::repositories
{
    class PipelineRepository: public IPipelineRepository, public repository::Repository<resources::entity::PipelineNode>
    {
    private:
        using EntityType = resources::entity::PipelineNode;

        std::shared_ptr<resources::object::PipelineNode> loadNode(const EntityType& nodeEntity);

    public:
        PipelineRepository() = default;

        PipelineRepository(std::initializer_list<EntityType>&& entities):
                repository::Repository<resources::entity::PipelineNode>(std::move(entities))
        {}

        std::list<std::shared_ptr<resources::object::PipelineNode>> getPipeline() override;
    };
}
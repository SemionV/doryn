#pragma once

#include <dory/core/repositories/iPipelineRepository.h>
#include <dory/core/repository.h>

namespace dory::core::repositories
{
    class PipelineRepository: public IPipelineRepository, public repository::Repository<resources::entity::PipelineNode<resources::DataContext>>
    {
    private:
        using EntityType = resources::entity::PipelineNode<resources::DataContext>;

        std::shared_ptr<resources::object::PipelineNode<resources::DataContext>> loadNode(const EntityType& nodeEntity);

    public:
        PipelineRepository() = default;

        PipelineRepository(std::initializer_list<EntityType>&& entities):
                repository::Repository<resources::entity::PipelineNode<resources::DataContext>>(std::move(entities))
        {}

        std::list<std::shared_ptr<resources::object::PipelineNode<resources::DataContext>>> getPipeline() override;
    };
}
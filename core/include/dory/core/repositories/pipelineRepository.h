#pragma once

#include <dory/core/repositories/iPipelineRepository.h>
#include <dory/core/generic/repository.h>

namespace dory::core::repositories
{
    class PipelineRepository: public IPipelineRepository, public generic::repository::Repository<resources::entity::PipelineNode<resources::DataContext>>
    {
    private:
        using EntityType = resources::entity::PipelineNode<resources::DataContext>;

        std::shared_ptr<resources::object::PipelineNode<resources::DataContext>> loadNode(const EntityType& nodeEntity);

    public:
        PipelineRepository() = default;

        PipelineRepository(std::initializer_list<EntityType>&& entities):
                generic::repository::Repository<resources::entity::PipelineNode<resources::DataContext>>(std::move(entities))
        {}

        std::list<std::shared_ptr<resources::object::PipelineNode<resources::DataContext>>> getPipeline() override;
    };
}
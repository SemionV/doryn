#pragma once

#include <dory/core/repositories/iBlockStreamRepository.h>
#include <dory/core/resources/blockStreamQueue.h>
#include <dory/core/repository.h>

namespace dory::core::repositories
{
    class BlockStreamRepository final : public EntityRepository<std::shared_ptr<resources::entities::BlockStreamQueue>, resources::IdType, IBlockStreamRepository>
    {
    public:
        EntityType* get(IdType id) override;
        EntityType* insert(const EntityType& scene) override;
        EntityType* insert(EntityType&& scene) override;
        void each(std::function<void(EntityType& entity)> predicate) override;
        EntityType* scan(std::function<bool(EntityType& entity)> predicate) override;
    };
}

#pragma once

#include <dory/core/repositories/iImageStreamRepository.h>
#include <dory/core/resources/imageStreamQueue.h>
#include <dory/core/repository.h>

namespace dory::core::repositories
{
    class ImageStreamRepository final : public EntityRepository<std::shared_ptr<resources::entities::ImageStreamQueue>, resources::IdType, IImageStreamRepository>
    {
    public:
        EntityType* get(IdType id) override;
        EntityType* insert(const EntityType& scene) override;
        EntityType* insert(EntityType&& scene) override;
        void each(std::function<void(EntityType& entity)> predicate) override;
        EntityType* scan(std::function<bool(EntityType& entity)> predicate) override;
    };
}

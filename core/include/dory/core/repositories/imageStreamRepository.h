#pragma once

#include <dory/core/repositories/iImageStreamRepository.h>
#include <dory/core/resources/imageStreamQueue.h>
#include <dory/core/repository.h>

namespace dory::core::repositories
{
    class ImageStreamRepository final : public EntityRepository<std::shared_ptr<resources::entities::ImageStreamQueue>, resources::IdType, IImageStreamRepository>
    {
    public:
        EntityType* get(IdType id) final;
        EntityType* create() final;
        EntityType* insert(const EntityType& scene) final;
        EntityType* insert(EntityType&& scene) final;
        void each(std::function<void(EntityType& entity)> predicate) final;
        EntityType* scan(std::function<bool(EntityType& entity)> predicate) final;
    };
}

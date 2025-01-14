#pragma once

#include <dory/core/repositories/iSceneRepository.h>
#include <dory/core/resources/scene/enttScene.h>
#include <dory/core/repository.h>

namespace dory::core::repositories
{
    class EnttSceneRepository: public EntityRepository<std::shared_ptr<resources::scene::EnttScene>, resources::IdType, ISceneRepository>
    {
    public:
        ISceneRepository::EntityType* get(ISceneRepository::IdType id) final;
        ISceneRepository::EntityType* create() final;
        ISceneRepository::EntityType* insert(const ISceneRepository::EntityType& scene) final;
        ISceneRepository::EntityType* insert(ISceneRepository::EntityType&& scene) final;
        void each(std::function<void(ISceneRepository::EntityType& entity)> predicate) final;
        ISceneRepository::EntityType* scan(std::function<bool(ISceneRepository::EntityType& entity)> predicate) final;
    };
}
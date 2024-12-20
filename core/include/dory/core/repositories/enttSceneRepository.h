#pragma once

#include <dory/core/repositories/iSceneRepository.h>
#include <dory/core/resources/scene/enttScene.h>
#include <dory/core/repository.h>

namespace dory::core::repositories
{
    class EnttSceneRepository: public EntityRepository<std::shared_ptr<resources::scene::EnttScene>, resources::IdType, ISceneRepository>
    {
    public:
        ISceneRepository::EntityType* get(ISceneRepository::IdType id) override
        {
            if(container.contains(id))
            {
                return container[id].get();
            }

            return {};
        }

        ISceneRepository::EntityType* insert(const ISceneRepository::EntityType& scene) override
        {
            return insert((resources::scene::Scene&&)scene);
        }

        ISceneRepository::EntityType* insert(ISceneRepository::EntityType&& scene) override
        {
            auto id = getNewId();

            auto enttScene = std::make_shared<resources::scene::EnttScene>(scene);
            if(enttScene)
            {
                enttScene->id = id;
                container[id] = enttScene;
                return enttScene.get();
            }

            return nullptr;
        }

        void each(std::function<void(ISceneRepository::EntityType& entity)> predicate) override
        {
            for(auto& [id, entity] : container)
            {
                predicate(*entity);
            }
        }

        ISceneRepository::EntityType* scan(std::function<bool(ISceneRepository::EntityType& entity)> predicate) override
        {
            for(auto& [id, entity] : container)
            {
                if(predicate(*entity))
                {
                    return entity.get();
                }
            }

            return nullptr;
        }
    };
}
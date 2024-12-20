#include <dory/core/repositories/enttSceneRepository.h>

namespace dory::core::repositories
{
    ISceneRepository::EntityType* EnttSceneRepository::get(ISceneRepository::IdType id)
    {
        if(container.contains(id))
        {
            return container[id].get();
        }

        return {};
    }

    ISceneRepository::EntityType* EnttSceneRepository::insert(const resources::scene::Scene& scene)
    {
        return insert((resources::scene::Scene&&)scene);
    }

    ISceneRepository::EntityType* EnttSceneRepository::insert(resources::scene::Scene&& scene)
    {
        auto id = getNewId();

        auto enttScene = std::make_shared<resources::scene::EnttScene>(scene);
        if(enttScene)
        {
            enttScene->id = id;
            enttScene->ecsType = resources::EcsType::entt;
            container[id] = enttScene;
            return enttScene.get();
        }

        return nullptr;
    }

    void EnttSceneRepository::each(std::function<void(ISceneRepository::EntityType&)> predicate)
    {
        for(auto& [id, entity] : container)
        {
            predicate(*entity);
        }
    }

    ISceneRepository::EntityType* EnttSceneRepository::scan(std::function<bool(ISceneRepository::EntityType&)> predicate)
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
}
#pragma once

#include <dory/core/implementation.h>
#include <dory/core/services/iSceneService.h>
#include <entt/entt.hpp>
#include <dory/core/resources/scene/enttScene.h>

namespace dory::core::services
{
    template<typename T, typename TPolicy, typename TState>
    class EntityComponentServiceGeneric: public implementation::ImplementationLevel<TPolicy, TState>
    {
    public:
        void addComponent(resources::IdType objectId, resources::scene::Scene& scene, const T& component) final
        {
            auto& enttScene = (resources::scene::EnttScene&)scene;
            auto& registry = enttScene.registry;

            if(enttScene.idMap.contains(objectId))
            {
                auto entity = enttScene.idMap[objectId];
                registry.emplace<T>(entity, component);
            }
        }

        void removeComponent(resources::IdType objectId, resources::scene::Scene& scene, const T& component) final
        {
            auto& enttScene = (resources::scene::EnttScene&)scene;
            auto& registry = enttScene.registry;

            if(enttScene.idMap.contains(objectId))
            {
                auto entity = enttScene.idMap[objectId];
                registry.remove<T>(entity);
            }
        }
    };

    struct EntityComponentServicePolicy: implementation::ImplementationPolicy<implementation::ImplementationList<EntityComponentServiceGeneric>>
    {};

    using EntityComponentService = implementation::Implementation<generic::TypeList<ISceneService>, ISceneService::ComponentTypes, EntityComponentServicePolicy>;

    class EnttSceneService: public EntityComponentService
    {
    private:
        static void destroyObjectEntity(resources::scene::Scene& scene, entt::entity entity);

    public:
        resources::IdType addObject(resources::scene::Scene& scene, const resources::objects::SceneObject& object) override;
        void deleteObject(resources::scene::Scene& scene, resources::IdType objectId) override;
    };
}

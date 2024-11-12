#pragma once

#include <dory/core/services/iSceneService.h>
#include <entt/entt.hpp>

namespace dory::core::services
{
    class EnttSceneService: public ISceneService
    {
    private:
        static void destroyObjectEntity(resources::scene::Scene& scene, entt::entity entity);

    public:
        resources::IdType addObject(resources::scene::Scene& scene, const resources::objects::SceneObject& object) override;
        void deleteObject(resources::scene::Scene& scene, resources::IdType objectId) override;
    };
}

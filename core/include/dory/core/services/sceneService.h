#pragma once

#include <dory/core/services/iSceneService.h>

namespace dory::core::services
{
    class SceneService: public ISceneService
    {
    private:
        static void cleanupSlot(resources::scene::Scene& scene, std::size_t slot);

    public:
        resources::IdType addObject(resources::scene::Scene& scene, const resources::objects::SceneObject& object) override;
        void deleteObject(resources::scene::Scene& scene, resources::IdType objectId) override;
    };
}

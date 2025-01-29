#pragma once

#include <dory/core/dependencyResolver.h>
#include <dory/core/services/iSceneConfigurationService.h>

namespace dory::core::services
{
    class SceneConfigurationService: public ISceneConfigurationService, public DependencyResolver
    {
    public:
        explicit SceneConfigurationService(Registry& registry);

        void load(const std::filesystem::path& filename, resources::scene::configuration::Scene& configuration) final;
        void save(const std::filesystem::path& filename, const resources::scene::configuration::Scene& configuration) final;
    };
}

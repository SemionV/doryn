#pragma once

#include <dory/core/services/iSceneDirector.h>
#include <dory/core/dependencyResolver.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services::scene::directors
{
    class ApplicationDirector: public ISceneDirector, public DependencyResolver
    {
    private:
        void attachScrips(const generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context) const;
        void onApplicationExit(core::resources::DataContext& context, const core::events::application::Exit& eventData) const;
        void onWindowClose(core::resources::DataContext& context, const core::events::window::Close& eventData) const;
        void onFilesystemEvent(core::resources::DataContext& context, const core::events::filesystem::FileModified& event) const;

    public:
        explicit ApplicationDirector(Registry& registry);

        void initialize(resources::scene::Scene& scene, resources::scene::configuration::SceneConfiguration& sceneConfig, resources::DataContext& context) final;
        void destroy(resources::scene::Scene& scene, resources::scene::configuration::SceneConfiguration& sceneConfig, resources::DataContext& context) final;
    };
}

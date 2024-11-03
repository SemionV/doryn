#pragma once

#include <dory/core/registry.h>
#include <dory/core/iBootstrap.h>
#include <dory/core/resources/localizationImpl.h>
#include <dory/core/resources/configuration.h>

namespace dory::game
{
    class Bootstrap: public core::IBootstrap
    {
    private:
        core::Registry& _registry;

        void attachEventHandlers(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context);
        void loadConfiguration(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context);
        void connectDevices(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context);
        void buildPipeline(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context);
        void attachScrips(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context);
        void loadExtensions(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context);

        void onApplicationExit(core::resources::DataContext& context, const core::events::application::Exit& eventData);
        void onWindowClose(core::resources::DataContext& context, const core::events::window::Close& eventData);

    public:
        explicit Bootstrap(core::Registry& registry);

        bool initialize(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context) final;
        bool run(core::resources::DataContext& context) final;
        void cleanup(core::resources::DataContext& context) final;
    };
}

#pragma once

#include <dory/core/iBootstrap.h>
#include <dory/core/resources/dataContext.h>
#include <dory/core/events/eventTypes.h>
#include <dory/core/resources/configuration.h>
#include <dory/generic/macros.h>

namespace dory::core
{
    class Registry;
}

namespace dory::game
{
    class DORY_DLLEXPORT Bootstrap: public core::IBootstrap
    {
    private:
        core::Registry& _registry;

        void attachEventHandlers(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context);
        void attachScrips(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context);
        void loadExtensions(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context);

        void onApplicationExit(core::resources::DataContext& context, const core::events::application::Exit& eventData);
        void onWindowClose(core::resources::DataContext& context, const core::events::window::Close& eventData);
        void onFilesystemEvent(core::resources::DataContext& context, const core::events::filesystem::FileModified& event);

    public:
        explicit Bootstrap(core::Registry& registry);

        bool initialize(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context) final;
        bool run(core::resources::DataContext& context) final;
        void cleanup(core::resources::DataContext& context) final;
    };
}

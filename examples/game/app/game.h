#pragma once

#include <dory/core/registry.h>

namespace dory::game
{
    class Game
    {
    private:
        core::Registry& _registry;

    public:
        explicit Game(core::Registry& registry):
            _registry(registry)
        {}

        bool initialize(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context)
        {
            _registry.get<dory::core::services::IWindowService>([&context](dory::core::services::IWindowService* windowService) {
                context.mainWindowId = windowService->createWindow({800, 600, "dory game"});
            });

            _registry.get<dory::core::repositories::IViewRepository>([&context](dory::core::repositories::IViewRepository* viewRepository) {
                auto view = dory::core::resources::entity::View{dory::core::resources::nullId, context.mainWindowId};
                viewRepository->insert(view);
            });

            _registry.get<
                    generic::registry::Service<core::repositories::IFileWatchRepository>,
                    generic::registry::Service<core::devices::IFileWatcherDevice, core::resources::FileWatchSystem::entropia>>(
            [](core::repositories::IFileWatchRepository* fileWatchRepository,
               core::devices::IFileWatcherDevice* fileWatcherDevice)
            {
                auto fileWatch = core::resources::entity::FileWatch {
                        {},
                        "modules/",
                        "renderer-opengl.so",
                        core::resources::AssetType::ExtensionLibrary,
                        core::resources::FileWatchSystem::entropia
                };
                fileWatchRepository->insert(fileWatch);

                fileWatcherDevice->updateWatches();
            });

            _registry.get<core::services::IScriptService>([this, &libraryHandle](core::services::IScriptService* scriptService){
                scriptService->addScript("load-renderer", libraryHandle, [this](core::resources::DataContext& context, const std::map<std::string, std::any>& arguments){
                    auto libraryService = _registry.get<core::services::ILibraryService>();
                    if(libraryService)
                    {
                        bool isCommandMode {};
                        _registry.get<core::devices::ITerminalDevice>([&isCommandMode](core::devices::ITerminalDevice* terminal) {
                            isCommandMode = terminal->isCommandMode();
                            if(isCommandMode)
                            {
                                terminal->exitCommandMode();
                            }
                        });

                        libraryService->unload("RendererOpengl");
                        auto openglLibrary = libraryService->load("RendererOpengl", "modules/renderer-opengl");
                        if(openglLibrary)
                        {
                            auto extension = openglLibrary->loadModule("renderer-opengl", _registry);
                            if(extension)
                            {
                                extension->attach(generic::extension::LibraryHandle{ openglLibrary }, context);
                            }
                        }

                        _registry.get<core::devices::ITerminalDevice>([&isCommandMode](core::devices::ITerminalDevice* terminal) {
                            if(isCommandMode)
                            {
                                terminal->enterCommandMode();
                            }
                        });
                    }
                });
            });

            return true;
        }
    };
}

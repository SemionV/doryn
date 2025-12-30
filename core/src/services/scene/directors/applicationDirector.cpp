#include <dory/core/registry.h>
#include <dory/core/services/scene/directors/applicationDirector.h>
#include <spdlog/fmt/fmt.h>
#include <../../../../../components/data-structures/include/dory/data-structures/hashId.h>

namespace dory::core::services::scene::directors
{
    using namespace resources;
    using namespace resources::scene;
    using namespace resources::scene::configuration;

    ApplicationDirector::ApplicationDirector(Registry& registry):
        DependencyResolver(registry)
    {}

    void ApplicationDirector::initialize(Scene& scene, SceneConfiguration& sceneConfig, DataContext& context)
    {
        if(auto listener = _registry.get<events::application::Bundle::IListener>())
        {
            listener->attach([this](auto& ctx, const auto& event) {
                this->onApplicationExit(ctx, event);
            });
        }

        if(auto listener = _registry.get<events::window::Bundle::IListener>())
        {
            listener->attach([this](auto& ctx, const events::window::Close& event) {
                this->onWindowClose(ctx, event);
            });
        }

        if(auto listener = _registry.get<events::filesystem::Bundle::IListener>())
        {
            listener->attach([this](auto& ctx, const auto& event) {
                this->onFilesystemEvent(ctx, event);
            });
        }
    }

    void ApplicationDirector::destroy(Scene& scene, SceneConfiguration& sceneConfig, DataContext& context)
    {
        if(auto terminalDevice = _registry.get<devices::ITerminalDevice>())
        {
            terminalDevice->exitCommandMode();
        }

        if(auto logger = _registry.get<ILogService, Logger::App>())
        {
            logger->information(std::string_view{ "Cleanup..." });
        }
    }

    void ApplicationDirector::onApplicationExit(DataContext& context, const events::application::Exit& eventData) const
    {
        _registry.get<ILoopService>([](ILoopService* frameService) {
            frameService->endLoop();
        });
    }

    void ApplicationDirector::onWindowClose(DataContext& context, const events::window::Close& eventData) const
    {
        _registry.get<IWindowService>([this, &eventData, &context](IWindowService* windowService) {
            windowService->closeWindow(eventData.windowId, eventData.windowSystem);

            if(eventData.windowId == context.mainWindowId)
            {
                if(auto dispatcher = _registry.get<events::application::Bundle::IDispatcher>())
                {
                    dispatcher->fire(context, events::application::Exit{});
                }
            }
        });
    }

    void ApplicationDirector::onFilesystemEvent(DataContext& context, const events::filesystem::FileModified& event) const
    {
        if(auto resolver = _registry.get<IAssetTypeResolver>())
        {
            if(const auto assetType = resolver->resolve(context, event.filePath))
            {
                _registry.get<IAssetReloadHandler>(data_structures::hash::hash(*assetType), [&context, &event](IAssetReloadHandler* assetLoader) {
                    assetLoader->reload(context, event.filePath);
                });
            }
        }
    }

    void ApplicationDirector::attachScrips(const generic::extension::LibraryHandle& libraryHandle, DataContext& context) const
    {
        if(auto scriptService = _registry.get<IScriptService>())
        {
            scriptService->addScript("exit", libraryHandle, [this](DataContext& context, const std::map<std::string, std::any>& arguments) {

                auto terminalDevice = _registry.get<devices::ITerminalDevice>();
                auto applicationDispatcher = _registry.get<events::application::Bundle::IDispatcher>();

                if(terminalDevice && applicationDispatcher)
                {
                    terminalDevice->writeLine(fmt::format("-\u001B[31m{0}\u001B[0m-", "exit"));
                    applicationDispatcher->fire(context, events::application::Exit{});
                }
            });
        }
    }
}

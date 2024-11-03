#include <dory/core/registry.h>
#include <dory/core/services/loaders/extensionLoader.h>

namespace dory::core::services::loaders
{
    ExtensionLoader::ExtensionLoader(Registry &registry):
        _registry(registry)
    {}

    bool ExtensionLoader::load(resources::DataContext &context, const std::filesystem::path &filePath)
    {
        _registry.get<core::services::ILibraryService>([this, &context, &filePath](core::services::ILibraryService* libraryService) {
            for(const auto& extension : context.configuration.extensions)
            {
                auto extensionPath = std::filesystem::path {extension.path}.replace_extension(generic::extension::ILibrary::systemSharedLibraryFileExtension);
                std::error_code errorCode;
                if(std::filesystem::equivalent(extensionPath, filePath, errorCode) && extension.reloadOnModification)
                {
                    if(!libraryService->isLoaded(extension.name) || extension.reloadOnModification)
                    {
                        bool isCommandMode {};
                        auto terminal = _registry.get<core::devices::ITerminalDevice>();
                        if(terminal)
                        {
                            isCommandMode = terminal->isCommandMode();
                            if(isCommandMode)
                            {
                                terminal->exitCommandMode();
                            }
                        }

                        auto library = libraryService->load(extension.name, extension.path);
                        if(library)
                        {
                            for(const auto& moduleName : extension.modules)
                            {
                                auto module = library->loadModule(moduleName, _registry);
                                if(module)
                                {
                                    module->attach(generic::extension::LibraryHandle{ library }, context);
                                }
                            }
                        }

                        if(terminal && isCommandMode)
                        {
                            terminal->enterCommandMode();
                        }
                    }
                }
            }
        });

        return true;
    }
}

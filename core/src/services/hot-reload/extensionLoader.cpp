#include <dory/core/registry.h>
#include <dory/core/services/hot-reload/extensionLoader.h>

namespace dory::core::services::loaders
{
    ExtensionLoader::ExtensionLoader(Registry &registry):
        _registry(registry)
    {}

    bool ExtensionLoader::reload(resources::DataContext &context, const std::filesystem::path &filePath)
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
                        libraryService->load(context, extension);
                    }
                }
            }
        });

        return true;
    }
}

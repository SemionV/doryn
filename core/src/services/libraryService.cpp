#include <dory/core/services/libraryService.h>

namespace dory::core::services
{
    std::shared_ptr<extensionPlatform::ILibrary> LibraryService::load(const std::filesystem::path &libraryName, const std::filesystem::path &libraryPath)
    {
        if(_libraries.contains(libraryName))
        {
            return _libraries[libraryName];
        }

        auto library = std::make_shared<extensionPlatform::DynamicLibrary>(libraryName, libraryPath);
        library->load(libraryPath);

        _libraries[libraryName] = library;

        return library;
    }

    void LibraryService::unload(const std::string &libraryName)
    {
        if(_libraries.contains(libraryName))
        {
            auto library = _libraries[libraryName];
            library->unload();

            _libraries.erase(libraryName);
        }
    }
}
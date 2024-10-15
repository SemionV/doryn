#include <dory/core/services/libraryService.h>

namespace dory::core::services
{
    std::shared_ptr<extensionPlatform::ILibrary> LibraryService::load(const std::filesystem::path &libraryName, const std::filesystem::path &libraryPath)
    {
        const std::string& libraryKey = libraryName.string();
        if(_libraries.contains(libraryKey))
        {
            return _libraries[libraryKey];
        }

        auto library = std::make_shared<extensionPlatform::DynamicLibrary>(libraryKey, libraryPath);
        library->load(libraryPath);

        _libraries[libraryKey] = library;

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
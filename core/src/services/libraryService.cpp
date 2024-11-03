#include <dory/core/services/libraryService.h>

namespace dory::core::services
{
    std::shared_ptr<extension::IDynamicLibrary> LibraryService::load(const std::string& libraryName, const std::filesystem::path &libraryPath)
    {
        if(_libraries.contains(libraryName))
        {
            return _libraries[libraryName];
        }

        auto library = std::make_shared<extension::DynamicLibrary>(libraryName, libraryPath);
        library->load(libraryPath);

        _libraries[libraryName] = library;

        return library;
    }

    void LibraryService::unload(const std::string& libraryName)
    {
        if(_libraries.contains(libraryName))
        {
            auto library = _libraries[libraryName];
            library->unload();

            _libraries.erase(libraryName);
        }
    }

    bool LibraryService::isLoaded(const std::string& libraryName)
    {
        return _libraries.contains(libraryName);
    }
}
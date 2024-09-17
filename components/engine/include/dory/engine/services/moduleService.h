#pragma once

#include "dory/module.h"
#include "logService.h"

namespace dory::engine::services::module
{
    template<typename TImplementation>
    class IDynamicLibraryService: NonCopyable
    {
    private:
        TImplementation& _implementation;

    public:
        explicit IDynamicLibraryService(TImplementation& implementation):
            _implementation(implementation)
        {}

        std::shared_ptr<DynamicLibrary> load(const std::filesystem::path& libraryName, const std::filesystem::path& libraryPath)
        {
            return _implementation.load(libraryName, libraryPath);
        }

        void unload(const std::string& libraryName)
        {
            _implementation.unload(libraryName);
        }
    };

    class DynamicLibraryService
    {
    private:
        std::map<std::string, std::shared_ptr<DynamicLibrary>> _libraries;

    public:
        std::shared_ptr<DynamicLibrary> load(const std::string& libraryName, const std::filesystem::path& libraryPath)
        {
            if(_libraries.contains(libraryName))
            {
                return _libraries[libraryName];
            }

            auto library = std::make_shared<DynamicLibrary>(libraryName, libraryPath);
            library->load(libraryPath);

            _libraries[libraryName] = library;

            return library;
        }

        void unload(const std::string& libraryName)
        {
            if(_libraries.contains(libraryName))
            {
                auto library = _libraries[libraryName];
                library->unload();

                _libraries.erase(libraryName);
            }
        }
    };
}
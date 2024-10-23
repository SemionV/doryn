#pragma once

#include <dory/core/registry.h>
#include <dory/generic/extension/iLibrary.h>
#include <dory/generic/extension/iModule.h>

namespace dory::core::extensionPlatform
{
    class IDynamicLibrary: public ILibrary
    {
    public:
        explicit IDynamicLibrary(const std::string &libraryName, const std::filesystem::path &libraryPath)
                : ILibrary(libraryName, libraryPath)
        {}

        virtual void load(const std::filesystem::path& libraryPath) = 0;
        virtual void unload() = 0;
        virtual std::shared_ptr<IModule> loadModule(const std::string& moduleName, Registry& registry) = 0;
    };
}

    #pragma once

#include <dory/generic/extension/iLibrary.h>
#include <dory/generic/extension/iModule.h>
#include <dory/core/resources/dataContext.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::extension
{
    class IDynamicLibrary: public dory::generic::extension::ILibrary
    {
    public:
        explicit IDynamicLibrary(const std::string &libraryName, const std::filesystem::path &libraryPath)
                : ILibrary(libraryName, libraryPath)
        {}

        virtual void load(const std::filesystem::path& libraryPath) = 0;
        virtual void unload() = 0;
        virtual std::shared_ptr<dory::generic::extension::IModule<resources::DataContext>> loadModule(const std::string& moduleName, Registry& registry) = 0;
    };
}

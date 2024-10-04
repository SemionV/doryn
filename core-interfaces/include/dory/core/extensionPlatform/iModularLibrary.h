#pragma once

#include "module.h"

namespace dory::core::extensionPlatform
{
    template<typename TModuleContext>
    class IModularLibrary: public ILibrary
    {
    public:
        explicit IModularLibrary(const std::string& libraryName, const std::filesystem::path& libraryPath):
                ILibrary(libraryName, libraryPath)
        {}

        virtual std::shared_ptr<IDynamicModule> loadModule(const std::string& moduleName, TModuleContext& moduleContext) = 0;
    };
}
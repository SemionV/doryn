#pragma once

#include "iModularLibrary.h"

namespace dory::core::extensionPlatform
{
    template<typename TModuleContext>
    class IDynamicLibrary: IModularLibrary<TModuleContext>
    {
    public:
        virtual void load(const std::filesystem::path& libraryPath) = 0;
        virtual void unload() = 0;
    };
}

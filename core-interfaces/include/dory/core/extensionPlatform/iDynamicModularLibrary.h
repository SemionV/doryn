#pragma once

#include "iModularLibrary.h"

namespace dory::core::extensionPlatform
{
    template<typename TModuleContext>
    class IDynamicModularLibrary: IModularLibrary<TModuleContext>
    {
    public:
        virtual void load(const std::filesystem::path& libraryPath) = 0;
        virtual void unload() = 0;
    };
}

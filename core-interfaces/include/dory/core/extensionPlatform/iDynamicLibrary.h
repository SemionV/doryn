#pragma once

#include "iLibrary.h"

namespace dory::core::extensionPlatform
{
    class IDynamicLibrary: public ILibrary
    {
    public:
        virtual void load(const std::filesystem::path& libraryPath) = 0;
        virtual void unload() = 0;
    };
}

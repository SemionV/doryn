#pragma once

#include "libraryHandle.h"
#include <dory/core/resources/moduleContext.h>

namespace dory::core::extensionPlatform
{
    class IModule
    {
    public:
        virtual ~IModule() = default;
    };

    class IExtensionModule: public IModule
    {
    public:
        virtual void attach(LibraryHandle library, const resources::ExtensionContext& extensionContext) = 0;
    };

    class IExecutableModule: public IModule
    {
    public:
        virtual int run(const resources::ExecuteContext& moduleContext) = 0;
    };
}

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
        ~IExtensionModule() override = default;
        virtual void attach(LibraryHandle library, const resources::ExtensionContext& extensionContext) = 0;
    };

    class IExecutableModule: public IModule
    {
    public:
        ~IExecutableModule() override = default;
        virtual int run(const resources::ExecuteContext& moduleContext) = 0;
    };
}

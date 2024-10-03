#pragma once

#include "libraryHandle.h"

namespace dory::core::extensionPlatform
{
    class IModule
    {
    public:
        virtual ~IModule() = default;
    };

    class IDynamicModule: public IModule
    {
    public:
        ~IDynamicModule() override = default;
        virtual void attach(LibraryHandle library) = 0;
    };

    template<typename TModuleContext>
    class IExecutableModule: public IModule
    {
    public:
        ~IExecutableModule() override = default;
        virtual int run(TModuleContext& moduleContext) = 0;
    };
}

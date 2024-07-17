#pragma once

#include <dory/sandbox/mainModule/extensionContext.h>
#include <dory/engine.h>

namespace dory::sandbox
{
    class Extension: public IDynamicModule<ExtensionContext>
    {
    public:
        ~Extension() override;

        void attach(LibraryHandle library, ExtensionContext& moduleContext) override;
    };
}
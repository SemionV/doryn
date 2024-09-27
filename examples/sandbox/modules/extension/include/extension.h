#pragma once

#include <dory/sandbox/mainModule/extensionContext.h>
#include <dory/module.h>

namespace dory::sandbox
{
    class Extension: public dory::IDynamicModule
    {
    private:
        ExtensionContext& _extensionContext;

    public:
        explicit Extension(ExtensionContext& extensionContext);
        ~Extension() override;

        void attach(LibraryHandle library) override;

    private:
        void pingScript(Registry::DataContextType& context, const std::map<std::string, std::any>& arguments);
    };
}
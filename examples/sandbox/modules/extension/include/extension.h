#pragma once

#include <dory/sandbox/mainModule/extensionContext.h>
#include <dory/module.h>
#include <dory/engineObjects/events/mainController.h>

namespace dory::sandbox
{
    class Extension: public IDynamicModule<ExtensionContext>
    {
    private:
        ExtensionContext& _extensionContext;

    public:
        explicit Extension(ExtensionContext& extensionContext);
        ~Extension() override;

        void attach(LibraryHandle library) override;

    private:
        void pingScript(Registry::DataContextType& context, const std::map<std::string, std::any>& arguments);
        void onStopEngine(Registry::DataContextType& context, const engine::events::mainController::Stop& eventData);
    };
}
#pragma once

#include <dory/bootstrap.h>
#include "mainModule/extensionContext.h"

namespace dory::sandbox
{
    namespace events = dory::domain::events;

    class MainModule: public IExecutableModule<bootstrap::StartupModuleContext>
    {
    private:
        using DataContextType = Registry::DataContextType;

        Registry registry;
        Registry::ServiceTypes::FrameServiceType frameService;
        dory::domain::services::module::DynamicLibraryService libraryService;
        ExtensionContext extensionContext = ExtensionContext{ registry };

    public:
        MainModule();

        int run(bootstrap::StartupModuleContext& moduleContext) final;

    private:
        void attachEventHandlers();
        void onInitializeEngine(DataContextType& context, const events::engine::Initialize& eventData);
        void onStopEngine(DataContextType& context, const events::engine::Stop& eventData);
        void onApplicationExit(DataContextType& context, const events::application::Exit& eventData);
        void onCloseWindow(DataContextType& context, events::window::Close& eventData);
        void onRunScript(DataContextType& context, const events::script::Run& eventData);
    };
}
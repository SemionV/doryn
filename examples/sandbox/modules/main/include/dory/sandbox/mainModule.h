#pragma once

#include <dory/bootstrap.h>
#include "mainModule/extensionContext.h"

namespace dory::sandbox
{
    namespace events = engine::events;

    class MainModule: public IExecutableModule<bootstrap::StartupModuleContext>
    {
    private:
        using DataContextType = Registry::DataContextType;
        using ConfigurationType = Registry::ConfigurationType;

        Registry registry;
        Registry::ServiceTypes::FrameServiceType frameService;
        engine::services::module::DynamicLibraryService libraryService;
        ExtensionContext extensionContext = ExtensionContext{ registry };

    public:
        int run(bootstrap::StartupModuleContext& moduleContext) final;

    private:
        void attachEventHandlers();
        void onInitializeEngine(DataContextType& context, const events::mainController::Initialize& eventData);
        void onStopEngine(DataContextType& context, const events::mainController::Stop& eventData);
        void onApplicationExit(DataContextType& context, const events::application::Exit& eventData);
        void onCloseWindow(DataContextType& context, events::window::Close& eventData);
        void onRunScript(DataContextType& context, const events::script::Run& eventData);
    };
}
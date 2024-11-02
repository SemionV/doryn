#include "dory/game/setup.h"
#include <iostream>
#include <dory/core/services/logService.h>
#include <dory/core/resources/localizationImpl.h>
#include <dory/game/bootstrap.h>
#include "game.h"

#ifdef DORY_MAIN_FUNCTION_UNIX
int main()
#endif
#ifdef DORY_MAIN_FUNCTION_WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
#endif
{
    dory::game::Setup setup;
    dory::generic::extension::LibraryHandle staticLibraryHandle {};
    auto registry = dory::core::Registry{};
    auto configuration = dory::core::resources::configuration::Configuration{};
    auto localization = dory::core::resources::LocalizationImpl{};
    auto context = dory::core::resources::DataContext{ configuration, localization };

    setup.setupRegistry(staticLibraryHandle, registry);

    //initial config
    configuration.section.loadFrom.emplace_back("settings.yaml");
    auto& bootLoggerConfig = configuration.loggingConfiguration.configurationLogger;
    bootLoggerConfig.name = "boot";
    bootLoggerConfig.rotationLogger = dory::core::resources::configuration::RotationLogSink{"logs/boot.log"};
    bootLoggerConfig.stdoutLogger = dory::core::resources::configuration::StdoutLogSink{};

    auto bootstrap = dory::game::Bootstrap{registry};
    bootstrap.initialize(staticLibraryHandle, context);

    auto game = dory::game::Game{registry};
    game.initialize(staticLibraryHandle, context);

    bootstrap.run(context);
    bootstrap.cleanup(context);

    return 0;
}
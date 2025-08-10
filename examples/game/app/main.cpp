#include "dory/game/setup.h"
#include <dory/core/services/logService.h>
#include <dory/core/resources/localizationImpl.h>
#include <dory/game/bootstrap.h>
#include "game.h"

#include <iostream>
#include <dory/profiling/profiler.h>

static auto systemMemPoolName = "System";
static bool traceOn = false;

void* operator new(const std::size_t size)
{
    void* ptr = std::malloc(size);
    if(traceOn)
    {
        DORY_TRACE_MEM_ALLOC(ptr, size, systemMemPoolName);
    }
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    if(traceOn)
    {
        DORY_TRACE_MEM_FREE(ptr, systemMemPoolName);
    }
    std::free(ptr);
}

void* operator new[](const std::size_t size)
{
    void* ptr = std::malloc(size);
    if(traceOn)
    {
        DORY_TRACE_MEM_ALLOC(ptr, size, systemMemPoolName);
    }
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

void operator delete[](void* ptr) noexcept
{
    if(traceOn)
    {
        DORY_TRACE_MEM_FREE(ptr, systemMemPoolName);
    }
    std::free(ptr);
}

#ifdef DORY_MAIN_FUNCTION_UNIX
int main()
#endif
#ifdef DORY_MAIN_FUNCTION_WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
#endif
{
    std::cout << "Begin Start Profiler" << std::endl;
    DORY_TRACE_START();
    traceOn = true;
    std::cout << "End Start Profiler" << std::endl;

    {
        dory::game::Setup setup;
        dory::generic::extension::LibraryHandle staticLibraryHandle {};
        auto registry = dory::core::Registry{};
        auto configuration = dory::core::resources::configuration::Configuration{};
        auto localization = dory::core::resources::LocalizationImpl{};
        auto context = dory::core::resources::DataContext{ configuration, localization };

        setup.setupRegistry(staticLibraryHandle, registry, configuration);

        //initial config
        configuration.section.loadFrom.emplace_back("settings.yaml");
        auto& bootLoggerConfig = configuration.loggingConfiguration.configurationLogger;
        bootLoggerConfig.name = "boot";
        bootLoggerConfig.rotationLogger = dory::core::resources::configuration::RotationLogSink{"logs/boot.log"};
        bootLoggerConfig.stdoutLogger = dory::core::resources::configuration::StdoutLogSink{};

        auto bootstrap = dory::game::Bootstrap{registry};
        bootstrap.initialize(staticLibraryHandle, context);

        dory::core::resources::scene::Scene* rootScene;

        if(auto sceneConfigurationService = registry.get<dory::core::services::ISceneConfigurationService>())
        {
            dory::core::resources::scene::configuration::SceneConfiguration sceneConfig;
            sceneConfigurationService->load("scenes/root.yaml", sceneConfig, context);

            if(auto sceneBuilder = registry.get<dory::core::services::ISceneBuilder>())
            {
                rootScene = sceneBuilder->build(sceneConfig, context);
            }
        }

        auto game = dory::game::Game{ registry };
        game.initialize(staticLibraryHandle, context);

        bootstrap.run(context);

        if(rootScene)
        {
            if(auto sceneBuilder = registry.get<dory::core::services::ISceneBuilder>())
            {
                sceneBuilder->destroy(*rootScene, context);
            }
        }

        bootstrap.cleanup(context);
    }

    std::cout << "Begin Shutdown Profiler" << std::endl;
    traceOn = false;
    DORY_TRACE_SHUTDOWN();
    std::cout << "End Shutdown Profiler" << std::endl;

    return 0;
}
#include "dory/game/setup.h"
#include <dory/core/services/logService.h>
#include <dory/core/resources/localizationImpl.h>
#include <dory/game/bootstrap.h>
#include "game.h"

#include <iostream>
#include <dory/profiling/profiler.h>

//static auto systemMemPoolName = "System";

//#include <dory/game/custom_new_delete.h>
//#include <dory/game/custom_alloc_free.h>

/*void* operator new(const std::size_t size)
{
    void* ptr = std::malloc(size);
    if(DORY_TRACE_IS_PROFILER_READY)
    {
        std::cout << "mem-alloc: " << ptr << std::endl;
        DORY_TRACE_MEM_ALLOC(ptr, size, systemMemPoolName);
    }
    if (!ptr)
    {
        throw std::bad_alloc();
    }
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    if(DORY_TRACE_IS_PROFILER_READY)
    {
        std::cout << "mem-free: " << ptr << std::endl;
        DORY_TRACE_MEM_FREE(ptr, systemMemPoolName);
    }
    std::free(ptr);
}

void* operator new[](const std::size_t size)
{
    void* ptr = std::malloc(size);
    if(DORY_TRACE_IS_PROFILER_READY)
    {
        DORY_TRACE_MEM_ALLOC(ptr, size, systemMemPoolName);
    }
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

void operator delete[](void* ptr) noexcept
{
    if(DORY_TRACE_IS_PROFILER_READY)
    {
        DORY_TRACE_MEM_FREE(ptr, systemMemPoolName);
    }
    std::free(ptr);
}*/

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
    DORY_TRACE_SET_PROFILER_READY();
    std::cout << "End Start Profiler" << std::endl;

    {
        DORY_TRACE_ZONE_NAMED(zoneBuildCoreObject, "game::main - Build Core Objects");
        dory::game::Setup setup;
        dory::generic::extension::LibraryHandle staticLibraryHandle {};
        auto registry = dory::core::Registry{};
        auto configuration = dory::core::resources::configuration::Configuration{};
        auto localization = dory::core::resources::LocalizationImpl{};
        auto context = dory::core::resources::DataContext{ configuration, localization };
        DORY_TRACE_ZONE_END(zoneBuildCoreObject);

        DORY_TRACE_ZONE_NAMED(zoneSetupRegistry, "game::main - Setup Registry");
        setup.setupRegistry(staticLibraryHandle, registry, configuration);
        DORY_TRACE_ZONE_END(zoneSetupRegistry);

        //initial config
        DORY_TRACE_ZONE_NAMED(zoneConfigureBootLogger, "game::main - Configure Boot Logger");
        configuration.section.loadFrom.emplace_back("settings.yaml");
        auto& bootLoggerConfig = configuration.loggingConfiguration.configurationLogger;
        bootLoggerConfig.name = "boot";
        bootLoggerConfig.rotationLogger = dory::core::resources::configuration::RotationLogSink{"logs/boot.log"};
        bootLoggerConfig.stdoutLogger = dory::core::resources::configuration::StdoutLogSink{};
        DORY_TRACE_ZONE_END(zoneConfigureBootLogger);

        DORY_TRACE_ZONE_NAMED(zoneBootstrapInitialize, "game::main - Bootstrap Initialize");
        auto bootstrap = dory::game::Bootstrap{registry};
        bootstrap.initialize(staticLibraryHandle, context);
        DORY_TRACE_ZONE_END(zoneBootstrapInitialize);

        dory::core::resources::scene::Scene* rootScene;

        DORY_TRACE_ZONE_NAMED(zoneBuildScene, "game::main - Build Scene");
        if(auto sceneConfigurationService = registry.get<dory::core::services::ISceneConfigurationService>())
        {
            dory::core::resources::scene::configuration::SceneConfiguration sceneConfig;
            sceneConfigurationService->load("scenes/root.yaml", sceneConfig, context);

            if(auto sceneBuilder = registry.get<dory::core::services::ISceneBuilder>())
            {
                rootScene = sceneBuilder->build(sceneConfig, context);
            }
        }
        DORY_TRACE_ZONE_END(zoneBuildScene);

        DORY_TRACE_ZONE_NAMED(zoneGameInitialize, "game::main - Game Initialize");
        auto game = dory::game::Game{ registry };
        game.initialize(staticLibraryHandle, context);
        DORY_TRACE_ZONE_END(zoneGameInitialize);

        DORY_TRACE_ZONE_NAMED(zoneBootstrapRun, "game::main - Bootstrap Run");
        bootstrap.run(context);
        DORY_TRACE_ZONE_END(zoneBootstrapRun);

        DORY_TRACE_ZONE_NAMED(zoneDestroyScene, "game::main - Destroy Scene");
        if(rootScene)
        {
            if(auto sceneBuilder = registry.get<dory::core::services::ISceneBuilder>())
            {
                sceneBuilder->destroy(*rootScene, context);
            }
        }
        DORY_TRACE_ZONE_END(zoneDestroyScene);

        DORY_TRACE_ZONE_NAMED(zoneBootstrapCleanup, "game::main - Bootstrap Cleanup");
        bootstrap.cleanup(context);
        DORY_TRACE_ZONE_END(zoneBootstrapCleanup);
    }

    std::cout << "Begin Shutdown Profiler" << std::endl;
    DORY_TRACE_SHUTDOWN();
    DORY_TRACE_SET_PROFILER_NOT_READY();
    std::cout << "End Shutdown Profiler" << std::endl;

    return 0;
}
set(DORY_MODULE_NAME ${PROJECT_NAME}-core)

if(LINUX)
    set(DORY_PLATFORM_MODULES ${DORY_PLATFORM_MODULES} src/devices/standardIoDeviceUnix.cpp)
endif()

if(WIN32)
    set(DORY_PLATFORM_MODULES ${DORY_PLATFORM_MODULES} src/devices/standardIoDeviceWin32.cpp)
endif()

add_library(${DORY_MODULE_NAME} STATIC
        src/extension/dynamicLibrary.cpp
        src/resources/imageStreamQueue.cpp
        src/devices/terminalDevice.cpp
        src/devices/glfwWindowSystemDevice.cpp
        src/devices/fileWatcherDevice.cpp
        src/devices/openglGpuDevice.cpp
        src/devices/imageStreamDevice.cpp
        src/repositories/pipelineRepository.cpp
        src/repositories/viewRepository.cpp
        src/repositories/shaderProgramRepository.cpp
        src/repositories/enttSceneRepository.cpp
        src/repositories/imageStreamRepository.cpp
        src/services/fileService.cpp
        src/services/libraryService.cpp
        src/services/logService.cpp
        src/services/dataFormatResolver.cpp
        src/services/scriptService.cpp
        src/services/localizationService.cpp
        src/services/pipelineService.cpp
        src/services/loopService.cpp
        src/services/windowService.cpp
        src/services/assetTypeResolver.cpp
        src/services/hot-reload/extensionLoader.cpp
        src/services/enttSceneService.cpp
        src/services/enttSceneQueryService.cpp
        src/services/graphics/meshAssetBinder.cpp
        src/services/graphics/materialAssetBinder.cpp
        src/services/graphics/shaderAssetBinder.cpp
        src/services/viewService.cpp
        src/services/graphics/renderer.cpp
        src/services/generators/meshGenerator.cpp
        src/services/imageStreamService.cpp
        src/services/files/imageFileService.cpp
        src/services/profilingService.cpp
        src/services/sceneConfigurationService.cpp
        src/services/sceneBuilder.cpp
        src/services/scene/directors/assetLoader.cpp
        src/services/scene/directors/applicationDirector.cpp
        src/services/scene/directors/viewDirector.cpp
        src/services/layoutService.cpp
        src/services/layoutSetupService.cpp
        src/services/displayService.cpp
        src/controllers/windowSystemController.cpp
        src/controllers/viewController.cpp
        src/controllers/movementController.cpp
        src/controllers/accelerationMovementController.cpp
        src/controllers/transformController.cpp
        src/controllers/frameCounter.cpp
        src/controllers/profiler.cpp
        src/controllers/eventDispatcher.cpp
        src/controllers/stateUpdater.cpp
        src/controllers/consoleFlusher.cpp
        src/triggers/timeFrameTrigger.cpp
        ${DORY_PLATFORM_MODULES})

target_include_directories(${DORY_MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${DORY_MODULE_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/external/stb)

target_link_libraries(${DORY_MODULE_NAME} PUBLIC
        dory-core-generic
        dory-serialization
        Boost::dll
        spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>
        opengl-glad
        glfw
        efsw
        EnTT)

set_target_properties(${DORY_MODULE_NAME} PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME "dory-core")

if(LINUX)
    target_compile_options(${DORY_MODULE_NAME} PRIVATE -fPIC)
endif()
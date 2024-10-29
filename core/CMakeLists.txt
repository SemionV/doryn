set(DORY_MODULE_NAME ${PROJECT_NAME}-core)

if(LINUX)
    set(DORY_PLATFORM_MODULES ${DORY_PLATFORM_MODULES} src/devices/standardIoDeviceUnix.cpp)
endif()

if(WIN32)
    set(DORY_PLATFORM_MODULES ${DORY_PLATFORM_MODULES} src/devices/standardIoDeviceWin32.cpp)
endif()

add_library(${DORY_MODULE_NAME} STATIC
        src/devices/terminalDevice.cpp
        src/extension/dynamicLibrary.cpp
        src/repositories/pipelineRepository.cpp
        src/services/fileService.cpp
        src/services/libraryService.cpp
        src/services/logService.cpp
        src/services/dataFormatResolver.cpp
        src/services/scriptService.cpp
        src/services/localizationService.cpp
        src/services/pipelineService.cpp
        ${DORY_PLATFORM_MODULES})

target_include_directories(${DORY_MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${DORY_MODULE_NAME} PUBLIC
        dory-core-generic
        dory-serialization
        core::boost
        spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)

set_target_properties(${DORY_MODULE_NAME} PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME "dory-core")

if(LINUX)
    target_compile_options(${DORY_MODULE_NAME} PRIVATE -fPIC)
endif()

add_subdirectory(external)
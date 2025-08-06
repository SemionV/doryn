set(DORY_MODULE_NAME ${PROJECT_NAME}-profiling)
set(DORY_MODULE_INTERFACE_NAME ${DORY_MODULE_NAME}-interface)

add_definitions(-DDORY_PROFILING_ON)

set(TRACY_ENABLE ON)
if(WIN32)
    add_definitions(-DTRACY_IMPORTS)
endif()

if(TRACY_ENABLE)
    get_target_property(TRACY_INCLUDE_DIR Tracy::TracyClient INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(TRACY_INCLUDE_DIR Tracy::TracyClient INTERFACE_INCLUDE_DIRECTORIES)
    # Extract the first path (BUILD_INTERFACE)
    list(GET TRACY_INCLUDE_DIR 0 TRACY_PUBLIC_DIR)
    string(REPLACE "$<BUILD_INTERFACE:" "" TRACY_PUBLIC_DIR "${TRACY_PUBLIC_DIR}")
    string(REPLACE ">" "" TRACY_PUBLIC_DIR "${TRACY_PUBLIC_DIR}")
    message(STATUS "Resolved Tracy public dir: ${TRACY_PUBLIC_DIR}")
endif()

set(TRACY_SRC ${TRACY_PUBLIC_DIR}/TracyClient.cpp)

add_library(${DORY_MODULE_NAME} SHARED
        src/metricsReader.cpp
        src/profiler.cpp
        ${TRACY_SRC})
target_include_directories(${DORY_MODULE_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(${DORY_MODULE_NAME} PRIVATE DORY_DLL_EXPORTS
        TRACY_ENABLE
        TRACY_DELAYED_INIT
        TRACY_MANUAL_LIFETIME)
if(TRACY_ENABLE)
    message("Tracy include dir: ${TRACY_INCLUDE_DIR}")
    target_include_directories(${DORY_MODULE_NAME} PUBLIC ${TRACY_INCLUDE_DIR})
endif()

add_library(${DORY_MODULE_INTERFACE_NAME} INTERFACE include/dory/profiling/profiler.h)
target_include_directories(${DORY_MODULE_INTERFACE_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(TRACY_ENABLE)
    get_target_property(TRACY_INCLUDE_DIR Tracy::TracyClient INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(TRACY_PUBLIC_DIR Tracy::TracyClient INTERFACE_INCLUDE_DIRECTORIES)
    message("Tracy include dir: ${TRACY_INCLUDE_DIR}")
    target_include_directories(${DORY_MODULE_NAME} PUBLIC ${TRACY_INCLUDE_DIR})
endif()

target_link_libraries(${DORY_MODULE_NAME} PRIVATE
        dory-macros
        $<$<BOOL:${TRACY_ENABLE}>:Tracy::TracyClient>)

set_target_properties(${DORY_MODULE_NAME} PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME ${DORY_MODULE_NAME})
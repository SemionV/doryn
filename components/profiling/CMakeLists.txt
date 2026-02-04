set(DORY_MODULE_NAME ${PROJECT_NAME}-profiling)
set(DORY_MODULE_INTERFACE_NAME ${DORY_MODULE_NAME}-interface)

add_definitions(-DDORY_PROFILING_ON)
add_definitions(-DDORY_GPU_PROFILING_ON)

set(TRACY_ENABLE ON)
if(WIN32)
    add_definitions(-DTRACY_EXPORTS)
endif()

if(DORY_PROFILING_MEMORY_JOURNAL)
    add_definitions(-DDORY_PROFILING_MEMORY_JOURNAL)
endif()

if(TRACY_ENABLE)
    get_target_property(TRACY_INCLUDE_DIR Tracy::TracyClient INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(TRACY_PUBLIC_DIR Tracy::TracyClient INTERFACE_INCLUDE_DIRECTORIES)

    message("Tracy include dir: ${TRACY_INCLUDE_DIR}")
    message("Tracy public dir: ${TRACY_PUBLIC_DIR}")

    # Extract the first path (BUILD_INTERFACE)
    list(GET TRACY_INCLUDE_DIR 0 TRACY_PUBLIC_DIR)
    string(REPLACE "$<BUILD_INTERFACE:" "" TRACY_PUBLIC_DIR "${TRACY_PUBLIC_DIR}")
    string(REPLACE ">" "" TRACY_PUBLIC_DIR "${TRACY_PUBLIC_DIR}")
    message(STATUS "Resolved Tracy public dir: ${TRACY_PUBLIC_DIR}")

    target_compile_definitions(TracyClient PRIVATE
            TRACY_MANUAL_LIFETIME
            TRACY_DELAYED_INIT
    )
    if(WIN32)
        target_compile_definitions(TracyClient PRIVATE TRACY_NO_RPMALLOC TRACY_EXPORTS)
    endif()
endif()

#set(TRACY_SRC ${TRACY_PUBLIC_DIR}/TracyClient.cpp)

add_library(${DORY_MODULE_NAME} SHARED
        src/dllmain.cpp
        include/dory/profiling/overrideNewDelete.h
        src/allocationTrack.cpp
        src/metricsReader.cpp
        src/profiler.cpp)
        #src/customNewDelete.cpp
        #${TRACY_SRC})
target_include_directories(${DORY_MODULE_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(${DORY_MODULE_NAME} PRIVATE
        DORY_DLL_EXPORTS
        DORY_ALLOC_BUILD
        TRACY_ENABLE
        TRACY_DELAYED_INIT
        TRACY_MANUAL_LIFETIME)

install(TARGETS ${DORY_MODULE_NAME}
        DESTINATION ${DORY_INSTALL_PREFIX}
        ARCHIVE DESTINATION ${DORY_INSTALL_PREFIX}/dev
        FILE_SET modules DESTINATION ${DORY_INSTALL_PREFIX}/dev/modules
        CXX_MODULES_BMI DESTINATION ${DORY_INSTALL_PREFIX}/dev
        COMPONENT ${DORY_COMPONENT})

add_library(${DORY_MODULE_INTERFACE_NAME} INTERFACE include/dory/profiling/profiler.h)
target_include_directories(${DORY_MODULE_INTERFACE_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(TRACY_ENABLE)
    target_include_directories(${DORY_MODULE_NAME} PUBLIC ${TRACY_INCLUDE_DIR})
endif()

target_link_libraries(${DORY_MODULE_NAME} PRIVATE  $<$<BOOL:${TRACY_ENABLE}>:Tracy::TracyClient>)

target_link_libraries(${DORY_MODULE_NAME} PUBLIC
        dory-macros
        opengl-glad
        glfw)

target_link_libraries(${DORY_MODULE_INTERFACE_NAME} INTERFACE
        dory-macros)

set_target_properties(${DORY_MODULE_NAME} PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME ${DORY_MODULE_NAME})

#A tiny INTERFACE library to force all TUs in other modules to include custom new/delete declarations
set(DORY_ALLOC_DECL_MODULE_NAME ${PROJECT_NAME}-alloc-forceinclude)

add_library(${DORY_ALLOC_DECL_MODULE_NAME} INTERFACE
        ../graphics/opengl/include/dory/graphics/opengl/compatibilityLayer.h
        ../graphics/opengl/src/compatibilityLayer.cpp)
target_include_directories(${DORY_ALLOC_DECL_MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(DORY_ALLOC_FORCE_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/dory/profiling/overrideNewDelete.h")

target_compile_options(${DORY_ALLOC_DECL_MODULE_NAME} INTERFACE
        $<$<COMPILE_LANGUAGE:CXX>:
        $<$<CXX_COMPILER_ID:MSVC>:/FI${DORY_ALLOC_FORCE_HEADER}>
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-include ${DORY_ALLOC_FORCE_HEADER}>
        >)

#A static lib, which is implementing custom new/delete overrides
set(DORY_CUSTOM_NEW_DELETE_MODULE_NAME ${PROJECT_NAME}-custom-new-delete)

add_library(${DORY_CUSTOM_NEW_DELETE_MODULE_NAME} STATIC
    src/overrideNewDelete.cpp)

target_link_libraries(${DORY_CUSTOM_NEW_DELETE_MODULE_NAME} PRIVATE ${DORY_MODULE_NAME})

target_include_directories(${DORY_CUSTOM_NEW_DELETE_MODULE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
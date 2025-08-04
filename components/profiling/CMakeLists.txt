set(DORY_MODULE_NAME ${PROJECT_NAME}-profiling)

add_library(${DORY_MODULE_NAME} SHARED
        src/metricsReader.cpp
        src/profiler.cpp)
target_include_directories(${DORY_MODULE_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(${DORY_MODULE_NAME} PRIVATE -DDORY_DLL_EXPORTS)

if(TRACY_ENABLE)
    get_target_property(TRACY_INCLUDE_DIR Tracy::TracyClient INTERFACE_INCLUDE_DIRECTORIES)
    message("Tracy include dir: ${TRACY_INCLUDE_DIR}")
    target_include_directories(${DORY_MODULE_NAME} PUBLIC ${TRACY_INCLUDE_DIR})
endif()

target_link_libraries(${DORY_MODULE_NAME} PRIVATE
        dory-macros
        $<$<BOOL:${TRACY_ENABLE}>:Tracy::TracyClient>)

set_target_properties(${DORY_MODULE_NAME} PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME ${DORY_MODULE_NAME})
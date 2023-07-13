cmake_minimum_required(VERSION 3.22)

project(dory-windowsImplementations)

add_library(windowsImplementations SHARED windowSystem.cpp
                                        window.cpp)

set_target_properties(windowsImplementations PROPERTIES PREFIX ""
                                            IMPORT_PREFIX ""
                                            IMPORT_SUFFIX ".lib"
                                            OUTPUT_NAME "windowsImplementations")

target_compile_definitions(windowsImplementations PRIVATE DORY_EXPORT)
target_include_directories(windowsImplementations PRIVATE "${CMAKE_SOURCE_DIR}")
target_precompile_headers(windowsImplementations PRIVATE dependencies.h)

target_link_libraries(windowsImplementations base)

install(TARGETS windowsImplementations
        RUNTIME DESTINATION ${INSTALL_LIBRARIES_DESTINATION} COMPONENT Engine
        LIBRARY DESTINATION ${INSTALL_LIBRARIES_DESTINATION} COMPONENT Engine
        ARCHIVE DESTINATION lib COMPONENT EngineDevelopment)
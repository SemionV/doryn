cmake_minimum_required(VERSION 3.22)

project(dory-systemWindows)

add_library(systemWindows SHARED systemThread.cpp)

set_target_properties(systemWindows PROPERTIES PREFIX ""
                                            IMPORT_PREFIX ""
                                            IMPORT_SUFFIX ".lib"
                                            OUTPUT_NAME "systemWindows")

target_compile_definitions(systemWindows PRIVATE DORY_EXPORT)
target_include_directories(systemWindows PRIVATE "${CMAKE_SOURCE_DIR}")
target_precompile_headers(systemWindows PRIVATE dependencies.h)

target_link_libraries(systemWindows base)

install(TARGETS systemWindows
        RUNTIME DESTINATION ${INSTALL_LIBRARIES_DESTINATION} COMPONENT Engine
        LIBRARY DESTINATION ${INSTALL_LIBRARIES_DESTINATION} COMPONENT Engine
        ARCHIVE DESTINATION lib COMPONENT EngineDevelopment)
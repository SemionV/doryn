cmake_minimum_required(VERSION 3.22)

project(dory-win32)

add_library(win32 SHARED windowFactory.cpp)

set_target_properties(win32 PROPERTIES PREFIX ""
                                            IMPORT_PREFIX ""
                                            IMPORT_SUFFIX ".lib"
                                            OUTPUT_NAME "win32")

target_compile_definitions(win32 PRIVATE DORY_EXPORT)
target_include_directories(win32 PRIVATE "${CMAKE_SOURCE_DIR}")
target_precompile_headers(win32 PRIVATE dependencies.h)

target_link_libraries(win32 base)

install(TARGETS win32
        RUNTIME DESTINATION ${INSTALL_LIBRARIES_DESTINATION} COMPONENT Engine
        LIBRARY DESTINATION ${INSTALL_LIBRARIES_DESTINATION} COMPONENT Engine
        ARCHIVE DESTINATION lib COMPONENT EngineDevelopment)
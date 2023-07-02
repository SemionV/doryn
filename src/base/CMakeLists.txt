cmake_minimum_required(VERSION 3.26)

project(dory-base)

add_library(base SHARED basicFrameService.cpp
                        controller.cpp
                        engine.cpp
                        frameService.cpp
                        types.cpp)

set_target_properties(base PROPERTIES PREFIX ""
                                            IMPORT_PREFIX ""
                                            IMPORT_SUFFIX ".lib"
                                            OUTPUT_NAME "base")

target_compile_definitions(base PRIVATE DORY_EXPORT)
target_precompile_headers(base PRIVATE dependencies.h)

install(TARGETS base
        RUNTIME DESTINATION . COMPONENT Engine
        LIBRARY DESTINATION . COMPONENT Engine
        ARCHIVE DESTINATION lib COMPONENT EngineDevelopment)
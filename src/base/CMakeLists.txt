cmake_minimum_required(VERSION 3.26)

project(dory-base)

add_library(base SHARED basicFrameService.cpp
                        controller.cpp
                        engine.cpp
                        frameService.cpp
                        types.cpp)
target_compile_definitions(base PRIVATE DORY_EXPORT)
target_precompile_headers(base PRIVATE dependencies.h)

install(TARGETS base COMPONENT Engine)
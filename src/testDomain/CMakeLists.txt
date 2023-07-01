cmake_minimum_required(VERSION 3.26)

project(dory-testDomain)

add_library(testDomain SHARED stepFrameService.cpp
                                testController.cpp)
target_compile_definitions(testDomain PRIVATE DORY_EXPORT)
target_include_directories(testDomain PRIVATE "${CMAKE_SOURCE_DIR}")
target_precompile_headers(testDomain PRIVATE dependencies.h)

target_link_libraries(testDomain base)
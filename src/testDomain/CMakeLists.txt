cmake_minimum_required(VERSION 3.26)

project(dory-testDomain)

add_library(testDomain SHARED stepFrameService.cpp
                                testController.cpp)

set_target_properties(testDomain PROPERTIES PREFIX ""
                                            IMPORT_PREFIX ""
                                            IMPORT_SUFFIX ".lib"
                                            OUTPUT_NAME "testDomain")

target_compile_definitions(testDomain PRIVATE DORY_EXPORT)
target_include_directories(testDomain PRIVATE "${CMAKE_SOURCE_DIR}")
target_precompile_headers(testDomain PRIVATE dependencies.h)

target_link_libraries(testDomain base)

install(TARGETS testDomain
        RUNTIME DESTINATION . COMPONENT Example
        LIBRARY DESTINATION . COMPONENT Example
        ARCHIVE DESTINATION lib COMPONENT ExampleDevelopment)
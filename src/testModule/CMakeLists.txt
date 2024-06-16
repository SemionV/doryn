cmake_minimum_required(VERSION 3.22)

project(dory-testModule)

add_library(testModule MODULE ${PLATFORM}
        testModule.cpp
        factory.cpp)

set_target_properties(testModule PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME "testModule")

target_link_libraries(testModule base)

target_include_directories(testModule PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(testModule PRIVATE ${VENDOR_DIR})

install(TARGETS testModule DESTINATION ./modules COMPONENT Example)
cmake_minimum_required(VERSION 3.26)

set(TEST_APP_DIR "testApp/")
project(testApp)
add_executable(testApp "${TEST_APP_DIR}main.cpp")
target_include_directories(testApp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_precompile_headers(testApp PRIVATE "${TEST_APP_DIR}dependencies.ext.h")

set(BASE_LIB_DIR "base/")
add_library(base SHARED "${BASE_LIB_DIR}basicFrameService.cpp"
                        "${BASE_LIB_DIR}controller.cpp"
                        "${BASE_LIB_DIR}engine.cpp"
                        "${BASE_LIB_DIR}frameService.cpp"
                        "${BASE_LIB_DIR}types.cpp")
target_compile_definitions(base PRIVATE DORY_EXPORT)
target_precompile_headers(base PRIVATE "${BASE_LIB_DIR}dependencies.ext.h")

set(TEST_DOMAIN_LIB_DIR "testDomain/")
add_library(testDomain SHARED "${TEST_DOMAIN_LIB_DIR}stepFrameService.cpp"
                                "${TEST_DOMAIN_LIB_DIR}testController.cpp")
target_compile_definitions(testDomain PRIVATE DORY_EXPORT)
target_include_directories(testDomain PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_precompile_headers(testDomain PRIVATE "${TEST_DOMAIN_LIB_DIR}dependencies.ext.h")

target_link_libraries(testDomain base)
target_link_libraries(testApp base testDomain)
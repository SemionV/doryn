cmake_minimum_required(VERSION 3.22)
project(dory-tests)

add_executable(tests modules.cpp)
                         
target_include_directories(tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(tests PRIVATE ${VENDOR_DIR})

target_link_libraries(tests PRIVATE base gmock gtest_main)

install(TARGETS tests DESTINATION . COMPONENT Tests)
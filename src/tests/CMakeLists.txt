cmake_minimum_required(VERSION 3.22)
project(dory-tests)

add_executable(tests mocking.cpp)
                         
target_include_directories(baseTests PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(baseTests PRIVATE ${VENDOR_DIR})

target_link_libraries(tests PRIVATE gmock gtest_main)

install(TARGETS tests DESTINATION . COMPONENT Tests)
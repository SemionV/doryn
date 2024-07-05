cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(dory-tests)

add_executable(tests
        modules.cpp
        dummy.cpp)

target_link_libraries(tests PRIVATE engine gmock gtest_main engine)

install(TARGETS tests DESTINATION . COMPONENT Tests)
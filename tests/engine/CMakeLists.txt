cmake_minimum_required(VERSION ${DORY_BUILD_TOOLS_REQUIRED_VERSION})
project(dory-tests)

add_executable(tests
        modules.cpp)

target_link_libraries(tests PRIVATE
        engine_include
        Boost::dll
        gmock
        gtest_main)

install(TARGETS tests DESTINATION . COMPONENT Tests)
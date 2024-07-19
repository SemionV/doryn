cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(dory-tests)

add_executable(tests
        modules.cpp)

target_link_libraries(tests PRIVATE
        inc_engine
        Boost::dll
        gmock
        gtest_main)

install(TARGETS tests DESTINATION . COMPONENT Tests)
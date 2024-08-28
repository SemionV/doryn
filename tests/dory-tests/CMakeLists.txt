cmake_minimum_required(VERSION ${DORY_BUILD_TOOLS_REQUIRED_VERSION})
project(dory-tests)

add_executable(${PROJECT_NAME}
        modules.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
        engine_include
        Boost::dll
        gmock
        gtest_main)

install(TARGETS ${PROJECT_NAME} DESTINATION ${TESTS_INSTALL_PREFIX} COMPONENT Tests)
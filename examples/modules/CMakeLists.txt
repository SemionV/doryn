cmake_minimum_required(VERSION ${DORY_BUILD_TOOLS_REQUIRED_VERSION})
project(dory-modules-example)

add_subdirectory(client bin)
add_subdirectory(testModule bin/modules)
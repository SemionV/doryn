set(DORY_BUILD_TOOLS_REQUIRED_VERSION 3.18)
cmake_minimum_required(VERSION ${DORY_BUILD_TOOLS_REQUIRED_VERSION})
project(dory VERSION 0.0.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

enable_testing()

include(NoInSourceBuilds)
include(LanguageOptions)

add_subdirectory(external EXCLUDE_FROM_ALL)
add_subdirectory(components)
add_subdirectory(tests)
add_subdirectory(examples)
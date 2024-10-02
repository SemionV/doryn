cmake_minimum_required(VERSION 3.30)
project(dory VERSION 0.0.1 LANGUAGES CXX)

set(DORY_CMAKE_SCRIPTS_PATH "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH ${DORY_CMAKE_SCRIPTS_PATH})

option(DORY_TEST_COVERAGE "Enable test coverage report" OFF)
enable_testing()

include(NoInSourceBuilds)
include(LanguageOptions)

add_subdirectory(external EXCLUDE_FROM_ALL)
add_subdirectory(core)
add_subdirectory(components)
add_subdirectory(tests)
add_subdirectory(examples)
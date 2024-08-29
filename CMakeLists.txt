set(DORY_BUILD_TOOLS_REQUIRED_VERSION 3.18)
cmake_minimum_required(VERSION ${DORY_BUILD_TOOLS_REQUIRED_VERSION})
project(dory VERSION 0.0.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(NoInSourceBuilds)
include(LanguageOptions)

add_subdirectory(external EXCLUDE_FROM_ALL)
add_subdirectory(components/engine)
add_subdirectory(components/bootstrap)

set(TESTS_INSTALL_PREFIX "tests/")
add_subdirectory(tests/dory-tests)
add_subdirectory(tests/dory-tests-catch)
add_subdirectory(tests/dory-benchmarks)

set(EXAMPLES_INSTALL_PREFIX "examples/")
add_subdirectory(examples/sandbox)
add_subdirectory(examples/modules)
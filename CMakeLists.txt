cmake_minimum_required(VERSION 3.30)
project(dory VERSION 0.0.1 LANGUAGES CXX)

set(DORY_CMAKE_SCRIPTS_PATH "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH ${DORY_CMAKE_SCRIPTS_PATH})

# Put installs into your repo's bin/ by default (works great for dev)
set(DORY_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/bin" CACHE PATH "Dory install output directory")

option(DORY_TEST_COVERAGE "Enable test coverage report" OFF)
option(DORY_PROFILING "Enable profiling" OFF)
option(DORY_GPU_PROFILING "Enable GPU profiling - extend general profiling" OFF)

enable_testing() #TODO: Make testing optional?

include(NoInSourceBuilds)
include(LanguageOptions)

add_subdirectory(external EXCLUDE_FROM_ALL)
add_subdirectory(components)
add_subdirectory(core-interfaces)
add_subdirectory(core-generic)
add_subdirectory(core)
add_subdirectory(tests)
add_subdirectory(examples)
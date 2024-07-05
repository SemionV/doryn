cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(dory-external)

find_package(opengl-glad QUIET)
if(NOT opengl-glad_FOUND)
    message("opengl-glad is not found, initializing git submodule")
    execute_process(
            COMMAND git submodule update --init -- opengl-glad
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    add_subdirectory(opengl-glad)
endif()

include(FetchContent)

FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0)

set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
FetchContent_Declare(google-benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.8.4)
FetchContent_MakeAvailable(google-benchmark)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(nlohmann-json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3)
FetchContent_MakeAvailable(nlohmann-json)

FetchContent_Declare(rapid-yaml
        GIT_REPOSITORY https://github.com/biojppm/rapidyaml
        GIT_TAG v0.7.0)
FetchContent_MakeAvailable(rapid-yaml)

message(NOTICE "fetching boost...")
set(BOOST_INCLUDE_LIBRARIES dll)
set(BOOST_ENABLE_CMAKE ON)
FetchContent_Declare(boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG boost-1.85.0)
FetchContent_MakeAvailable(boost)

FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.14.1)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(reflcpp
        GIT_REPOSITORY https://github.com/veselink1/refl-cpp.git
        GIT_TAG v0.12.4)
FetchContent_MakeAvailable(reflcpp)

FetchContent_Declare(catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.6.0)
FetchContent_MakeAvailable(catch2)

FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4)
FetchContent_MakeAvailable(glfw)
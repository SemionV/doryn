find_package(opengl-glad QUIET)
if(NOT opengl-glad_FOUND)
    message(STATUS "opengl-glad is not found, initializing git submodule")
    execute_process(
            COMMAND git submodule update --remote -- opengl-glad
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    add_subdirectory(opengl-glad)
endif()

message(STATUS "initializing git submodule: dory-boost-adapter")
execute_process(
        COMMAND git submodule update --remote -- dory-boost-adapter
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_subdirectory(dory-boost-adapter)

include(FetchContent)

FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_Declare(google-benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.8.4)
set(BENCHMARK_ENABLE_INSTALL OFF)
FetchContent_MakeAvailable(google-benchmark)

FetchContent_Declare(catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.6.0)
FetchContent_MakeAvailable(catch2)

FetchContent_Declare(nlohmann-json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3)
FetchContent_MakeAvailable(nlohmann-json)

FetchContent_Declare(rapid-yaml
        GIT_REPOSITORY https://github.com/biojppm/rapidyaml)
FetchContent_MakeAvailable(rapid-yaml)

FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.14.1)
set(SAVED_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
FetchContent_MakeAvailable(spdlog)
set(CMAKE_CXX_FLAGS "${SAVED_CMAKE_CXX_FLAGS}")

FetchContent_Declare(reflcpp
        GIT_REPOSITORY https://github.com/veselink1/refl-cpp.git
        GIT_TAG v0.12.4)
FetchContent_MakeAvailable(reflcpp)

FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4)
FetchContent_MakeAvailable(glfw)
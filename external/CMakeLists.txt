cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(dory-external)

include(FetchContent)

FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0)

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

set(BOOST_INCLUDE_LIBRARIES dll)
set(BOOST_ENABLE_CMAKE ON)
FetchContent_Declare(boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG boost-1.85.0)
FetchContent_MakeAvailable(boost)
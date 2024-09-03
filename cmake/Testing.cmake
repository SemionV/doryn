enable_testing()

include(FetchContent)

FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
option(INSTALL_GMOCK "Install GMock" OFF)
option(INSTALL_GTEST "Install GTest" OFF)
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

include(GoogleTest)
include(Catch)

macro(AddGoogleTests target)
    target_link_libraries(${target} PRIVATE gtest_main gmock)
    gtest_discover_tests(${target})
endmacro()

macro(AddCatchTests target)
    target_link_libraries(${target} PRIVATE Catch2::Catch2WithMain)
    catch_discover_tests(${target})
endmacro()

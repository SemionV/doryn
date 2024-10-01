project(benchmarks)

set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_Declare(google-benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.8.4)
set(BENCHMARK_ENABLE_INSTALL OFF)
FetchContent_MakeAvailable(google-benchmark)

add_executable(${PROJECT_NAME} concurrency.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE dory-engine dory-concurrency benchmark::benchmark)

install(TARGETS ${PROJECT_NAME} DESTINATION ${TESTS_INSTALL_PREFIX} COMPONENT Tests)
cmake_minimum_required(VERSION ${DORY_BUILD_TOOLS_REQUIRED_VERSION})

project(dory-benchmarks)

add_executable(baseBenchmarks concurrency.cpp)

target_link_libraries(baseBenchmarks PRIVATE engine_include benchmark::benchmark)

install(TARGETS baseBenchmarks DESTINATION . COMPONENT Tests)
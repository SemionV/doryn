cmake_minimum_required(VERSION ${DORY_BUILD_TOOLS_REQUIRED_VERSION})

project(dory-benchmarks)

add_executable(baseBenchmarks concurrency.cpp)

target_link_libraries(baseBenchmarks PRIVATE inc_engine benchmark::benchmark)
#target_precompile_headers(baseBenchmarks PRIVATE dependencies.h)

install(TARGETS baseBenchmarks DESTINATION . COMPONENT Tests)
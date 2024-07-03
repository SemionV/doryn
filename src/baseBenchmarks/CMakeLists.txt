cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})

project(dory-benchmarks)

add_executable(baseBenchmarks concurrency.cpp)

target_include_directories(baseBenchmarks PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(baseBenchmarks PRIVATE ${VENDOR_DIR})

target_link_libraries(baseBenchmarks PRIVATE benchmark::benchmark base)

target_precompile_headers(baseBenchmarks PRIVATE dependencies.h)

install(TARGETS baseBenchmarks DESTINATION . COMPONENT Tests)
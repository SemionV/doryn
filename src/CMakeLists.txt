cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(dory-src)

add_subdirectory(baseTests)
add_subdirectory(baseBenchmarks)
add_subdirectory(testApp)
add_subdirectory(testModule)
add_subdirectory(client)
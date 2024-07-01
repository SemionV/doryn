cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(dory-src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(base)
add_subdirectory(openGL)
add_subdirectory(application)
add_subdirectory(baseTests)
add_subdirectory(baseBenchmarks)
add_subdirectory(testApp)
add_subdirectory(testModule)
add_subdirectory(client)
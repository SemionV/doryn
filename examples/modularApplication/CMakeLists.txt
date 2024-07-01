cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(dory-examples-modular-application)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(mainModule)
add_subdirectory(app)
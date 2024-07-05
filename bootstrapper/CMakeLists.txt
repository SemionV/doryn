cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(dory-bootstrapper)

add_library(${PROJECT_NAME}_mod INTERFACE include/dory/bootstrapper.h)
target_include_directories(${PROJECT_NAME}_mod INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
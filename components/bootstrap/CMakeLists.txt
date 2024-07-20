cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(dory-bootstrap)

add_library(${PROJECT_NAME}_mod INTERFACE include/dory/bootstrap.h)
target_include_directories(${PROJECT_NAME}_mod INTERFACE include)
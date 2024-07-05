cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})

project(dory-openGL)

add_library(dory-openGL INTERFACE include/dory/opengl.h)

target_include_directories(dory-openGL INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(dory-openGL INTERFACE opengl-glad glfw)
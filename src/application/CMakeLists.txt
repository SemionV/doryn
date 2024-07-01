cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(application)

add_library(application INTERFACE include/dory/application.h
        registry.h)

target_include_directories(application INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(application INTERFACE base dory-openGL)
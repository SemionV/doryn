cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(main module)

add_library(modularApp::mainModule STATIC ${PLATFORM})

target_link_libraries(modularApp::mainModule PRIVATE application)
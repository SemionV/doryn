cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(mainModule)

add_library(mainModule INTERFACE ${PLATFORM}
        include/dory/examples/modularApp.h)
target_include_directories(base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

#target_link_libraries(mainModule PRIVATE application)
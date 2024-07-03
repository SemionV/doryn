cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})

project(dory-openGL)

add_library(dory-openGL ${CMAKE_CURRENT_SOURCE_DIR}/src/gl.cpp)

target_include_directories(dory-openGL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(dory-openGL PROPERTIES PREFIX ""
                                            IMPORT_PREFIX ""
                                            IMPORT_SUFFIX ".lib"
                                            OUTPUT_NAME "openGL")

target_precompile_headers(dory-openGL PRIVATE dependencies.h)

target_link_libraries(dory-openGL base glfw)
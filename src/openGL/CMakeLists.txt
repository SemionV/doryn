cmake_minimum_required(VERSION 3.22)

project(dory-openGL)

add_library(openGL SHARED viewControllerOpenGL.cpp)

set_target_properties(openGL PROPERTIES PREFIX ""
                                            IMPORT_PREFIX ""
                                            IMPORT_SUFFIX ".lib"
                                            OUTPUT_NAME "openGL")

target_compile_definitions(openGL PRIVATE DORY_EXPORT)
target_include_directories(openGL PRIVATE "${CMAKE_SOURCE_DIR}")
target_precompile_headers(openGL PRIVATE dependencies.h)

target_link_libraries(openGL base)

install(TARGETS openGL
        RUNTIME DESTINATION ${INSTALL_LIBRARIES_DESTINATION} COMPONENT Engine
        LIBRARY DESTINATION ${INSTALL_LIBRARIES_DESTINATION} COMPONENT Engine
        ARCHIVE DESTINATION lib COMPONENT EngineDevelopment)
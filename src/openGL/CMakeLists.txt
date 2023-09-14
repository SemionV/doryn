cmake_minimum_required(VERSION 3.22)

project(dory-openGL)

find_package(OpenGL REQUIRED)

find_package(glfw3 REQUIRED
            PATHS ../vendor/glfw NO_DEFAULT_PATH)

set_source_files_properties(${CMAKE_SOURCE_DIR}/vendor/GL/gl3w.c PROPERTIES LANGUAGE CXX )

add_library(dory-openGL SHARED ${CMAKE_SOURCE_DIR}/vendor/GL/gl3w.c
                                viewControllerOpenGL.cpp
                                glfwWindowSystem.cpp)

set_target_properties(dory-openGL PROPERTIES PREFIX ""
                                            IMPORT_PREFIX ""
                                            IMPORT_SUFFIX ".lib"
                                            OUTPUT_NAME "openGL")

target_compile_definitions(dory-openGL PRIVATE DORY_EXPORT)
target_include_directories(dory-openGL PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(dory-openGL PRIVATE "${CMAKE_SOURCE_DIR}/vendor")
target_precompile_headers(dory-openGL PRIVATE dependencies.h)

target_link_libraries(dory-openGL base glfw ${OPENGL_LIBRARIES})

install(TARGETS dory-openGL
        RUNTIME DESTINATION ${INSTALL_LIBRARIES_DESTINATION} COMPONENT Engine
        LIBRARY DESTINATION ${INSTALL_LIBRARIES_DESTINATION} COMPONENT Engine
        ARCHIVE DESTINATION lib COMPONENT EngineDevelopment)
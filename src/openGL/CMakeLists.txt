cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})

project(dory-openGL)

find_package(OpenGL REQUIRED)

find_package(glfw3 REQUIRED
            PATHS ${VENDOR_DIR}glfw NO_DEFAULT_PATH)

set_source_files_properties(${VENDOR_DIR}GL/gl3w.c PROPERTIES LANGUAGE CXX )

add_library(dory-openGL ${VENDOR_DIR}GL/gl3w.c)

target_include_directories(dory-openGL INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(dory-openGL PROPERTIES PREFIX ""
                                            IMPORT_PREFIX ""
                                            IMPORT_SUFFIX ".lib"
                                            OUTPUT_NAME "openGL")

target_compile_definitions(dory-openGL PRIVATE DORY_EXPORT)
target_include_directories(dory-openGL PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(dory-openGL PRIVATE ${VENDOR_DIR})
target_precompile_headers(dory-openGL PRIVATE dependencies.h)

target_link_libraries(dory-openGL glfw ${OPENGL_LIBRARIES})
target_link_libraries(dory-openGL base)

install(TARGETS dory-openGL
        RUNTIME DESTINATION ${INSTALL_LIBRARIES_DESTINATION} COMPONENT Engine
        LIBRARY DESTINATION ${INSTALL_LIBRARIES_DESTINATION} COMPONENT Engine
        ARCHIVE DESTINATION lib COMPONENT EngineDevelopment)
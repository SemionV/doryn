cmake_minimum_required(VERSION 3.22)

project(dory-testApp)

find_package(glfw3 REQUIRED PATHS ${VENDOR_DIR}glfw NO_DEFAULT_PATH)

set_source_files_properties(${VENDOR_DIR}GL/gl3w.c PROPERTIES LANGUAGE CXX )

add_executable(testApp ${PLATFORM} main.cpp ${VENDOR_DIR}GL/gl3w.c)
target_include_directories(testApp PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(testApp PRIVATE ${VENDOR_DIR})
target_precompile_headers(testApp PRIVATE dependencies.h)

target_link_libraries(testApp win32 base glfw ${OPENGL_LIBRARIES})

install(TARGETS testApp DESTINATION . COMPONENT Example)
install(DIRECTORY configuration DESTINATION . COMPONENT Example)
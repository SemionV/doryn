cmake_minimum_required(VERSION 3.22)

project(dory-testApp)

add_executable(testApp ${PLATFORM} main.cpp)

target_include_directories(testApp PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(testApp PRIVATE ${VENDOR_DIR})
target_precompile_headers(testApp PRIVATE dependencies.h)

target_link_libraries(testApp win32 dory-openGL)

install(TARGETS testApp DESTINATION . COMPONENT Example)
install(DIRECTORY configuration DESTINATION . COMPONENT Example)
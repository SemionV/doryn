cmake_minimum_required(VERSION 3.22)

project(dory-testApp)

add_executable(testApp main.cpp)
target_include_directories(testApp PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(testApp PRIVATE "${CMAKE_SOURCE_DIR}/vendor")
target_precompile_headers(testApp PRIVATE dependencies.h)

target_link_libraries(testApp testDomain dory-openGL base)
#target_link_libraries(testApp windowsImplementations)

install(TARGETS testApp DESTINATION . COMPONENT Example)
install(DIRECTORY configuration DESTINATION . COMPONENT Example)
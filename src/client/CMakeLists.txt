cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})

project(dory-client)

add_executable(client ${PLATFORM} main.cpp)

target_link_libraries(client base)

target_include_directories(client PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(client PRIVATE ${VENDOR_DIR})

install(TARGETS client DESTINATION . COMPONENT Example)
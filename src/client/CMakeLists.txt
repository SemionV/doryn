cmake_minimum_required(VERSION 3.22)

project(dory-client)

set(CMAKE_PREFIX_PATH "${VENDOR_DIR}boost" ${CMAKE_PREFIX_PATH})

find_package(Boost 1.86 REQUIRED COMPONENTS dll)

add_executable(client ${PLATFORM} main.cpp)

target_link_libraries(client base)
target_link_libraries(client ${Boost_LIBRARIES})

target_include_directories(client PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(client PRIVATE ${VENDOR_DIR})

# Include Boost directories
include_directories(${Boost_INCLUDE_DIRS})

install(TARGETS client DESTINATION . COMPONENT Example)
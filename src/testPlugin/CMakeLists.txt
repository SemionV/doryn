cmake_minimum_required(VERSION 3.22)

project(dory-testPlugin)

set(CMAKE_PREFIX_PATH "${VENDOR_DIR}boost" ${CMAKE_PREFIX_PATH})

#set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost REQUIRED COMPONENTS dll)

add_library(testPlugin MODULE ${PLATFORM}
        plugin.cpp
        factory.cpp)

if(WIN32)
    set_target_properties(testPlugin PROPERTIES PREFIX ""
            IMPORT_PREFIX ""
            IMPORT_SUFFIX ".lib"
            OUTPUT_NAME "testPlugin")
endif()

target_link_libraries(testPlugin base)
target_link_libraries(testPlugin ${Boost_LIBRARIES})

target_include_directories(testPlugin PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(testPlugin PRIVATE ${VENDOR_DIR})

# Include Boost directories
include_directories(${Boost_INCLUDE_DIRS})

install(TARGETS testPlugin DESTINATION . COMPONENT Example)
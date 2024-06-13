cmake_minimum_required(VERSION 3.22)

project(dory-testModule)

set(CMAKE_PREFIX_PATH "${VENDOR_DIR}boost" ${CMAKE_PREFIX_PATH})

#set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost REQUIRED COMPONENTS dll)

add_library(testModule MODULE ${PLATFORM}
        testModule.cpp
        factory.cpp)

if(WIN32)
    set_target_properties(testModule PROPERTIES PREFIX ""
            IMPORT_PREFIX ""
            IMPORT_SUFFIX ".lib"
            OUTPUT_NAME "testModule")
endif()

target_link_libraries(testModule base)
target_link_libraries(testModule ${Boost_LIBRARIES})

target_include_directories(testModule PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(testModule PRIVATE ${VENDOR_DIR})

# Include Boost directories
include_directories(${Boost_INCLUDE_DIRS})

install(TARGETS testModule DESTINATION ./modules COMPONENT Example)
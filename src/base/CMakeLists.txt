cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})

project(dory-base)

#set(CMAKE_PREFIX_PATH "${VENDOR_DIR}boost" ${CMAKE_PREFIX_PATH})
#find_package(Boost 1.86 REQUIRED COMPONENTS dll)
#find_package(nlohmann_json 3.2.0 PATHS ${VENDOR_DIR}nlohmann-json NO_DEFAULT_PATH REQUIRED)
#find_package(ryml PATHS ${VENDOR_DIR}rapid-yaml NO_DEFAULT_PATH REQUIRED)
find_package(spdlog PATHS ${VENDOR_DIR}spdlog NO_DEFAULT_PATH REQUIRED)

add_library(base INTERFACE
        serialization/jsonDeserializer.h
        domain/services/localizationService.h
        module.h
        domain/services/moduleService.h)

target_include_directories(base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(base INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>)
target_include_directories(base INTERFACE ${VENDOR_DIR})

# Include Boost directories
#include_directories(${Boost_INCLUDE_DIRS})
target_include_directories(base PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(base INTERFACE Boost::dll)
target_link_libraries(base INTERFACE nlohmann_json::nlohmann_json)
target_link_libraries(base INTERFACE ryml::ryml)
#target_link_libraries(base INTERFACE spdlog::spdlog_header_only) -- very long compile times on slow machines
target_link_libraries(base INTERFACE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(engine)

find_package(Boost 1.86 REQUIRED COMPONENTS dll)

add_library(engine INTERFACE
        include/dory/engine.h
        include/dory/engine/dependencies.h)

target_include_directories(engine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(engine INTERFACE
        nlohmann_json::nlohmann_json
        ryml::ryml
        ${Boost_LIBRARIES})
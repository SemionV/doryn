cmake_minimum_required(VERSION 3.22)

project(dory-base)

find_package(nlohmann_json 3.2.0 PATHS ${VENDOR_DIR}nlohmann-json NO_DEFAULT_PATH REQUIRED)
find_package(ryml PATHS ${VENDOR_DIR}rapid-yaml NO_DEFAULT_PATH REQUIRED)

add_library(base INTERFACE
        serialization/jsonDeserializer.h)

target_link_libraries(base INTERFACE nlohmann_json::nlohmann_json)
target_link_libraries(base INTERFACE ryml::ryml)
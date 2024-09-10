set(DORY_MODULE_NAME ${PROJECT_NAME}-serialization)

add_library(${DORY_MODULE_NAME} INTERFACE include/dory/serialization/yamlSerializer.h)
target_include_directories(${DORY_MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${DORY_MODULE_NAME} INTERFACE
        nlohmann_json::nlohmann_json
        ryml::ryml
        refl-cpp)
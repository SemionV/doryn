set(DORY_MODULE_NAME ${PROJECT_NAME}-generic-tools)

add_library(${DORY_MODULE_NAME} INTERFACE)
target_link_libraries(${DORY_MODULE_NAME} INTERFACE dory-generic)
target_include_directories(${DORY_MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
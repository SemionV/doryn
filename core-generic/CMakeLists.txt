set(DORY_MODULE_NAME ${PROJECT_NAME}-core-generic)

add_library(${DORY_MODULE_NAME} INTERFACE
    include/dory/core/eventSystem.h)
target_link_libraries(${DORY_MODULE_NAME} INTERFACE dory-core-interfaces)
target_include_directories(${DORY_MODULE_NAME} INTERFACE include)
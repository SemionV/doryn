set(DORY_MODULE_NAME ${PROJECT_NAME}-events)

add_library(${DORY_MODULE_NAME} INTERFACE include/dory/events.h)
target_include_directories(${DORY_MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${DORY_MODULE_NAME} INTERFACE dory-module dory-generics)
set(DORY_MODULE_NAME ${PROJECT_NAME}-types)

add_library(${DORY_MODULE_NAME} INTERFACE include/dory/types.h)
target_include_directories(${DORY_MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
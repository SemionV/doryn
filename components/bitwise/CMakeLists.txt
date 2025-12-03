set(DORY_MODULE_NAME ${PROJECT_NAME}-bitwise)

add_library(${DORY_MODULE_NAME} INTERFACE
        include/dory/bitwise/numbers.h)
target_include_directories(${DORY_MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
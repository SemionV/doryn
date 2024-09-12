set(DORY_MODULE_NAME ${PROJECT_NAME}-generics)

add_library(${DORY_MODULE_NAME} INTERFACE include/dory/generics/typeTraits.h)
target_include_directories(${DORY_MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
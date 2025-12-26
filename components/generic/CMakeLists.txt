set(DORY_MODULE_NAME ${PROJECT_NAME}-generic)

add_library(${DORY_MODULE_NAME} INTERFACE
    include/dory/generic/allocationResource.h
    include/dory/generic/registryLayer2.h)
target_include_directories(${DORY_MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
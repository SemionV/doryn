set(DORY_MODULE_NAME ${PROJECT_NAME}-memory)

add_library(${DORY_MODULE_NAME} STATIC
    src/globalAllocator.cpp
    src/allocation.cpp)
target_include_directories(${DORY_MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
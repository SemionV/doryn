set(DORY_MODULE_NAME ${PROJECT_NAME}-memory)

add_library(${DORY_MODULE_NAME} STATIC
        src/allocation.cpp
        src/allocators/pageAllocator.cpp
        include/dory/memory/allocationResource.h
        include/dory/memory/allocators/freeListAllocator.h
        include/dory/memory/allocators/segregationAllocator.h
        include/dory/memory/allocators/systemAllocator.h
        include/dory/memory/allocators/standardAllocator.h
        src/allocators/bumpAllocator.cpp
        src/allocators/dynamicAllocator.cpp
        src/memoryBlockAllocator.cpp
        src/stackAllocator.cpp)
target_include_directories(${DORY_MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${DORY_MODULE_NAME} PUBLIC dory-macros dory-concurrency)
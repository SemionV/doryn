set(DORY_MODULE_NAME ${PROJECT_NAME}-memory)

add_library(${DORY_MODULE_NAME} STATIC
        src/allocation.cpp
        src/allocators/pageAllocator.cpp
        include/dory/memory/allocators/objectBufferAllocator.h
        include/dory/memory/allocators/freeListAllocator.h
        include/dory/memory/allocators/segregationAllocator.h
        include/dory/memory/allocators/general-purpose/systemAllocator.h
        include/dory/memory/allocators/systemAllocator.h
        include/dory/memory/allocators/standardAllocator.h
        src/allocators/bumpAllocator.cpp
        src/allocators/dynamicAllocator.cpp
        src/memoryBlockAllocator.cpp
        src/stackAllocator.cpp
        include/dory/memory/profilers/memClassAuditProfiler.h
        include/dory/memory/profilers/iBlockAllocProfiler.h
        include/dory/memory/profilers/blockAuditProfiler.h
        src/profilers/blockAuditProfiler.cpp
        include/dory/memory/profilers/iFreeListAllocProfiler.h
        include/dory/memory/profilers/iObjectBufferAllocatorProfiler.h
        include/dory/memory/profilers/objectBufferAllocationProfiler.h
        src/profilers/objectBufferAllocationProfiler.cpp)
target_include_directories(${DORY_MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${DORY_MODULE_NAME} PUBLIC
        ${PROJECT_NAME}-macros
        ${PROJECT_NAME}-concurrency
        ${PROJECT_NAME}-data-structures
        ${PROJECT_NAME}-bitwise)
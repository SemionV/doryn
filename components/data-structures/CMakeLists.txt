set(DORY_MODULE_NAME ${PROJECT_NAME}-data-structures)

add_library(${DORY_MODULE_NAME} INTERFACE
        include/dory/data-structures/containers/string.h
        include/dory/data-structures/containers/list.h
        include/dory/data-structures/containers/deque.h
        include/dory/data-structures/containers/hashMap.h
        include/dory/data-structures/crc32.h
        include/dory/data-structures/hashId.h
        include/dory/data-structures/function.h
        include/dory/data-structures/containers/lockfree/freeListArray.h
        include/dory/data-structures/containers/lockfree/list.h)
target_include_directories(${DORY_MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${DORY_MODULE_NAME} INTERFACE ${PROJECT_NAME}-macros ${PROJECT_NAME}-bitwise)
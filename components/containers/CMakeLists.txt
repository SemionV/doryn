set(DORY_MODULE_NAME ${PROJECT_NAME}-containers)

add_library(${DORY_MODULE_NAME} INTERFACE
        include/dory/containers/string.h
        include/dory/containers/list.h
        include/dory/containers/deque.h
        include/dory/containers/hashMap.h
        include/dory/containers/crc32.h
        include/dory/containers/hashId.h)
target_include_directories(${DORY_MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${DORY_MODULE_NAME} INTERFACE ${PROJECT_NAME}-macros ${PROJECT_NAME}-bitwise)
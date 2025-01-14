set(DORY_MODULE_NAME ${PROJECT_NAME}-concurrency)

add_library(${DORY_MODULE_NAME} INTERFACE include/dory/concurrency/log.h)
target_include_directories(${DORY_MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${DORY_MODULE_NAME} INTERFACE readerwriterqueue concurrentqueue)
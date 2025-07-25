set(DORY_MODULE_NAME ${PROJECT_NAME}-profiling)

add_library(${DORY_MODULE_NAME} STATIC
        src/metricsReader.cpp)
target_include_directories(${DORY_MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${DORY_MODULE_NAME} PUBLIC dory-macros)
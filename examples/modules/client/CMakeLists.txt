set(DORY_MODULE_NAME ${PROJECT_NAME}-client)

add_executable(${DORY_MODULE_NAME} main.cpp)
target_include_directories(${DORY_MODULE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${DORY_MODULE_NAME} dory-engine)

target_compile_definitions(${DORY_MODULE_NAME} PRIVATE -DDORY_MAIN_FUNCTION_UNIX)

install(TARGETS ${DORY_MODULE_NAME} DESTINATION "${EXAMPLES_INSTALL_PREFIX}modular" COMPONENT Example)

add_library(${DORY_MODULE_NAME}_include INTERFACE include/dory/client/client.h)
target_include_directories(${DORY_MODULE_NAME}_include INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${DORY_MODULE_NAME}_include INTERFACE dory-engine)
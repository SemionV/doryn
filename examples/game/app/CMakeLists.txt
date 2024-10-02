set(DORY_MODULE_NAME ${PROJECT_NAME}-app)

add_executable(${DORY_MODULE_NAME} main.cpp)
set_target_properties(${DORY_MODULE_NAME} PROPERTIES
        OUTPUT_NAME ${DORY_MODULE_NAME})

target_compile_definitions(${DORY_MODULE_NAME} PRIVATE -DDORY_MAIN_FUNCTION_UNIX)

target_link_libraries(${DORY_MODULE_NAME} PRIVATE ${PROJECT_NAME}-engine)
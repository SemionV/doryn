project(tests)

add_executable(${PROJECT_NAME}
        modules.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
        engine_include
        Boost::dll)

AddTests(${PROJECT_NAME})

add_test(NAME gtests COMMAND ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} DESTINATION ${TESTS_INSTALL_PREFIX} COMPONENT Tests)
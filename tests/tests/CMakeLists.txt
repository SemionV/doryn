project(tests)

add_executable(${PROJECT_NAME}
        modules.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
        dory-generics
        dory-module
        engine_include)

AddGoogleTests(${PROJECT_NAME})
if(DORY_TEST_COVERAGE)
    AddCoverage(${PROJECT_NAME})
    CleanCoverage(${PROJECT_NAME})
    EnableCoverage(${PROJECT_NAME})
endif()
AddMemcheck(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} DESTINATION ${TESTS_INSTALL_PREFIX} COMPONENT Tests)
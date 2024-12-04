cmake_minimum_required(VERSION 3.30)
project(unit-tests)

add_executable(${PROJECT_NAME}
        enttSceneService.cpp
        pipelineRepository.cpp
        graphics/meshAssetBinderTests.cpp
        camera.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
        dory-core)

AddGoogleTests(${PROJECT_NAME})
if(DORY_TEST_COVERAGE)
    AddCoverage(${PROJECT_NAME})
    CleanCoverage(${PROJECT_NAME})
    EnableCoverage(${PROJECT_NAME})
endif()
AddMemcheck(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} DESTINATION ${TESTS_INSTALL_PREFIX} COMPONENT Tests)
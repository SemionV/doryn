cmake_minimum_required(VERSION 3.30)
project(unit-tests)

add_executable(${PROJECT_NAME}
        enttSceneService.cpp
        pipelineRepository.cpp
        graphics/meshAssetBinderTests.cpp
        camera.cpp
        graphics/geometry.cpp
        pipelineServiceTests.cpp
        serialization.cpp
        layoutTests.cpp
        layoutSetupTests.cpp
        coroutinesPlayground.cpp
        logicalGates.cpp
        memoriaTests.cpp
        src/concurrency.cpp
        src/memory.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
        dory-core
        dory-profiling)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

AddGoogleTests(${PROJECT_NAME})
if(DORY_TEST_COVERAGE)
    AddCoverage(${PROJECT_NAME})
    CleanCoverage(${PROJECT_NAME})
    EnableCoverage(${PROJECT_NAME})
endif()
AddMemcheck(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} DESTINATION ${TESTS_INSTALL_PREFIX} COMPONENT Tests)
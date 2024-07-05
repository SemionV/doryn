cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})

project(dory-benchmarks)

add_executable(baseTests linearAlgebra.cpp
        templates.cpp
        eventTests.cpp
        messagesTests.cpp
        repositoryTests.cpp
        typeMapping.cpp
        matrixCalculatorTests.cpp
        pipelineServiceTests.cpp
        dataLayoutTests.cpp
        objectVisitorTests.cpp
        concurrency.cpp
        atm_book.cpp
        scriptService.cpp
        serializationTests.cpp
        configurationTests.cpp
        modulesTest.cpp
)

target_link_libraries(baseTests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(baseTests PRIVATE engine)

target_precompile_headers(baseTests PRIVATE dependencies.h)

install(TARGETS baseTests DESTINATION . COMPONENT Tests)
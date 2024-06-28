cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})

project(dory-benchmarks)

find_package(Catch2 3 REQUIRED)
find_package(FakeIt REQUIRED
            PATHS ${VENDOR_DIR} NO_DEFAULT_PATH)
find_package(refl-cpp REQUIRED 
            PATHS ${VENDOR_DIR} NO_DEFAULT_PATH)

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
        serviceContainer.cpp
        scriptService.cpp
        serializationTests.cpp
        configurationTests.cpp
        modulesTest.cpp
)
                         
target_include_directories(baseTests PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(baseTests PRIVATE ${VENDOR_DIR})

target_link_libraries(baseTests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(baseTests PRIVATE FakeIt::FakeIt-catch)
target_link_libraries(baseTests PRIVATE base)

target_precompile_headers(baseTests PRIVATE dependencies.h)

install(TARGETS baseTests DESTINATION . COMPONENT Tests)
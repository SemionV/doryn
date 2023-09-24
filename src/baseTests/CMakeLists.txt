cmake_minimum_required(VERSION 3.22)

project(dory-base-tests)

find_package(Catch2 3 REQUIRED)
find_package(FakeIt REQUIRED
            PATHS ${VENDOR_DIR} NO_DEFAULT_PATH)

add_executable(baseTests eventTests.cpp
                         messagesTests.cpp
                         repositoryTests.cpp
                         domain/geometry/matrixCalculatorTests.cpp
                         domain/geometry/transformationCalculatorTests.cpp
                         domain/services/pipelineServiceTests.cpp)
                         
target_include_directories(baseTests PRIVATE "${CMAKE_SOURCE_DIR}")
target_include_directories(baseTests PRIVATE ${VENDOR_DIR})

target_link_libraries(baseTests PRIVATE base Catch2::Catch2WithMain)
target_link_libraries(baseTests PRIVATE FakeIt::FakeIt-catch)

install(TARGETS baseTests DESTINATION . COMPONENT Tests)
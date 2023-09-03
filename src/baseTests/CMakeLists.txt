cmake_minimum_required(VERSION 3.22)

project(dory-base-tests)

find_package(Catch2 3 REQUIRED)

add_executable(baseTests eventTests.cpp
                         messagesTests.cpp
                         domain/geometry/matrixCalculatorTests.cpp)
                         
target_include_directories(baseTests PRIVATE "${CMAKE_SOURCE_DIR}")

target_link_libraries(baseTests PRIVATE base Catch2::Catch2WithMain)

install(TARGETS baseTests DESTINATION . COMPONENT Tests)
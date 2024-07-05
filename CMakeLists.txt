set(BUILD_TOOLS_REQUIRED_VERSION 3.22)
cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(dory)

set(CMAKE_CXX_STANDARD 20)
set(INSTALL_LIBRARIES_DESTINATION ".")

if (UNIX)
    set(INSTALL_LIBRARIES_DESTINATION "/usr/lib")
endif (UNIX)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    add_compile_options(/std:c++latest)
    set(PLATFORM base/win32)
else()
    set(PLATFORM "")
endif()

set(EXAMPLES_INSTALL_PREFIX "./examples/")

add_subdirectory(external)
add_subdirectory(tests/engine)
add_subdirectory(tests/baseTests)
add_subdirectory(tests/baseBenchmarks)
add_subdirectory(examples/modularApplication)
add_subdirectory(examples/testApp)
add_subdirectory(examples/modules)

add_subdirectory(engine)
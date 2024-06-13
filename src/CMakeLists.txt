cmake_minimum_required(VERSION 3.22)

project(dory-src)

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

add_subdirectory(base)
add_subdirectory(baseTests)
add_subdirectory(baseBenchmarks)
add_subdirectory(openGL)
add_subdirectory(testApp)
add_subdirectory(testModule)
add_subdirectory(client)
cmake_minimum_required(VERSION 3.22)

project(dory)

set(INSTALL_LIBRARIES_DESTINATION ".")

if (UNIX)
    set(INSTALL_LIBRARIES_DESTINATION "/usr/lib")
endif (UNIX)

add_subdirectory(base)
add_subdirectory(baseTests)
add_subdirectory(openGL)
add_subdirectory(win32)
add_subdirectory(testDomain)
add_subdirectory(testApp)
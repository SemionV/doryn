set(BUILD_TOOLS_REQUIRED_VERSION 3.18)
cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(dory)

set(CMAKE_CXX_STANDARD 20)
set(INSTALL_LIBRARIES_DESTINATION ".")

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    add_compile_options(/std:c++latest)
    set(PLATFORM base/win32)
else()
    set(PLATFORM "")
endif()

#This option is needed to avoid STB_GNU_UNIQUE symbols in dynamic libraries, which are preventing
#library from unload(broken hot reload functionality). See here https://stackoverflow.com/questions/24467404/dlclose-doesnt-really-unload-shared-object-no-matter-how-many-times-it-is-call
add_compile_options(-fno-gnu-unique)

add_subdirectory(external)
add_subdirectory(components/engine)
add_subdirectory(components/bootstrap)

add_subdirectory(tests/engine)
add_subdirectory(tests/baseTests)
add_subdirectory(tests/baseBenchmarks)

set(EXAMPLES_INSTALL_PREFIX "./examples/")
add_subdirectory(examples/sandbox)
add_subdirectory(examples/modules)
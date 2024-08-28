set(DORY_BUILD_TOOLS_REQUIRED_VERSION 3.18)
cmake_minimum_required(VERSION ${DORY_BUILD_TOOLS_REQUIRED_VERSION})

project(dory VERSION 0.0.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
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

add_subdirectory(external EXCLUDE_FROM_ALL)
add_subdirectory(components/engine)
add_subdirectory(components/bootstrap)

set(TESTS_INSTALL_PREFIX "tests/")
add_subdirectory(tests/dory-tests)
add_subdirectory(tests/dory-tests-catch)
add_subdirectory(tests/dory-benchmarks)

set(EXAMPLES_INSTALL_PREFIX "examples/")
add_subdirectory(examples/sandbox)
add_subdirectory(examples/modules)
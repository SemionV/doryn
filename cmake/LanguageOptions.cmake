set(CMAKE_CXX_STANDARD 23)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure compiler extensions are disabled (use -std=c++20 instead of -std=gnu++20)
set(CMAKE_CXX_EXTENSIONS OFF)

#This option is needed to avoid STB_GNU_UNIQUE symbols in dynamic libraries, which are preventing
#library from unload(broken hot reload functionality). See here https://stackoverflow.com/questions/24467404/dlclose-doesnt-really-unload-shared-object-no-matter-how-many-times-it-is-call
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-fno-gnu-unique)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-fexperimental-library)
endif()

if(WIN32)
    add_definitions(-DDORY_PLATFORM_WIN32)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_definitions(-DC4_MINGW) #fix rapid-yaml library
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(-DNOMINMAX)
    endif()
elseif (LINUX)
    add_definitions(-DDORY_PLATFORM_LINUX)
endif()

add_definitions(-DASSERT_ENABLED)

if(Debug)
    add_definitions(-DDEBUG_ASSERT_ENABLED)
endif()
if(LINUX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

find_package(opengl-glad QUIET)
if(NOT opengl-glad_FOUND)
    message(STATUS "opengl-glad is not found, initializing git submodule")
    execute_process(
            COMMAND git submodule update --remote -- opengl-glad
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    add_subdirectory(opengl-glad)
endif()

message(STATUS "initializing git submodule: dory-boost-adapter")
execute_process(
        COMMAND git submodule update --remote -- dory-boost-adapter
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_subdirectory(dory-boost-adapter)

include(FetchContent)

FetchContent_Declare(nlohmann-json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3)
FetchContent_MakeAvailable(nlohmann-json)

if(POLICY CMP0177)
    cmake_policy(SET CMP0177 NEW)
endif()
FetchContent_Declare(rapid-yaml
        GIT_REPOSITORY https://github.com/biojppm/rapidyaml
        GIT_TAG	v0.7.0)
FetchContent_MakeAvailable(rapid-yaml)
target_compile_options(ryml PRIVATE -fPIC)

FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.14.1)
set(SAVED_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
FetchContent_MakeAvailable(spdlog)
set(CMAKE_CXX_FLAGS "${SAVED_CMAKE_CXX_FLAGS}")

FetchContent_Declare(reflcpp
        GIT_REPOSITORY https://github.com/veselink1/refl-cpp.git)
FetchContent_MakeAvailable(reflcpp)

FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(efsw
        GIT_REPOSITORY https://github.com/SpartanJ/efsw.git)
FetchContent_MakeAvailable(efsw)

FetchContent_Declare(EnTT
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.14.0)
FetchContent_MakeAvailable(EnTT)

FetchContent_Declare(glm
        GIT_REPOSITORY	https://github.com/g-truc/glm.git
        GIT_TAG	1.0.1
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
        readerwriterqueue
        GIT_REPOSITORY    https://github.com/cameron314/readerwriterqueue
        GIT_TAG           master
)
FetchContent_MakeAvailable(readerwriterqueue)

FetchContent_Declare(
        readerwriterqueue
        GIT_REPOSITORY    https://github.com/cameron314/readerwriterqueue
        GIT_TAG           master
)
FetchContent_MakeAvailable(readerwriterqueue)

FetchContent_Declare(
        concurrentqueue
        GIT_REPOSITORY    https://github.com/cameron314/concurrentqueue
)
FetchContent_MakeAvailable(concurrentqueue)

FetchContent_Declare(
        magic_enum
        GIT_REPOSITORY    https://github.com/Neargye/magic_enum.git
)
FetchContent_MakeAvailable(magic_enum)
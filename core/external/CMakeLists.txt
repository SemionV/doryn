message(STATUS "initializing git submodule: dory-boost-adapter")
execute_process(
        COMMAND git submodule update --remote -- dory-boost-adapter
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
set(DORY_DEPENDENCY_TARGET_NAME "core-boost-adapter")
set(DORY_DEPENDENCY_TARGET_ALIAS "core::boost")
add_subdirectory(dory-boost-adapter)
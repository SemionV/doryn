cmake_minimum_required(VERSION 3.30)
project(sandbox VERSION 0.0.1)

set(DORY_SANDBOX_INSTALL_PREFIX "${EXAMPLES_INSTALL_PREFIX}sandbox/")
set(DORY_SANDBOX_MODULES_INSTALL_PREFIX "${DORY_SANDBOX_INSTALL_PREFIX}modules/")
set(DORY_SANDBOX_COMPONENT Sandbox)

add_subdirectory(modules/main)
add_subdirectory(modules/extension)
add_subdirectory(app)
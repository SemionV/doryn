cmake_minimum_required(VERSION ${DORY_BUILD_TOOLS_REQUIRED_VERSION})
project(dory-examples-modular-application)

set(DORY_SANDBOX_INSTALL_PREFIX "${EXAMPLES_INSTALL_PREFIX}sandbox/")
set(DORY_SANDBOX_MODULES_INSTALL_PREFIX "${DORY_SANDBOX_INSTALL_PREFIX}modules/")
set(DORY_SANDBOX_COMPONENT Sandbox)

add_subdirectory(modules/main)
add_subdirectory(modules/extension bin/modules)
add_subdirectory(app bin)
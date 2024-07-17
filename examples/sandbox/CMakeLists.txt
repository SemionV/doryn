cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(dory-examples-modular-application)

set(DORY_SANDBOX_INSTALL_PREFIX "${EXAMPLES_INSTALL_PREFIX}sandbox")

add_subdirectory(modules/main)
add_subdirectory(modules/extension)
add_subdirectory(app)
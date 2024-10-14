cmake_minimum_required(VERSION 3.30)
project(game VERSION 0.0.1)

set(DORY_INSTALL_PREFIX "${EXAMPLES_INSTALL_PREFIX}game/")
set(DORY_MODULES_INSTALL_PREFIX "${DORY_INSTALL_PREFIX}modules/")
set(DORY_COMPONENT Game)

add_subdirectory(app)
add_subdirectory(engine)
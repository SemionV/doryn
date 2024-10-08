cmake_minimum_required(VERSION 3.30)
project(game VERSION 0.0.1)

set(DORY_GAME_INSTALL_PREFIX "${EXAMPLES_INSTALL_PREFIX}game/")
set(DORY_GAME_MODULES_INSTALL_PREFIX "${DORY_GAME_INSTALL_PREFIX}modules/")
set(DORY_GAME_COMPONENT Game)

add_subdirectory(app)
add_subdirectory(engine)
cmake_minimum_required(VERSION ${DORY_BUILD_TOOLS_REQUIRED_VERSION})

project(dory-client)

add_executable(client ${PLATFORM} main.cpp)
target_include_directories(client PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(client engine)

install(TARGETS client DESTINATION "${EXAMPLES_INSTALL_PREFIX}modular" COMPONENT Example)

add_library(client_mod INTERFACE include/dory/client/client.h)
target_include_directories(client_mod INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(client_mod INTERFACE engine)
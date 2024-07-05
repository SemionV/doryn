cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(modularApp)

add_executable(modularApp ${PLATFORM} main.cpp)
set_target_properties(modularApp PROPERTIES
        OUTPUT_NAME "app")

target_link_libraries(modularApp PRIVATE engine)

install(TARGETS modularApp DESTINATION "${EXAMPLES_INSTALL_PREFIX}modularApplication" COMPONENT Example)
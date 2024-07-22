cmake_minimum_required(VERSION ${DORY_BUILD_TOOLS_REQUIRED_VERSION})

project(dory-testModule)

add_library(testModule MODULE ${PLATFORM}
        testModule2.cpp
        factory.cpp)

set_target_properties(testModule PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME "testModule")

target_link_libraries(testModule client_mod)

install(TARGETS testModule DESTINATION "${EXAMPLES_INSTALL_PREFIX}modular/modules" COMPONENT Example)
cmake_minimum_required(VERSION ${DORY_BUILD_TOOLS_REQUIRED_VERSION})

project(modular-example-testModule)

add_library(${PROJECT_NAME} MODULE ${PLATFORM}
        testModule2.cpp
        factory.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME "testModule")

target_link_libraries(${PROJECT_NAME} modular-example-client_include)

install(TARGETS ${PROJECT_NAME} DESTINATION "${EXAMPLES_INSTALL_PREFIX}modular/modules" COMPONENT Example)
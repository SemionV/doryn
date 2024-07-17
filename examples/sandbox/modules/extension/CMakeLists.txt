cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(extensionModule)

add_library(${PROJECT_NAME} MODULE src/extension.cpp
        src/factory.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE engine mainModule_mod)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME "extension")

install(TARGETS ${PROJECT_NAME} DESTINATION modules COMPONENT Sandbox)
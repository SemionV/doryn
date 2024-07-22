cmake_minimum_required(VERSION ${DORY_BUILD_TOOLS_REQUIRED_VERSION})
project(extensionModule)

add_library(${PROJECT_NAME} MODULE src/extension.cpp
        src/factory.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE mainModule_mod)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME "extension")

set(DORY_SANDBOX_EXTENSION_COMPONENT ${DORY_SANDBOX_COMPONENT}-Extension)

install(TARGETS ${PROJECT_NAME} DESTINATION ${DORY_SANDBOX_MODULES_INSTALL_PREFIX} COMPONENT ${DORY_SANDBOX_EXTENSION_COMPONENT})

add_custom_target(install_${PROJECT_NAME}
        "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --component ${DORY_SANDBOX_EXTENSION_COMPONENT}
        DEPENDS ${PROJECT_NAME}
        COMMENT "Installing ${PROJECT_NAME}")
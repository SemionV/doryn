set(DORY_MODULE_NAME ${PROJECT_NAME}-extension)

add_library(${DORY_MODULE_NAME} MODULE src/extension.cpp
        src/factory.cpp)
target_link_libraries(${DORY_MODULE_NAME} PRIVATE sandbox-mainModule_include)
target_include_directories(${DORY_MODULE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(${DORY_MODULE_NAME} PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME "extension")

set(DORY_SANDBOX_EXTENSION_COMPONENT ${DORY_SANDBOX_COMPONENT}-Extension)

install(TARGETS ${DORY_MODULE_NAME} DESTINATION ${DORY_SANDBOX_MODULES_INSTALL_PREFIX} COMPONENT ${DORY_SANDBOX_EXTENSION_COMPONENT})

add_custom_target(${DORY_MODULE_NAME}_install
        "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --component ${DORY_SANDBOX_EXTENSION_COMPONENT}
        DEPENDS ${DORY_MODULE_NAME}
        COMMENT "Installing ${DORY_MODULE_NAME}")
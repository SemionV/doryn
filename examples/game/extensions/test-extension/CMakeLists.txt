set(DORY_MODULE_NAME ${PROJECT_NAME}-test-extension)

add_library(${DORY_MODULE_NAME} MODULE src/extension.cpp
        src/factory.cpp)
target_link_libraries(${DORY_MODULE_NAME} PRIVATE dory-core-interfaces Boost::dll)
target_include_directories(${DORY_MODULE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(${DORY_MODULE_NAME} PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME "test-extension")

if(APPLE)
    set_target_properties(${DORY_MODULE_NAME} PROPERTIES
            SUFFIX ".dylib"
    )
endif()

set(DORY_GAME_EXTENSION_COMPONENT ${DORY_COMPONENT}-TestExtension)

include(CppCheck)
AddCppCheck(${DORY_MODULE_NAME})

install(TARGETS ${DORY_MODULE_NAME} DESTINATION ${DORY_MODULES_INSTALL_PREFIX} COMPONENT ${DORY_GAME_EXTENSION_COMPONENT})

add_custom_target(${DORY_MODULE_NAME}_install
        "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --config Debug --component ${DORY_GAME_EXTENSION_COMPONENT}
        DEPENDS ${DORY_MODULE_NAME}
        COMMENT "Installing ${DORY_MODULE_NAME}")
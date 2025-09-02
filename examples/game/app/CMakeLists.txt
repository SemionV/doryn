set(DORY_MODULE_NAME ${PROJECT_NAME}-app)

add_executable(${DORY_MODULE_NAME}
        main.cpp
        src/logic/mainSceneKeyboardHandler.cpp
        src/logic/cameraService.cpp)
set_target_properties(${DORY_MODULE_NAME} PROPERTIES
        OUTPUT_NAME ${DORY_MODULE_NAME})

if(DORY_PLATFORM_WIN32)
    target_compile_definitions(${DORY_MODULE_NAME} PRIVATE -DDORY_MAIN_FUNCTION_WIN32)
else()
    target_compile_definitions(${DORY_MODULE_NAME} PRIVATE -DDORY_MAIN_FUNCTION_UNIX)
endif()

target_link_libraries(${DORY_MODULE_NAME} PRIVATE ${PROJECT_NAME}-engine dory-thread-lifetime-trace)
target_include_directories(${DORY_MODULE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (DORY_PROFILING)
    target_link_libraries(${DORY_MODULE_NAME} PRIVATE ${PROJECT_NAME}-profiling ${PROJECT_NAME}-alloc-forceinclude ${PROJECT_NAME}-custom-new-delete)
else()
    target_link_libraries(${DORY_MODULE_NAME} PRIVATE ${PROJECT_NAME}-profiling-interface)
endif()

include(BuildInfoSettings)
BuildInfoSettings(${DORY_INSTALL_PREFIX} ${DORY_COMPONENT})

set(FILES_TO_INSTALL settings.yaml)
set(DIRECTORIES_TO_INSTALL configuration scenes)

install(TARGETS ${DORY_MODULE_NAME} DESTINATION ${DORY_INSTALL_PREFIX} COMPONENT ${DORY_COMPONENT})
install(DIRECTORY ${DIRECTORIES_TO_INSTALL} DESTINATION ${DORY_INSTALL_PREFIX} COMPONENT ${DORY_COMPONENT})
install(FILES ${FILES_TO_INSTALL} DESTINATION ${DORY_INSTALL_PREFIX} COMPONENT ${DORY_COMPONENT})
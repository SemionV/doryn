cmake_minimum_required(VERSION 3.22)

project(dory-base)

add_library(base SHARED basicFrameService.cpp
                        controller.cpp
                        engine.cpp
                        frameService.cpp
                        types.cpp
                        controllers/inputController.cpp
                        devices/device.cpp
                        devices/deviceListener.cpp
                        messaging/messageFactory.cpp
                        messaging/message.cpp
                        messaging/deviceMessage.cpp
                        messaging/messagePool.cpp
                        messaging/messages/consoleMessage.cpp
                        messaging/messages/windowMessage.cpp
                        repositories/resourceScopeRepository.cpp
                        multithreading/task.cpp
                        multithreading/currentProcessThread.cpp
                        multithreading/individualProcessThread.cpp)

set_target_properties(base PROPERTIES PREFIX ""
                                    IMPORT_PREFIX ""
                                    IMPORT_SUFFIX ".lib"
                                    OUTPUT_NAME "base")

target_compile_definitions(base PRIVATE DORY_EXPORT)
target_include_directories(base PRIVATE "${CMAKE_SOURCE_DIR}")
target_precompile_headers(base PRIVATE dependencies.h)

install(TARGETS base
        RUNTIME DESTINATION ${INSTALL_LIBRARIES_DESTINATION} COMPONENT Engine
        LIBRARY DESTINATION ${INSTALL_LIBRARIES_DESTINATION} COMPONENT Engine
        ARCHIVE DESTINATION lib COMPONENT EngineDevelopment)
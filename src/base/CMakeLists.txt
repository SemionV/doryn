cmake_minimum_required(VERSION 3.22)

project(dory-base)

add_library(base SHARED configuration/fileSystemBasedConfiguration.cpp
                        basicFrameService.cpp
                        engine.cpp
                        frameService.cpp
                        types.cpp
                        multithreading/task.cpp
                        multithreading/currentProcessThread.cpp
                        multithreading/individualProcessThread.cpp
                        domain/geometry/matrixCalculator.cpp
                        domain/geometry/transformationCalculator.cpp)

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
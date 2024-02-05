cmake_minimum_required(VERSION 3.22)

project(dory-base)

add_library(base SHARED configuration/fileSystemBasedConfiguration.cpp
        concurrency/task.cpp
        concurrency/currentProcessThread.cpp
        concurrency/individualProcessThread.cpp
                        domain/services/matrixCalculatorService.cpp
                        domain/services/transformationCalculatorService.cpp
                        domain/services/pipelineService.cpp
        concurrency/queue.h
)

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
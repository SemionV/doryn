cmake_minimum_required(VERSION 3.22)

project(dory-testDomain)

add_library(testDomain SHARED stepFrameService.cpp
                                testLogic.cpp)

set_target_properties(testDomain PROPERTIES PREFIX ""
                                            IMPORT_PREFIX ""
                                            IMPORT_SUFFIX ".lib"
                                            OUTPUT_NAME "testDomain")

target_compile_definitions(testDomain PRIVATE DORY_EXPORT)
target_include_directories(testDomain PRIVATE "${CMAKE_SOURCE_DIR}")
target_precompile_headers(testDomain PRIVATE dependencies.h)


#Library to interact with unix console
if (UNIX)
        find_package(Curses)
        target_link_libraries(testDomain ${CURSES_LIBRARIES})
endif (UNIX)

target_link_libraries(testDomain base)

install(TARGETS testDomain
        RUNTIME DESTINATION ${INSTALL_LIBRARIES_DESTINATION} COMPONENT Example
        LIBRARY DESTINATION ${INSTALL_LIBRARIES_DESTINATION} COMPONENT Example
        ARCHIVE DESTINATION lib COMPONENT ExampleDevelopment)
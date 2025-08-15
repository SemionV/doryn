set(DORY_MODULE_NAME ${PROJECT_NAME}-thread-lifetime-trace)

add_library(${DORY_MODULE_NAME} SHARED
        src/hook.cpp)

if(LINUX)
    target_link_libraries(${DORY_MODULE_NAME} PRIVATE dl)
endif()

target_link_libraries(${DORY_MODULE_NAME} PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)

if (DORY_PROFILING)
    target_link_libraries(${DORY_MODULE_NAME} PUBLIC dory-profiling)
else()
    target_link_libraries(${DORY_MODULE_NAME} PUBLIC dory-profiling-interface)
endif()

set_target_properties(${DORY_MODULE_NAME} PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME ${DORY_MODULE_NAME})

target_compile_definitions(${DORY_MODULE_NAME} PRIVATE -DDORY_DLL_EXPORTS)

if(LINUX)
    target_compile_options(${DORY_MODULE_NAME} PRIVATE -fPIC)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${DORY_MODULE_NAME} PRIVATE /bigobj)
endif()

install(TARGETS ${DORY_MODULE_NAME}
        DESTINATION ${DORY_INSTALL_PREFIX}
        ARCHIVE DESTINATION ${DORY_INSTALL_PREFIX}/dev
        FILE_SET modules DESTINATION ${DORY_INSTALL_PREFIX}/dev/modules
        CXX_MODULES_BMI DESTINATION ${DORY_INSTALL_PREFIX}/dev
        COMPONENT ${DORY_COMPONENT})
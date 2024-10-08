set(DORY_MODULE_NAME ${PROJECT_NAME}-core)

add_library(${DORY_MODULE_NAME} STATIC
        src/services/fileService.cpp
        src/services/libraryService.cpp
        src/extensionPlatform/dynamicLibrary.cpp)
target_include_directories(${DORY_MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${DORY_MODULE_NAME} PUBLIC dory-core-interfaces core::boost)
set_target_properties(${DORY_MODULE_NAME} PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME "dory-core")

if(LINUX)
    target_compile_options(${DORY_MODULE_NAME} PRIVATE -fPIC)
endif()

add_subdirectory(external)
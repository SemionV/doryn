set(DORY_MODULE_NAME ${PROJECT_NAME}-core-interfaces)

add_library(${DORY_MODULE_NAME} INTERFACE
        include/dory/core/services/iFileService.h
        include/dory/core/registry.h)

target_link_libraries(${DORY_MODULE_NAME} INTERFACE dory-generic)

target_sources(${DORY_MODULE_NAME} INTERFACE
    FILE_SET HEADERS
    BASE_DIRS
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    FILES
        include/dory/core/services/iFileService.h
        include/dory/core/services/iLibraryService.h
        include/dory/core/registry.h
        include/dory/core/iSetup.h)
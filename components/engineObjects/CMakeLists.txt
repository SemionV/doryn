set(DORY_MODULE_NAME ${PROJECT_NAME}-engine-objects)

add_library(${DORY_MODULE_NAME} STATIC
        src/dataContext.cpp
        src/devices.cpp
        src/repositories.cpp
        src/services.cpp
        src/controllers/pipelineController.cpp)
target_include_directories(${DORY_MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${DORY_MODULE_NAME} PUBLIC dory-engine)
set_target_properties(${DORY_MODULE_NAME} PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME "dory-engine")
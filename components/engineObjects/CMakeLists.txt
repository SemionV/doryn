set(DORY_MODULE_NAME ${PROJECT_NAME}-engine-objects)

add_library(${DORY_MODULE_NAME} SHARED
        src/resources/dataContext.cpp
        src/events/mainController.cpp
        src/resources/entity.cpp
        src/repositories/pipelineRepository.cpp
        src/controllers/mainController.cpp
        src/devices.cpp)
target_include_directories(${DORY_MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${DORY_MODULE_NAME} PUBLIC dory-engine)
set_target_properties(${DORY_MODULE_NAME} PROPERTIES PREFIX ""
        IMPORT_PREFIX ""
        OUTPUT_NAME "dory-engine")
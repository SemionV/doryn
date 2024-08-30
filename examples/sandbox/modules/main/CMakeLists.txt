set(DORY_MODULE_NAME ${PROJECT_NAME}-mainModule)

add_library(${DORY_MODULE_NAME} STATIC src/mainModule.cpp
        include/dory/sandbox/mainModule/registry.h
        include/dory/sandbox/mainModule/extensionContext.h)
target_link_libraries(${DORY_MODULE_NAME} PRIVATE engine bootstrap_include)
target_include_directories(${DORY_MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(${DORY_MODULE_NAME}_include INTERFACE include/dory/sandbox/mainModule.h)
target_include_directories(${DORY_MODULE_NAME}_include INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${DORY_MODULE_NAME}_include INTERFACE engine)
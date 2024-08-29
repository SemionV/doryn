project(sandbox-mainModule)

add_library(${PROJECT_NAME} STATIC src/mainModule.cpp
        include/dory/sandbox/mainModule/registry.h
        include/dory/sandbox/mainModule/extensionContext.h)
target_link_libraries(${PROJECT_NAME} PRIVATE engine dory-bootstrap_include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(${PROJECT_NAME}_include INTERFACE include/dory/sandbox/mainModule.h)
target_include_directories(${PROJECT_NAME}_include INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}_include INTERFACE engine)
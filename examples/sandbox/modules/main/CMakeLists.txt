cmake_minimum_required(VERSION ${DORY_BUILD_TOOLS_REQUIRED_VERSION})
project(mainModule)

add_library(${PROJECT_NAME} STATIC src/mainModule.cpp
        include/dory/sandbox/mainModule/registry.h
        include/dory/sandbox/mainModule/extensionContext.h)
target_link_libraries(${PROJECT_NAME} PRIVATE engine dory-bootstrap_mod)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(${PROJECT_NAME}_mod INTERFACE include/dory/sandbox/mainModule.h)
target_include_directories(${PROJECT_NAME}_mod INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}_mod INTERFACE engine)
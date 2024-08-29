project(modular-example-client)

add_executable(${PROJECT_NAME} ${PLATFORM} main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME} engine)

install(TARGETS ${PROJECT_NAME} DESTINATION "${EXAMPLES_INSTALL_PREFIX}modular" COMPONENT Example)

add_library(${PROJECT_NAME}_include INTERFACE include/dory/client/client.h)
target_include_directories(${PROJECT_NAME}_include INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME}_include INTERFACE engine)
cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(sandbox-app)

add_executable(${PROJECT_NAME} ${PLATFORM} main.cpp src/app.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "sandbox")

target_link_libraries(${PROJECT_NAME} PRIVATE engine mainModule dory-bootstrapper_mod)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS ${PROJECT_NAME} DESTINATION "${EXAMPLES_INSTALL_PREFIX}sandbox" COMPONENT Example)
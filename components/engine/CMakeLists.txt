project(engine)

add_library(${PROJECT_NAME} INTERFACE include/dory/registry.h)

target_include_directories(${PROJECT_NAME} INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(${PROJECT_NAME} INTERFACE
        dory-generics
        dory-math
        dory-serialization
        dory-concurrency
        dory-module
        dory-events
        opengl-glad
        glfw
        spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
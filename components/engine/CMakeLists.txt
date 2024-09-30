set(DORY_MODULE_NAME ${PROJECT_NAME}-engine)

add_library(${DORY_MODULE_NAME} STATIC include/dory/registry.h)

target_include_directories(${DORY_MODULE_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(${DORY_MODULE_NAME} PUBLIC
        dory-generics
        dory-math
        dory-serialization
        dory-concurrency
        dory-module
        dory-events
        opengl-glad
        glfw
        spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
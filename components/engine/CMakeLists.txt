project(engine)

add_library(${PROJECT_NAME} STATIC
        src/rendering/opengl-utils.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        opengl-glad
        glfw
        nlohmann_json::nlohmann_json
        ryml::ryml
        Boost::dll
        spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>
        refl-cpp)

add_library(${PROJECT_NAME}_include INTERFACE include/dory/engine/typeComponents.h)
target_include_directories(${PROJECT_NAME}_include INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
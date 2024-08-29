project(engine)

add_library(${PROJECT_NAME} STATIC
        src/rendering/opengl-utils.cpp)

#target_precompile_headers(${PROJECT_NAME} PRIVATE include/dory/engine/dependencies.h)

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} PUBLIC
        opengl-glad
        glfw
        nlohmann_json::nlohmann_json
        ryml::ryml
        Boost::dll
        spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>
        refl-cpp)

add_library(${PROJECT_NAME}_include INTERFACE include/dory/engine/typeComponents.h)
target_include_directories(${PROJECT_NAME}_include INTERFACE include)
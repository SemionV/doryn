cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
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

add_library(inc_${PROJECT_NAME} INTERFACE include/dory/engine/dependencies.h)
target_include_directories(inc_${PROJECT_NAME} INTERFACE include)
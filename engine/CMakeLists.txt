cmake_minimum_required(VERSION ${BUILD_TOOLS_REQUIRED_VERSION})
project(engine)

add_library(engine STATIC
        src/rendering/opengl-utils.cpp
        include/dory/engine.h include/dory/registry.h)

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(engine PUBLIC
        opengl-glad
        glfw
        nlohmann_json::nlohmann_json
        ryml::ryml
        Boost::dll
        spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>
        refl-cpp)
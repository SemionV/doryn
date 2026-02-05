set(DORY_MODULE_NAME ${PROJECT_NAME}-graphics-opengl)

add_library(${DORY_MODULE_NAME} STATIC
        include/dory/graphics/opengl/caps.h
        include/dory/graphics/opengl/compatibilityLayer.h
        src/compatibilityLayer.cpp)
target_include_directories(${DORY_MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${DORY_MODULE_NAME} PUBLIC opengl-glad)
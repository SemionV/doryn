set(DORY_MODULE_NAME ${PROJECT_NAME}-math)

add_library(${DORY_MODULE_NAME} STATIC src/geometryMorphing.cpp)
target_include_directories(${DORY_MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${DORY_MODULE_NAME} PUBLIC glm::glm)

if(LINUX)
    target_compile_options(${DORY_MODULE_NAME} PRIVATE -fPIC)
endif()
set(DORY_MODULE_NAME ${PROJECT_NAME}-macros)

add_library(${DORY_MODULE_NAME} STATIC src/assert.cpp)
target_include_directories(${DORY_MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(LINUX)
    target_compile_options(${DORY_MODULE_NAME} PRIVATE -fPIC)
endif()
project(dory-bootstrap)

add_library(${PROJECT_NAME}_include INTERFACE include/dory/bootstrap.h)
target_include_directories(${PROJECT_NAME}_include INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
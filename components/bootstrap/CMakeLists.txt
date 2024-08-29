project(dory-bootstrap)

add_library(${PROJECT_NAME}_include INTERFACE include/dory/bootstrap.h)
target_include_directories(${PROJECT_NAME}_include INTERFACE include)
project(benchmarks)

add_executable(${PROJECT_NAME} concurrency.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE dory-engine dory-concurrency benchmark::benchmark)

install(TARGETS ${PROJECT_NAME} DESTINATION ${TESTS_INSTALL_PREFIX} COMPONENT Tests)
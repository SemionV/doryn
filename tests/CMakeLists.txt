set(TESTS_INSTALL_PREFIX "tests/")

include(Testing)

add_subdirectory(tests)
add_subdirectory(tests-catch)
add_subdirectory(benchmarks)
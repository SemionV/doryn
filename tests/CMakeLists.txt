set(TESTS_INSTALL_PREFIX "tests/")

include(Testing)
enable_testing()

add_subdirectory(tests)
add_subdirectory(tests-catch)
add_subdirectory(benchmarks)
set(TESTS_INSTALL_PREFIX "tests/")

include(Testing)
include(Coverage)
include(Memcheck)

add_subdirectory(unit-tests)
add_subdirectory(benchmarks)
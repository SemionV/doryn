. "build/build-library.sh"
BUILD_CONFIG=Release
installDependency "Google benchmark" "google-benchmark" "https://github.com/google/benchmark.git" "-DBENCHMARK_ENABLE_GTEST_TESTS=OFF"
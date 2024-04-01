. "build/settings.sh"
printInstallDependency "Google benchmark"

GB_BUILD_CONFIG=Release

GOOGLE_BENCHMARK_DIR=${LIB_DIR}google-benchmark/
mkdir -p "$GOOGLE_BENCHMARK_DIR"
git clone https://github.com/google/benchmark.git "$GOOGLE_BENCHMARK_DIR"
cd $GOOGLE_BENCHMARK_DIR
cmake -S . -B "$CMAKE_BUILD_DIR" -DCMAKE_BUILD_TYPE=$GB_BUILD_CONFIG -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DCMAKE_INSTALL_PREFIX=${VENDOR_DIR}google-benchmark/ -G "$CMAKE_GENERATOR"
cmake --build "$CMAKE_BUILD_DIR" --config "$GB_BUILD_CONFIG"
cmake --install "$CMAKE_BUILD_DIR" --config "$GB_BUILD_CONFIG"
cd -
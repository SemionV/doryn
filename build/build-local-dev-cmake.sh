source ./build/settings.sh

cmake -S src -B "$CMAKE_BUILD_DIR" -DCMAKE_BUILD_TYPE=$BUILD_CONFIG -DVENDOR_DIR="${VENDOR_DIR}" -G "$CMAKE_GENERATOR"
cmake --build "$CMAKE_BUILD_DIR" --config "$BUILD_CONFIG"
cmake --install "$CMAKE_BUILD_DIR" --component Engine --prefix bin --config "$BUILD_CONFIG"
cmake --install "$CMAKE_BUILD_DIR" --component Tests --prefix bin --config "$BUILD_CONFIG"
cmake --install "$CMAKE_BUILD_DIR" --component Example --prefix bin --config "$BUILD_CONFIG"
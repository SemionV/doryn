. build/settings.sh

if [ -n "$2" ]; then
    BUILD_CONFIG="$2"
fi

cmake -S . -B "$CMAKE_BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_CONFIG" -DVENDOR_DIR="${VENDOR_DIR}" -G "$CMAKE_GENERATOR"
if [ -n "$1" ]; then
    cmake --build "$CMAKE_BUILD_DIR" --config "$BUILD_CONFIG" --target "$1"
else
    cmake --build "$CMAKE_BUILD_DIR" --config "$BUILD_CONFIG"
fi

cmake --install "$CMAKE_BUILD_DIR" --component Engine --prefix bin --config "$BUILD_CONFIG"
cmake --install "$CMAKE_BUILD_DIR" --component Tests --prefix bin --config "$BUILD_CONFIG"
cmake --install "$CMAKE_BUILD_DIR" --component Example --prefix bin --config "$BUILD_CONFIG"
cmake --install "$CMAKE_BUILD_DIR" --component Sandbox --prefix bin --config "$BUILD_CONFIG"
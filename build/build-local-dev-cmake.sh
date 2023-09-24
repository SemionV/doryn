source ./build/settings.sh

cmake -S src -B build/cmake-artifacts -DCMAKE_BUILD_TYPE=Debug -DVENDOR_DIR="${VENDOR_DIR}"
cmake --build build/cmake-artifacts
cmake --install build/cmake-artifacts --component Engine --prefix bin
cmake --install build/cmake-artifacts --component Tests --prefix bin
cmake --install build/cmake-artifacts --component Example --prefix bin
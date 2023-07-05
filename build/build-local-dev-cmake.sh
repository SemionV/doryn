cmake -S src -B build/cmake-artifacts -DCMAKE_BUILD_TYPE=Debug
cmake --build build/cmake-artifacts
cmake --install build/cmake-artifacts --component Engine --prefix bin
cmake --install build/cmake-artifacts --component Example --prefix bin
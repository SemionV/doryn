git clone --recursive https://github.com/SemionV/doryn.git doryn
cd doryn
cmake -S . -B build/cmake-artifacts -DCMAKE_BUILD_TYPE="Release"
cmake --build build/cmake-artifacts
cmake --install build/cmake-artifacts  --prefix bin
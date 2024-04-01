. "build/settings.sh"
printInstallDependency "refl-cpp"

REFL_CPP_DIR=${LIB_DIR}refl-cpp/
mkdir -p $REFL_CPP_DIR
git clone https://github.com/veselink1/refl-cpp.git $REFL_CPP_DIR
cd $REFL_CPP_DIR
cmake -S . -B "$CMAKE_BUILD_DIR" -DCMAKE_BUILD_TYPE=$BUILD_CONFIG -DCMAKE_INSTALL_PREFIX=${VENDOR_DIR}refl-cpp/ -G "$CMAKE_GENERATOR"
cmake --build "$CMAKE_BUILD_DIR" --config "$BUILD_CONFIG"
cmake --install "$CMAKE_BUILD_DIR" --config "$BUILD_CONFIG"
cd -
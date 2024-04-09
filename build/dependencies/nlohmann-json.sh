. "build/settings.sh"
printInstallDependency "nlohmann-json"

NLOHMANN_JSON_DIR=${LIB_DIR}nlohmann-json/
mkdir -p $NLOHMANN_JSON_DIR
git clone https://github.com/nlohmann/json.git $NLOHMANN_JSON_DIR
cd $NLOHMANN_JSON_DIR
cmake -S . -B "$CMAKE_BUILD_DIR" -DCMAKE_BUILD_TYPE=$BUILD_CONFIG -DCMAKE_INSTALL_PREFIX=${VENDOR_DIR}nlohmann-json/ -G "$CMAKE_GENERATOR"
cmake --build "$CMAKE_BUILD_DIR" --config "$BUILD_CONFIG"
cmake --install "$CMAKE_BUILD_DIR" --config "$BUILD_CONFIG"
cd -
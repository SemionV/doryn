. "build/settings.sh"
printInstallDependency "rapid-yaml"

RAPID_YAML_DIR=${LIB_DIR}rapid-yaml/
mkdir -p $RAPID_YAML_DIR
git clone --recursive https://github.com/biojppm/rapidyaml $RAPID_YAML_DIR
cd $RAPID_YAML_DIR
cmake -S . -B "$CMAKE_BUILD_DIR" -DCMAKE_BUILD_TYPE=$BUILD_CONFIG -DCMAKE_INSTALL_PREFIX=${VENDOR_DIR}rapid-yaml/ -G "$CMAKE_GENERATOR"
cmake --build "$CMAKE_BUILD_DIR" --config "$BUILD_CONFIG"
cmake --install "$CMAKE_BUILD_DIR" --config "$BUILD_CONFIG"
cd -
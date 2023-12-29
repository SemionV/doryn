source "./build/settings.sh"
printInstallDependency "FakeIt"

FAKEIT_DIR=${LIB_DIR}FakeIt/
mkdir -p $FAKEIT_DIR
git clone https://github.com/eranpeer/FakeIt.git ${FAKEIT_DIR}
cd $FAKEIT_DIR
cmake -S . -B "$CMAKE_BUILD_DIR" -DCMAKE_INSTALL_PREFIX=${VENDOR_DIR}FakeIt/ -G "$CMAKE_GENERATOR"
cmake --build "$CMAKE_BUILD_DIR" --config "$BUILD_CONFIG"
cmake --install "$CMAKE_BUILD_DIR" --config "$BUILD_CONFIG"
cd -
source "./build/settings.sh"
printInstallDependency "GLFW"

GLFW_DIR=${LIB_DIR}glfw/
mkdir -p $GLFW_DIR
git clone https://github.com/glfw/glfw.git $GLFW_DIR
cd $GLFW_DIR
cmake -S . -B "$CMAKE_BUILD_DIR" -DCMAKE_BUILD_TYPE=$BUILD_CONFIG -DCMAKE_INSTALL_PREFIX=${VENDOR_DIR}glfw/ -G "$CMAKE_GENERATOR"
cmake --build "$CMAKE_BUILD_DIR" --config "$BUILD_CONFIG"
cmake --install "$CMAKE_BUILD_DIR" --config "$BUILD_CONFIG"
cd -
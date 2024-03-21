source "./build/settings.sh"
printInstallDependency "Catch2"

CATCH2_DIR=${LIB_DIR}Catch2/
mkdir -p $CATCH2_DIR
git clone https://github.com/catchorg/Catch2.git ${CATCH2_DIR}
cd $CATCH2_DIR
cmake -B "$CMAKE_BUILD_DIR" -H. -DBUILD_TESTING=OFF -G "$CMAKE_GENERATOR"
cmake --build "$CMAKE_BUILD_DIR" --target install --config "$BUILD_CONFIG"
cd -
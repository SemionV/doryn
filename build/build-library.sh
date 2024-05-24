. "build/settings.sh"

printInstallDependency()
{
    echo ""
    echo "====================================================Installing: ${1}"
    echo ""
}

installDependency()
{
  NAME=${1}
  DIRECTORY=${2}
  GIT_REPOSITORY=${3}
  BUILD_OPTIONS=${4}

  printInstallDependency "$NAME"

  DEPENDENCY_SOURCE_DIR=${LIB_DIR}$DIRECTORY/
  mkdir -p DEPENDENCY_SOURCE_DIR
  git clone --recursive "$GIT_REPOSITORY" "$DEPENDENCY_SOURCE_DIR"
  cd "$DEPENDENCY_SOURCE_DIR" || return
  cmake -S . -B "$CMAKE_BUILD_DIR" $BUILD_OPTIONS -DCMAKE_BUILD_TYPE="$BUILD_CONFIG" -DCMAKE_INSTALL_PREFIX="${VENDOR_DIR}$DIRECTORY/" -G "$CMAKE_GENERATOR"
  cmake --build "$CMAKE_BUILD_DIR" --config "$BUILD_CONFIG"
  cmake --install "$CMAKE_BUILD_DIR" --config "$BUILD_CONFIG"
  cd - || return
}
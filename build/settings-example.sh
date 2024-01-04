export LIB_DIR=c:/MyWorkRepository/doryn-lib/
export VENDOR_DIR=c:/MyWorkRepository/doryn-vendor/
export SYSTEM_BIN_DIR="C:/Program Files/"
export DEBUGGER_PATH="C:/msys64/mingw64/bin/gdb.exe"
export BUILD_CONFIG=Debug
export CMAKE_GENERATOR="Visual Studio 16 2019"
export CMAKE_BUILD_DIR="build/cmake-artifacts-vs"

function printInstallDependency()
{
    echo ""
    echo "====================================================Installing: ${1}"
    echo ""
}
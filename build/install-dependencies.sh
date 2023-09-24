source ./build/settings.sh

mkdir -p $VENDOR_DIR

#install Catch2 - unit testing framework
CATCH2_DIR=${LIB_DIR}Catch2/
mkdir -p $CATCH2_DIR
git clone https://github.com/catchorg/Catch2.git ${CATCH2_DIR}
cd $CATCH2_DIR
cmake -B build -H. -DBUILD_TESTING=OFF
cmake --build build/ --target install
cd -

#install FakeIt - Mock framework for unit tests
FAKEIT_DIR=${LIB_DIR}FakeIt/
mkdir -p $FAKEIT_DIR
git clone https://github.com/eranpeer/FakeIt.git ${FAKEIT_DIR}
cd $FAKEIT_DIR
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=${VENDOR_DIR}FakeIt/
cmake --build build
cmake --install build
cd -

#install GLFW - (Graphics Library Framework) utility framework for OpenGL
GLFW_DIR=${LIB_DIR}glfw/
mkdir -p $GLFW_DIR
git clone https://github.com/glfw/glfw.git $GLFW_DIR
cd $GLFW_DIR
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=${VENDOR_DIR}glfw/
cmake --build build
cmake --install build
cd -

#install gl3w - OpenGL functions binder library
GL3W_DIR=${LIB_DIR}gl3w/
mkdir -p $GL3W_DIR
git clone https://github.com/skaslev/gl3w.git $GL3W_DIR
cd $GL3W_DIR
python gl3w_gen.py
cd -
mkdir -p ${VENDOR_DIR}GL
cp ${GL3W_DIR}include/GL/gl3w.h ${VENDOR_DIR}GL/gl3w.h
cp ${GL3W_DIR}include/GL/glcorearb.h ${VENDOR_DIR}GL/glcorearb.h
cp ${GL3W_DIR}src/gl3w.c ${VENDOR_DIR}GL/gl3w.c
LIB_DIR=src/lib/
mkdir -p src/vendor

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
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=../../vendor/FakeIt/
cmake --build build
cmake --install build
cd -

#install GLFW - (Graphics Library Framework) utility framework for OpenGL
GLFW_DIR=${LIB_DIR}glfw
mkdir -p $GLFW_DIR
git clone https://github.com/glfw/glfw.git $GLFW_DIR
cd $GLFW_DIR
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=../../vendor/glfw/
cmake --build build
cmake --install build
cd -
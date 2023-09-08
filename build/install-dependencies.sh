LIB_DIR=src/lib/
mkdir -p src/vendor

#install Catch2
CATCH2_DIR=${LIB_DIR}Catch2/
mkdir -p $CATCH2_DIR
git clone https://github.com/catchorg/Catch2.git ${CATCH2_DIR}
cd $CATCH2_DIR
cmake -B build -H. -DBUILD_TESTING=OFF
cmake --build build/ --target install
cd -

#install FakeIt
FAKEIT_DIR=${LIB_DIR}FakeIt/
mkdir -p $FAKEIT_DIR
git clone https://github.com/eranpeer/FakeIt.git ${FAKEIT_DIR}
cd $FAKEIT_DIR
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=../../vendor/FakeIt/
cmake --build build
cmake --install build
cd -
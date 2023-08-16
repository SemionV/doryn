#install Catch2
CATCH2_DIR=src/lib/Catch2/
mkdir -p $CATCH2_DIR
cd $CATCH2_DIR
git clone https://github.com/catchorg/Catch2.git
cmake -B build -H. -DBUILD_TESTING=OFF
cmake --build build/ --target install
cd -

# doryn

* In order to debug a plugin, one can attach a debugger(gdb) and change current directory(pwd and cd commands) to the directory of the executable

To setup project localy:
* Choose lib and vendor folders outside the project tree, where the 3rd party dependencies will be downloaded, build and installed
* Make a copy of build/settings-example.sh file and set the proper values of your machine there
* Execute build/format-vscode-settings.sh script
* Execute build/install-dependencies.sh script
* Execute build/build-local-dev-cmake.sh script
* Enjoy!

**To run tests coverage:**
* Install lcov:
  * sudo apt-get -y install lcov
* cmake -B <binary_tree> -S <source_tree> -DCMAKE_BUILD_TYPE=Debug
* cmake --build <binary_tree> -t coverage-tests
* * go to <binary_tree>/coverage-tests/index.html 

**To run memory check with Valgrind:**
* Install valgrind and gawk:
    * sudo apt-get install valgrind
    * sudo apt-get install gawk
* cmake -B <binary_tree> -S <source_tree> -DCMAKE_BUILD_TYPE=Debug
* cmake --build <binary_tree> -t memcheck-tests
* go to <binary_tree>/memcheck-tests/index.html

**To use performance profiling tool:**
* sudo apt install linux-tools-$(uname -r)

**To use static analysis tool:**
* sudo apt-get install cppcheck
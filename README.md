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

**To profile with Tracy
* Build the engine and the game with -DDORY_PROFILING definition
* Clone tracy to a separate git repository
* Build and run tracy server:
  * Go to tracy folder
  * Use cmake to build and install the server:
    * cmake -B profiler/build -S profiler -DCMAKE_BUILD_TYPE=Release -Wno-dev
    * cmake --build profiler/build --config Release --parallel
    * cmake --install profiler/build --prefix profiler
    * cd profiler/bin
    * ./tracy-profiler
  * Run the game application
  * Connect tracy server to the game

**Build-time definitions:**
* ASSERT_ENABLED : on/off the asserts for production builds 
* DEBUG_ASSERT_ENABLED : on/off the asserts for in-house release builds
* IN_HOUSE_ASSERT_ENABLED : on/off the asserts for debug builds
* DORY_PROFILING : use embedded profiler client
* DORY_GPU_PROFILING : use embedded profiler client with GPU tracing
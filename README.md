# doryn

* In order to debug a plugin, one can attach a debugger(gdb) and change current directory(pwd and cd commands) to the directory of the executable

To setup project localy:
* Choose lib and vendor folders outside the project tree, where the 3rd party dependencies will be downloaded, build and installed
* Make a copy of build/settings-example.sh file and set the proper values of your machine there
* Execute build/format-vscode-settings.sh script
* Execute build/install-dependencies.sh script
* Execute build/build-local-dev-cmake.sh script
* Enjoy!
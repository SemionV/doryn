cd hello
g++ -c "**.cpp"
g++ -o "../bin/hello.exe" "**.o" -L. -l:"../mathLibrary/math_lib.a"
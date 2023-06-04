cd mathLibrary
g++ -c -DMATHLIBRARY_EXPORTS "**.cpp"
g++ -shared -o "../bin/math.dll" "**.o" -Wl,--out-implib,math_lib.a
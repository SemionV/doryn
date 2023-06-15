targetDir := bin/

buildDir := build/

build-hello: build-math
	sh $(buildDir)build-module.sh hello hello.exe $(targetDir) --macroses "TEST TEST2" --config "debug" --lib-dirs "lib/ ." --link-libs "lib/math_lib.a"

build-math:
	sh $(buildDir)build-module.sh mathLibrary math.dll $(targetDir) --macroses "MATHLIBRARY_EXPORTS" --config "debug" --out-lib "lib/math_lib.a" --module-type "shared"
binDir := bin/
mathLibraryDir := mathLibrary/
helloDir := hello/
projects := hello mathLibrary

helloBuildFiles := $(shell find $(helloDir) -name "hello*.cpp")

buildDir := build/

build-hello:
	sh $(buildDir)build-module.sh hello hello.exe bin/ --macroses "TEST TEST2" --config "debug" --lib-dirs "lib/ ." --link-libs "lib/math_lib.a"

build-math:
	sh $(buildDir)build-module.sh mathLibrary math.dll bin/ --macroses "MATHLIBRARY_EXPORTS" --config "debug" --out-lib "lib/math_lib.a" --module-type "shared"

# build-math:
# 	g++ -c -DMATHLIBRARY_EXPORTS "$(mathLibraryDir)**.cpp"
# 	g++ -shared -o "$(binDir)math.dll" "**.o" -Wl,--out-implib,math_lib.a

clean clean-math clean-hello:
	rm -f *.o

build: clean build-math clean-math build-hello clean-hello

print:
	echo $(helloBuildFiles)

target:
	for i in 1 2 3; do \
		echo $$i; \
	done;
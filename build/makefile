binDir := bin/
mathLibraryDir := mathLibrary/
helloDir := hello/
projects := hello mathLibrary

helloBuildFiles := $(shell find $(helloDir) -name "hello*.cpp")

build-hello-obj:
	mkdir $(helloDir)obj -p
	files=$(helloBuildFiles)
	echo "$$files"
	for i in $$files; do \
		echo $$i; \
		g++ -c $$i -o $(helloDir)obj/*.o; \
	done;

hello/helloworld.cpp hello/test/test.cpp hello/test/test2/test.cpp:
	mkdir obj
	g++ -c "$(@)" -o "obj/*.o"

$(binDir)hello.exe: hello/helloworld.cpp hello/test/test.cpp hello/test/test2/test.cpp
	g++ -o "$(binDir)hello.exe" "$(helloDir)obj/*.o" -L. -l:"math_lib.a"

build-hello-2: $(binDir)hello.exe

hello:
	echo "Hello, World!"
build-math:
	g++ -c -DMATHLIBRARY_EXPORTS "$(mathLibraryDir)**.cpp"
	g++ -shared -o "$(binDir)math.dll" "**.o" -Wl,--out-implib,math_lib.a

build-hello: build-math
	g++ -c "$(helloDir)**.cpp"
	g++ -o "$(binDir)hello.exe" "**.o" -L. -l:"math_lib.a"

clean clean-math clean-hello:
	rm -f *.o

build: clean build-math clean-math build-hello clean-hello

print:
	echo $(helloBuildFiles)

target:
	for i in 1 2 3; do \
		echo $$i; \
	done;
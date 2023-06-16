targetDir := bin/
buildDir := build/
libDirectories := lib/ .
srcDir := src/
ifndef config
override config := release
endif

build-project-hello: project := $(srcDir)hello
build-project-hello: outputFile := hello.exe
build-project-hello: macroses := TEST TEST2
build-project-hello: linkLibraries := --link-libs "lib/math_lib.a lib/message_lib.a"
build-hello: build-math build-message build-project-hello

build-project-math: project := $(srcDir)mathLibrary
build-project-math: outputFile := math.dll
build-project-math: macroses := MATHLIBRARY_EXPORTS
build-project-math: outLib := --out-lib "lib/math_lib.a"
build-project-math: moduleType := --module-type "shared"
build-math: build-project-math

build-project-message: project := $(srcDir)messageLibrary
build-project-message: outputFile := message.dll
build-project-message: macroses := MESSAGELIBRARY_EXPORTS
build-project-message: outLib := --out-lib "lib/message_lib.a"
build-project-message: moduleType := --module-type "shared"
build-message: build-project-message

build-project-hello build-project-math build-project-message:
	sh $(buildDir)build-module.sh $(project) "$(outputFile)" "$(targetDir)" $(moduleType) --macroses "$(macroses)" --config $(config) $(outLib) --lib-dirs "$(libDirectories)" $(linkLibraries)
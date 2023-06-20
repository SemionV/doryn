targetDir := bin/
buildDir := build/
libDirectories := lib/ .
buildTestSrcDir := src-build-test/
srcDir := src/
ifndef config
override config := release
endif
moduleExportMacros := DORY_EXPORT

build-project-test-app: project := $(srcDir)testApp
build-project-test-app: outputFile := testApp.exe
build-project-test-app: linkLibraries := --link-libs "lib/doryBase_lib.a lib/testDomain_lib.a"
build-test-app: build-base build-test-domain build-project-test-app

build-project-base: project := $(srcDir)base
build-project-base: outputFile := doryBase.dll
build-project-base: macroses := $(moduleExportMacros)
build-project-base: outLib := --out-lib "lib/doryBase_lib.a"
build-project-base: moduleType := --module-type "shared"
build-base: build-project-base

build-project-test-domain: project := $(srcDir)testDomain
build-project-test-domain: outputFile := testDomain.dll
build-project-test-domain: macroses := $(moduleExportMacros)
build-project-test-domain: outLib := --out-lib "lib/testDomain_lib.a"
build-project-test-domain: linkLibraries := --link-libs "lib/doryBase_lib.a"
build-project-test-domain: moduleType := --module-type "shared"
build-test-domain: build-project-test-domain

build-project-base build-project-test-domain build-project-test-app:
	sh $(buildDir)build-module.sh $(project) "$(outputFile)" "$(targetDir)" $(moduleType) --macroses "$(macroses)" --config $(config) $(outLib) --lib-dirs "$(libDirectories)" $(linkLibraries)
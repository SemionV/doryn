targetDir := bin/
buildDir := build/
libDirectories := lib/ .
buildTestSrcDir := src-build-test/
srcDir := src/
ifndef config
override config := release
endif
moduleExportMacros := DORY_EXPORT
usePrecompiledHeaders := --precompiled-headers true

projectBase := $(srcDir)base
projectTestDomain:= $(srcDir)testDomain
projectTestApp := $(srcDir)testApp

build-project-test-app: project := $(projectTestApp)
build-project-test-app: outputFile := testApp.exe
build-project-test-app: linkLibraries := --link-libs "lib/doryBase_lib.a lib/testDomain_lib.a"
build-project-test-app: dependencies := "$(projectBase) $(projectTestDomain)"
build-project-test-app: precompiledHeaders := $(usePrecompiledHeaders)
build-test-app: build-project-base build-project-test-domain build-project-test-app

build-project-base: project := $(projectBase)
build-project-base: outputFile := doryBase.dll
build-project-base: macroses := $(moduleExportMacros)
build-project-base: outLib := --out-lib "lib/doryBase_lib.a"
build-project-base: moduleType := --module-type "shared"
build-project-base: precompiledHeaders := $(usePrecompiledHeaders)
build-project-base: dependencies := ""
build-base: build-project-base

build-project-test-domain: project := $(projectTestDomain)
build-project-test-domain: outputFile := testDomain.dll
build-project-test-domain: macroses := $(moduleExportMacros)
build-project-test-domain: outLib := --out-lib "lib/testDomain_lib.a"
build-project-test-domain: linkLibraries := --link-libs "lib/doryBase_lib.a"
build-project-test-domain: moduleType := --module-type "shared"
build-project-test-domain: dependencies := "$(projectBase)"
build-project-test-domain: precompiledHeaders := $(usePrecompiledHeaders)
#build-project-test-domain: forceRecompileHeaders := --force-recompile-headers true
build-test-domain: build-project-test-domain

build-project-base build-project-test-domain build-project-test-app:
	sh $(buildDir)build-module.sh \
	$(project) \
	"$(outputFile)" \
	"$(targetDir)" \
	$(moduleType) \
	--macroses "$(macroses)" \
	--config $(config) \
	$(outLib) \
	--lib-dirs "$(libDirectories)" \
	$(linkLibraries) \
	$(precompiledHeaders) \
	$(forceRecompileHeaders) \
	--dependencies $(dependencies)
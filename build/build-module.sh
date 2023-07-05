project="${1}"
outputFile=${2}
targetDir=${3}

projectDir="${project}/"
buildDir="$(dirname "$0")/"
targetDir="${targetDir:-${projectDir}bin/}"
targetFile="${targetDir}${outputFile}"
moduleFileExtensions=("cpp")
headerFileExtensions=("h")
precompiledHeaderFileExtension="gch"

#read named arguments of the script
while [ $# -gt 0 ]; do
    if [[ $1 == "--macroses" ]]; then
        declare "macroses"="$2"
        shift
    elif
        [[ $1 == "--module-type" ]]; then
        declare "moduleType"="$2"
        shift
    elif
        [[ $1 == "--out-lib" ]]; then
        declare "outputImportLibrary"="$2"
        shift
    elif
        [[ $1 == "--config" ]]; then
        declare "config"="$2"
        shift
    elif
        [[ $1 == "--lib-dirs" ]]; then
        declare "libDirs"="$2"
        shift
    elif [[ $1 == "--link-libs" ]]; then
        declare "linkLibraries"="$2"
        shift
    elif [[ $1 == "--precompiled-headers" ]]; then
        declare "usePrecompiledHeaders"="$2"
        shift
    elif [[ $1 == "--force-recompile-headers" ]]; then
        declare "forceRecompileHeaders"="$2"
        shift
    elif [[ $1 == "--dependencies" ]]; then
        declare "dependencies"="$2"
        shift
    fi
    shift
done

config="${config:-release}"
objDir="${projectDir}obj-${config}/"

if [[ $config == "debug" ]]; then
    compilerGenerateDebugSymbols="-g"
fi

if [[ $moduleType == "shared" ]]; then
    linkerSharedModule="-shared" #build DLL, otherwise EXE
fi

if [[ ! -z "$outputImportLibrary" ]]; then
    linkerOutputImportLib="--out-implib,${outputImportLibrary}"
fi

if [[ ! -z "$dependencies" ]]; then
    dependencies=($dependencies)
else
    dependencies=()
fi

function areProjectHeadersModified()
{
    local dependencyProject="${1}";
    local headers=(`find ${dependencyProject} -name "*.${headerFileExtensions[0]}"`)
    local headersTouched="";

    for header in ${headers[*]}; do 
        if [ $header -nt "$targetFile" ]; then
            headersTouched="true"
            break
        fi
    done;

    echo $headersTouched;
}

function compileProject()
{
    #check if any header file in the project or dependency project was modified
    local dependenciesModified=""
    echo "Dependency projects: ${dependencies[@]}"
    for dependency in ${dependencies[*]}; do
        local isDependencyModified=$(areProjectHeadersModified "${dependency}")

        if [ ! -z $isDependencyModified ]; then
            echo "One of headers in dependency project ${dependency} was modified, it will trigger recompilation."
            dependenciesModified="true"
            break
        fi
    done;

    #check local project header files if they are modified
    local projectHeadersModified=$(areProjectHeadersModified "${projectDir}")
    local headersTouched=""

    if [ ! -z $projectHeadersModified ]; then
        echo "One of headers in project ${projectDir} was modified, it will trigger recompilation."
        headersTouched="true"
        break
    fi

    #build macro option for compiler
    local macroOptions=()
    for macro in ${macroses[*]}; do 
        macroOptions+=("-D${macro}")
    done;

    local modules=(`find ${projectDir} -name "*.${moduleFileExtensions[0]}"`)
    local externalHeaders=(`find ${projectDir} -name "*.ext.${headerFileExtensions[0]}"`)

    #compile headers
    if [ ! -z "${usePrecompiledHeaders}" ]; then
        for header in ${externalHeaders[*]}; do 
            local outputPchFile="${header}.${precompiledHeaderFileExtension}"

            if [ $header -nt "$outputPchFile" ] || [ ! -z "${forceRecompileHeaders}" ] || [ ! -z "${dependenciesModified}" ]; then
                echo "Compile: $header";
                headersTouched="true"
                g++ -Wall -fdiagnostics-color=always ${compilerGenerateDebugSymbols} -c "$header" -o $outputPchFile ${macroOptions[*]};
            fi
        done;
    fi

    objFiles=()
    mkdir "$objDir" -p

    echo "Compile modules with the following macroses defined: ${macroses[*]}"
    for module in ${modules[*]}; do 
        local regex="^${projectDir}(.*)\.${moduleFileExtensions[0]}" #regex to extract path to the module file relative to the project directory
        [[ $module =~ $regex ]] #execute regex
        local objFile=${BASH_REMATCH[1]} #get matched group
        objFile=${objFile////.} #replace / symbols with .
        objFile="${objDir}${objFile}.o" #combine path to object file

        objFiles+=($objFile) #collect names of all obj files in order to link them. this is a protection form linking deleted modules, as we do not clean up obj folder in order to save time for compilation

        if [ $module -nt "$objFile" ] || [ ! -z "${headersTouched}" ] || [ ! -z "${dependenciesModified}" ]; then
            echo "Compile: $module";
            g++ -H -Wall -fdiagnostics-color=always ${compilerGenerateDebugSymbols} -c "$module" -o "$objFile" ${macroOptions[*]};
        else
            echo "Skip: $module";
        fi
    done;
}

function linkProject()
{
    mkdir "$targetDir" -p

    local linkLibrariesOptions=()
    for lib in ${linkLibraries[*]}; do 
        linkLibrariesOptions+=("-l:${lib}")
    done;

    local libDirectoriesOptions=()
    for libDir in ${libDirs[*]}; do 
        libDirectoriesOptions+=("-L${libDir}")
    done;

    if [[ ! -z "$linkerOutputImportLib" ]]; then
        linkerOptions="-Wl,${linkerOutputImportLib}"
    fi

    g++ ${linkerSharedModule} -o "${targetFile}" "${objFiles[@]}" ${libDirectoriesOptions[*]} ${linkerOptions} ${linkLibrariesOptions[*]}
}

echo "Compile project: ${project}. Using config: ${config}"
compileProject

echo "Link project: ${project}"
linkProject
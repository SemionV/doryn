project="${1}"
outputFile=${2}
targetDir=${3}

projectDir="${project}/"
buildDir="$(dirname "$0")/"
targetDir="${targetDir:-${projectDir}bin/}"
moduleFileExtensions=("cpp")

#read named arguments of the script
while [ $# -gt 0 ]; do
    if [[ $1 == "--macroses" ]]; then
        declare "macroses"="$2"
        shift
    elif
        [[ $1 == "--linker-mode" ]]; then
        declare "linkerMode"="$2"
        shift
    elif
        [[ $1 == "--config" ]]; then
        declare "config"="$2"
        shift
    fi
    shift
done

config="${config:-release}"
objDir="${projectDir}obj-${config}/"

#validate script arguments
#
# TODO
#

function compileProject()
{
    local macroOptions=()
    for macro in ${macroses[*]}; do 
        macroOptions+=("-D${macro}")
    done;

    local modules=(`find ${projectDir} -name "*.${moduleFileExtensions[0]}"`)

    mkdir "$objDir" -p
    #rm -f ${objDir}*

    objFiles=()

    echo "Compile with the following macroses defined: ${macroses[*]}"
    for module in ${modules[*]}; do 
        local regex="^${projectDir}(.*)\.${moduleFileExtensions[0]}" #regex to extract path to the module file relative to the project directory
        [[ $module =~ $regex ]] #execute regex
        local objFile=${BASH_REMATCH[1]} #get matched group
        objFile=${objFile////.} #replace / symbols with .
        objFile="${objDir}${objFile}.o" #combine path to object file

        objFiles+=($objFile) #collect names of all obj files in order to link them. this is a protection form linking deleted modules, as we do not clean up obj folder in order to save time for compilation

        if [[ $config == "debug" ]]; then
            compilerOptions="-g"
        fi

        if [ $module -nt $objFile ]; then
            echo "Compile: $module";
            g++ ${compilerOptions} -c $module -o $objFile ${macroOptions[*]};
        else
            echo "Skip: $module";
        fi
    done;
}

echo "Compile project: ${project}. Using config: ${config}"
compileProject

echo "Link project: ${project}"
mkdir "$targetDir" -p
g++ -o "${targetDir}${outputFile}" "${objFiles[@]}" -L. -l:"math_lib.a"
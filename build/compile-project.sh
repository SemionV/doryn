project="${1}"
projectDir="${project}/"
outputFile=${2}
binDir=${3}
objDir="${projectDir}obj/"
modules=(`find ${projectDir} -name "*.cpp"`)

while [ $# -gt 0 ]; do
    if [[ $1 == "--macroses" ]]; then
        declare "macroses"="$2"
        shift
    fi
    shift
done


macroOptions=()
for macro in ${macroses[*]}; do 
    macroOptions+=("-D${macro}")
done;

#compilation
mkdir "$objDir" -p
echo "Modules to compile: ${modules[*]}"
echo "Compile with the following macroses defined: ${macroses[*]}"
for module in ${modules[*]}; do 
    fileName=$(basename -- $module)
    fileNameWithoutExtension=${fileName%.*}
    regex="^${projectDir}(.*)\.cpp"
    [[ $module =~ $regex ]]
    objFile=${BASH_REMATCH[1]}
    objFile=${objFile////.}
    objFile="${objDir}${objFile}.o"

    if [ $module -nt $objFile ]; then
        echo "Compile: $module";
        g++ -c $module -o $objFile ${macroOptions[*]};
    else
        echo "Skip: $module";
    fi
done;
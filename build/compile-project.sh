project="${1}"
projectDir="${project}/"
outputFile=${2}
binDir=${3}
objDir="${projectDir}obj/"
modules=(`find ${projectDir} -name "*.cpp"`)

macroses=($4)
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
    objFile="${objDir}${fileNameWithoutExtension}.o"

    if [ $module -nt $objFile ]; then
        echo "Compile: $module";
        g++ -c $module -o $objFile ${macroOptions[*]};
    else
        echo "Skip: $module";
    fi
done;
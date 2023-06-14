binDir=bin/
project="${1}"
projectDir="${project}/"
outputFile=${2}
objDir="${projectDir}obj/"
sourceFiles=(`find ${projectDir} -name "*.cpp"`)

echo "Build project: ${project}"

#compilation
mkdir "$objDir" -p
files=${sourceFiles[*]}
echo "files to compile: ${files}"
for i in ${files}; do \
    echo "compiling: $i"; \
    fileName=$(basename -- $i)
    fileNameWithoutExtension=${fileName%.*}
    g++ -c $i -o "${objDir}${fileNameWithoutExtension}.o"; \
done;

#linking
g++ -o "${binDir}${outputFile}" "${objDir}**.o" -L. -l:"math_lib.a"
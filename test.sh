binDir=bin/
helloDir=hello/
objDir="${helloDir}obj/"
helloBuildFiles=(`find ${helloDir} -name "*.cpp"`)
echo ${helloBuildFiles[*]}

#compilation
mkdir "$objDir" -p
files=${helloBuildFiles[*]}
echo "files to compile: ${files}"
for i in ${files}; do \
    echo "compiling: $i"; \
    fileName=$(basename -- $i)
    fileNameWithoutExtension=${fileName%.*}
    g++ -c $i -o "${objDir}${fileNameWithoutExtension}.o"; \
done;

#linking
g++ -o "${binDir}hello.exe" "${objDir}**.o" -L. -l:"math_lib.a"
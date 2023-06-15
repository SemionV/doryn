project="${1}"
projectDir="${project}/"
outputFile=${2}
binDir=${3}
objDir="${projectDir}obj/"
buildDir="$(dirname "$0")/"

echo "Compile project: ${project}"
sh ${buildDir}compile-project.sh $project $outputFile $binDir --macroses "TEST TEST2"

echo "Link project: ${project}"
g++ -o "${binDir}${outputFile}" "${objDir}**.o" -L. -l:"math_lib.a"
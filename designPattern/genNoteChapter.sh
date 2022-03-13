#!/bin/sh

if [ -z $1 ]; then
    echo "specify patternName to replace in first arg"
    exit
fi

if [ -z $2 ]; then
    echo "specify chapter in second arg"
    exit
fi

patternName=$1
chapter=$2

# upper case first char for $patternName
PatternName=$(echo $patternName | python3 -c "import sys; print(sys.stdin.read().title().strip())")
folder="$chapter.$patternName"

cp -r _note_template $folder
cd $folder

# macOs syntax for sed ... (-i needs extension)
find . -type f -exec sed -i  '' -e "s/placeholder/$patternName/g" {} +
find . -type f -exec sed -i '' -e "s/Placeholder/$PatternName/g" {} +
for f in $(ls .);do mv $f ${f/placeholder/$patternName}; done

echo "Adding these to ../CMakeList.txt"
echo "\tadd_subdirectory(designPattern/$folder $PatternName)"
echo "\tAPPEND EXTRA_LIBS(... $PatternName)"
echo "\nAdding these to ../main.cpp"
echo "\t#include "designPattern/$folder/${patternName}UseCase.h""
echo "\nAppend designPattern/README.md"
echo "\t[$chapter. $PatternName]($folder)"




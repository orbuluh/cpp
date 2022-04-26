#!/bin/sh
bld_folder=build
bin=$bld_folder/playground
cur_dir=$(pwd)
if [ ! -d "$bld_folder" ]; then
    mkdir $bld_folder
    cd build
    cmake ..
fi
rm $bin
cd "$cur_dir"
cmake --build build
echo "================="
$bin
#!/bin/sh
bld_folder=build
cur_dir=$(pwd)
if [ ! -d "$bld_folder" ]; then
    mkdir $bld_folder
    cd build
    cmake ..
fi
cd "$cur_dir"
cmake --build build
echo "---------------------------"
./build/playground
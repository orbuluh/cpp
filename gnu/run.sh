#!/bin/sh
bld_folder=build
cur_dir=$(pwd)
echo $cur_dir
if [ ! -d "$bld_folder" ]; then
    mkdir $bld_folder
    cd build
    cmake ..
fi
cd "$cur_dir"
cmake --build build
echo "--------------------\n"
./build/gnuPlayground
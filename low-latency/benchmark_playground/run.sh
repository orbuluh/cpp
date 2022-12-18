#!/bin/sh
bld_folder=build
cur_dir=$(pwd)
if [ ! -d "$bld_folder" ]; then
    mkdir $bld_folder
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
fi
cd "$cur_dir"
cmake --build build
echo "execute...\n"
./build/playground
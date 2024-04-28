#!/bin/sh
bld_folder=build
cur_dir=$(pwd)
if [ ! -d "$bld_folder" ]; then
    mkdir $bld_folder
    cd $bld_folder
    cmake -DCMAKE_BUILD_TYPE=Release ..
fi
cd "$cur_dir/$bld_folder"
pwd
cmake --build .
echo "\n\n"
echo "binary @ build/release/[proj_name]"
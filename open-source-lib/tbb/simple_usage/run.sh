#!/bin/sh
bld_folder=build
bin=$bld_folder/tbb_hello
cur_dir=$(pwd)
if [ ! -d "$bld_folder" ]; then
    mkdir $bld_folder
    cd $bld_folder
    cmake ..
fi
rm $bin
cd "$cur_dir"
cmake --build $bld_folder
echo "================="
$bin
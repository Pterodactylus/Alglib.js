#!/bin/bash
cwd=$(pwd)
bdir=$(pwd)/build

echo $bdir
#rm -rf $bdir
#mkdir $bdir

# Get the emsdk repo
#git clone https://github.com/emscripten-core/emsdk.git $bdir/emsdk --branch 2.0.13

# Enter that directory
#cd $bdir/emsdk

# Fetch the latest version of the emsdk (not needed the first time you clone)
#git pull

# Download and install the latest SDK tools.
#$bdir/emsdk/upstream/emscripten/em++ -v || ./emsdk install latest

# Make the "latest" SDK "active" for the current user. (writes ~/.emscripten file)
#./emsdk activate latest

# Activate PATH and other environment variables in the current terminal
#source ./emsdk_env.sh

#read -p "Press any key to continue ..."

#rm -rf $bdir/alglib
#mkdir $bdir/alglib
#cd $bdir/alglib
#cmake $cwd/src/ALGLIB/cpp/src
#make VERBOSE=1

rm -rf $bdir/alglib_helper
mkdir $bdir/alglib_helper
cd $bdir/alglib_helper
cmake $cwd/
make VERBOSE=1

set -x
$cwd/build/alglib_helper/alglibjsmain



#!/bin/bash

CMAKE_EXTRA_ARGUMENTS=""
CMAKE_EXTRA_ARGUMENTS="$CMAKE_EXTRA_ARGUMENTS -DCMAKE_BUILD_TYPE=Release"
	
echo -e "Building cross-compiled solution...$CMAKE_EXTRA_ARGUMENTS\n"
CMAKE_EXTRA_ARGUMENTS="$CMAKE_EXTRA_ARGUMENTS -DCMAKE_TOOLCHAIN_FILE=../cmake/win64.cmake"

mkdir -p build && cd build
if [ ! -f Makefile ]; then
	cmake $CMAKE_EXTRA_ARGUMENTS ..
fi

make -j 4
cd ..

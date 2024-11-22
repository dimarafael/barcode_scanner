#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ ! -d $DIR/build ]; then
	mkdir $DIR/build
fi
cd $DIR/build

cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-rpi4.cmake ..

make VERBOSE=1

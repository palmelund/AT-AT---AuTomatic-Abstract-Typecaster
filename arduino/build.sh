#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd $DIR

mkdir -p build
mkdir -p build/src
cd build/src
cp -a $DIR/defines.h .
cp -a $DIR/src/* .
cp -a $DIR/api/* .
cp -a $DIR/lib/SparkFun_ISL29125_Breakout_Arduino_Library/src/* .

if [ "$1" == 'v' ];
then
	arduino --verify src.ino
else
	arduino --upload src.ino
fi

cd ../..
rm -rf build

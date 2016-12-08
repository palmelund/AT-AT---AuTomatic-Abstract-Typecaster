#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd $DIR

mkdir -p build
mkdir -p build/unit_test
cd build/unit_test
cp -a $DIR/{*.h,*.ino} .
cp -a $DIR/../../arduino/defines.h .
cp -a $DIR/../../arduino/src/* .
cp -a $DIR/../../arduino/api/* .
cp -a $DIR/../../arduino/lib/SparkFun_ISL29125_Breakout_Arduino_Library/src/* .
rm -rf src.ino

if [ "$1" == 'v' ];
then
	arduino --verify unit_test.ino
else
	arduino --upload unit_test.ino
fi

cd ../..
rm -rf build

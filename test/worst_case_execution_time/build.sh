#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd $DIR

mkdir -p build
mkdir -p build/worst_case_execution_time
cd build/worst_case_execution_time
cp -a $DIR/{*.cpp,*.h,*.ino} .
cp -a $DIR/../../arduino/defines.h .
cp -a $DIR/../../arduino/src/* .
cp -a $DIR/../../arduino/api/* .
cp -a $DIR/../../arduino/lib/SparkFun_ISL29125_Breakout_Arduino_Library/src/* .
rm -rf src.ino

if [ "$1" == 'v' ];
then
	arduino --verify worst_case_execution_time.ino
else
	arduino --upload worst_case_execution_time.ino
fi

cd ../..
rm -rf build

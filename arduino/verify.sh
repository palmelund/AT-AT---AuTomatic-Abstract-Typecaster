#!/bin/bash

mkdir -p build
mkdir -p build/src
cd build/src
cp -a ../../defines.h .
cp -a ../../src/{src.ino,src.h} .
cp -a ../../api/* .
cp -a ../../lib/SparkFun_ISL29125_Breakout_Arduino_Library/src/* .

#Arduino is the dir in which the arduino binary is located
../../../../arduino/arduino --verify src.ino

cd ../..
rm -rf build

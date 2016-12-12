New-Item -ItemType Directory -Force .\build\color_test
cd build\color_test

Copy-Item $PSScriptRoot\..\..\arduino\api\* .
Copy-Item $PSScriptRoot\..\..\arduino\src\* .
Copy-Item $PSScriptRoot\..\..\arduino\defines.h
Copy-Item $PSScriptRoot\..\..\arduino\lib\SparkFun_ISL29125_Breakout_Arduino_Library\src\* .

Copy-Item $PSScriptRoot\color_test.ino

Remove-Item src.ino

# $PSScriptRoot
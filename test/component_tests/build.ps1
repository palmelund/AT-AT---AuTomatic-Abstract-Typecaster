New-Item -ItemType Directory -Force .\build\component_tests
cd build\component_tests

Copy-Item $PSScriptRoot\..\..\arduino\api\* .
Copy-Item $PSScriptRoot\..\..\arduino\src\* .
Copy-Item $PSScriptRoot\..\..\arduino\defines.h
Copy-Item $PSScriptRoot\..\..\arduino\lib\SparkFun_ISL29125_Breakout_Arduino_Library\src\* .

Copy-Item $PSScriptRoot\component_tests.ino
Copy-Item $PSScriptRoot\component_tests.h
Copy-Item $PSScriptRoot\component_tests.cpp

Remove-Item src.ino

# $PSScriptRoot
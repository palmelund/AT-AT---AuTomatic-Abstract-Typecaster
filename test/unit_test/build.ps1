New-Item -ItemType Directory -Force .\build\unit_test
cd build\unit_test

Copy-Item $PSScriptRoot\..\..\arduino\api\* .
Copy-Item $PSScriptRoot\..\..\arduino\src\* .
Copy-Item $PSScriptRoot\..\..\arduino\defines.h .
Copy-Item $PSScriptRoot\..\..\arduino\lib\SparkFun_ISL29125_Breakout_Arduino_Library\src\* .
Copy-Item $PSScriptRoot\unit_test.ino .
Copy-Item $PSScriptRoot\unit_test.h .

Remove-Item src.ino

# $PSScriptRoot
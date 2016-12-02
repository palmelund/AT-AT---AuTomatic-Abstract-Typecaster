New-Item -ItemType Directory -Force .\build\src
cd build\src

Copy-Item $PSScriptRoot\api\* .
Copy-Item $PSScriptRoot\src\*
Copy-Item $PSScriptRoot\defines.h
Copy-Item $PSScriptRoot\lib\SparkFun_ISL29125_Breakout_Arduino_Library\src\*



# $PSScriptRoot
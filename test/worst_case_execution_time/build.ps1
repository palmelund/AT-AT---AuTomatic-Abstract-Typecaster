New-Item -ItemType Directory -Force .\build\worst_case_execution_time
cd build\worst_case_execution_time

Copy-Item $PSScriptRoot\..\..\arduino\api\* .
Copy-Item $PSScriptRoot\..\..\arduino\src\*
Copy-Item $PSScriptRoot\..\..\arduino\defines.h
Copy-Item $PSScriptRoot\..\..\arduino\lib\SparkFun_ISL29125_Breakout_Arduino_Library\src\*
Copy-Item $PSScriptRoot\worst_case_execution_time.cpp
Copy-Item $PSScriptRoot\worst_case_execution_time.h
Copy-Item $PSScriptRoot\worst_case_execution_time.ino
Remove-Item src.ino

# $PSScriptRoot
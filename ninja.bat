@echo off
setlocal

REM Configure
cmake -S . ^
 -B out\build\x86-Debug ^
 -G Ninja ^
 -DCMAKE_BUILD_TYPE=Debug ^
 -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=out\bin\x86-Debug ^
 -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=out\bin\x86-Debug

REM Build
cmake --build out\build\x86-Debug
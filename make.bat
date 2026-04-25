@echo off
setlocal

cmake -S . -B out\build\nmake-matching -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo

REM nmake is kinda dumb and sometimes needs 2 runs
cmake --build out\build\nmake-matching --config Release && cmake --build out\build\nmake-matching --config Release

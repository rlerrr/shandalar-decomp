@echo off
setlocal

cmake -S . -B out\build\nmake-matching -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo

cmake --build out\build\nmake-matching --config Release

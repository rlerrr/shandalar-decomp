@echo off
setlocal

cmake -S . -B out\build\nmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DSHANDALAR_MODERN_FIXES=ON -DCMAKE_INSTALL_PREFIX=C:\Magic\Program

cmake --build out\build\nmake

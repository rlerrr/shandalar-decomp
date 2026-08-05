setlocal

call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64
if errorlevel 1 exit /b %errorlevel%

set "PATH=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;%PATH%"
set "LLVM_LINK=C:/Program Files (x86)/LLVM/bin/lld-link.exe"
set "WINDOWS_RC=C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x86/rc.exe"

cmake -S . ^
    -B out\build\ninja-clangcl-modern-cl ^
    -G Ninja ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DSHANDALAR_MODERN_FIXES=ON ^
    -DCMAKE_INSTALL_PREFIX="C:/Magic-Copy/Program" ^
    -DCMAKE_C_COMPILER="C:/Program Files (x86)/LLVM/bin/clang-cl.exe" ^
    -DCMAKE_CXX_COMPILER="C:/Program Files (x86)/LLVM/bin/clang-cl.exe" ^
    -DCMAKE_LINKER="%LLVM_LINK%" ^
    -DCMAKE_RC_COMPILER="%WINDOWS_RC%" ^
    -DCMAKE_C_FLAGS="-fuse-ld=lld" ^
    -DCMAKE_CXX_FLAGS="-fuse-ld=lld" ^
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=out/bin/ninja-clangcl-modern-cl ^
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=out/bin/ninja-clangcl-modern-cl

cmake --build out\build\ninja-clangcl-modern-cl

REM cmake --install out\build\ninja-clangcl-modern-cl
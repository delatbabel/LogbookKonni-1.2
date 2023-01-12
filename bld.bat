REM cd build
REM cmake -T v141_xp -G "Visual Studio 15 2017" ..
REM cmake --build . --target tarball --config RelWithDebInfo >output.txt

REM .\buildwin\win_deps.bat
cd build
cmake -T v141_xp -G "Visual Studio 15 2017" ^
       -DCMAKE_BUILD_TYPE=RelWithDebInfo  ..
cmake --build . --target tarball --config RelWithDebInfo >output.txt
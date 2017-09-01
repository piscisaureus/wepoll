@echo off
rd /s/q CMakeFiles
del CMakeCache.txt *.cmake ALL_BUILD*.* ZERO_CHECK*.*
cmake.exe -G "Visual Studio 14 2015 Win64" %*

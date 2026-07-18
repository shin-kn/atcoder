@echo off
setlocal enabledelayedexpansion

g++ -Wall -Wl,--stack,1073741824 -std=c++23 main.cpp -o main


if !errorlevel! neq 0 (
	exit /b
)
type test.txt

echo;
echo;


main.exe < test.txt

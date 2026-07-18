@echo off
set PATH=%PATH%;C:\mingw64\bin
setlocal enabledelayedexpansion
type nul > .\test.txt
for /f "delims=" %%A in ('powershell get-clipboard') do echo %%A >> .\test.txt



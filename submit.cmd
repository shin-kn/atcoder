@echo off

setlocal enabledelayedexpansion

uv --directory ./python run include.py ../main.cpp | clip


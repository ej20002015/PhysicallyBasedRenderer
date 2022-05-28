@echo off
REM Set working directory up a level
pushd %~dp0\..\
call Premake\bin\premake5.exe vs2019
popd
REM Give the user time to see the output of premake
PAUSE
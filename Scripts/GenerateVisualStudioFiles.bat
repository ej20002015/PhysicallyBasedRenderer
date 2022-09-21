@echo off
REM Set the root directory of the project as the current working directory
pushd %~dp0\..\
call Premake\bin\Windows\premake5.exe vs2019
popd
REM Give the user time to see the output of premake
PAUSE
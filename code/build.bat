@echo off

if not exist ..\build mkdir ..\build

pushd ..\build

cl -nologo -FC -Z7 -Oi -Od ..\code\win32_layer.c -link user32.lib

popd

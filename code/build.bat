@echo off

if not exist ..\build mkdir ..\build

pushd ..\build

cl -nologo -FC -Z7 -Oi -Od -W4 -WX -wd4100 ..\code\win32_layer.c -link user32.lib

popd

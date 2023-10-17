@echo off

if not exist jni\nul mkdir jni
copy Android.mk jni /y
copy Application.mk jni /y
cd jni

if exist src\nul goto remove_src
if not exist src goto make_src
del src /q
goto make_src
:remove_src
rmdir src /q
:make_src
mklink /d src "%cd%\..\..\..\src"

call ndk-build.cmd clean
call ndk-build.cmd

cd ..

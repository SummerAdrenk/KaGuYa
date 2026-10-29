@echo off
setlocal
if "%~2"=="" exit /b 2
call "%~1\VC\Auxiliary\Build\vcvarsall.bat" x86 -vcvars_ver=%~2 >nul
if errorlevel 1 exit /b 1
set INCLUDE
set LIB
set PATH
set VCToolsInstallDir
set WindowsSdkDir
set WindowsSDKVersion
exit /b 0

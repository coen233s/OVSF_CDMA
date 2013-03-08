@echo off
setlocal
goto BEGIN
:USAGE
echo ********************************** Usage **********************************
echo This batch script beautifies one or more (C/C++/XML,...) source files
echo From Windows Explorer, drag one or more files to %0
echo ---------------------------------------------------------------------------
echo Command Line Syntax: %0 file1 {file2} {file3} {...}
echo ********************************** Usage **********************************
goto FINISH

:BEGIN
if not exist C:\Windows\uncrustify.exe (goto NEED_UNCRUSTIFY)

REM Expect at least one file name as parameter
if "%1" == "" (goto USAGE)

echo ---------------------------------------------------------------------------
echo TO ABORT: Close this console window.
echo.
echo You are about to beautify the following file(s) by replacing the original
echo ---------------------------------------------------------------------------
for %%i in (%*) do (
    echo %%i
)
echo ---------------------------------------------------------------------------
echo.
echo TO ABORT: Close this console window.
echo.
echo ===========================================================================
echo If you are absolutely certain to perform this operation, then
pause

uncrustify --replace -c %~dp0\beautyc.ini %*

goto FINISH
:NEED_UNCRUSTIFY
echo ********************************** ERROR **********************************
echo C:\Windows\uncrustify.exe is missing
echo Download 0.57 or newer from http://uncrustify.sourceforge.net/
echo ********************************** ERROR **********************************

:FINISH
endlocal
pause
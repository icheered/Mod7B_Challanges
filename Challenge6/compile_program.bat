:: This file should be placed at the top level of your project
:: i.e. it should be in the same folder as the "src" folder (containing .cpp files)
:: and the "include" folder (containing .h files).

@echo Compiling your program...

@echo off
setlocal ENABLEDELAYEDEXPANSION
set cppfiles=
for /f %%i in ('dir *.cpp /b /a-d') do (
    set cppfiles=!cppfiles! "%%i"
)
echo on

if not exist obj\NUL mkdir obj

:: `cl` is the MSVC compiler

cl %cppfiles% /W3 /EHsc /std:c++latest /I client /I protocol /Fo.\obj\ /link Ws2_32.lib /SUBSYSTEM:CONSOLE /out:program.exe
if %errorlevel% neq 0 exit /b %errorlevel%


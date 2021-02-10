:: This file should be placed at the top level of your project
:: i.e. it should be in the same folder as the "framework" and "my_protocol" folders (containing .cpp and .h files).

@echo Compiling your reliable data transfer challenge client...

@echo off
setlocal ENABLEDELAYEDEXPANSION
set cppfiles=
for /f %%i in ('dir framework\*.cpp /b /a-d') do (
    set cppfiles=!cppfiles! "framework\%%i"
)
for /f %%i in ('dir my_protocol\*.cpp /b /a-d') do (
    set cppfiles=!cppfiles! "my_protocol\%%i"
)
echo on

if not exist obj\NUL mkdir obj

:: `cl` is the MSVC compiler

cl %cppfiles% /W3 /EHsc /std:c++latest /I framework /I my_protocol /Fo.\obj\ /link Ws2_32.lib /SUBSYSTEM:CONSOLE /out:My_ReliableDataTransferCC.exe
if %errorlevel% neq 0 exit /b %errorlevel%


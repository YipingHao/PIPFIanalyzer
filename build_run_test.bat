@echo off
echo Building main with MSVC (incremental build)...
if not exist obj mkdir obj
nmake /nologo /f tools\Makefile.msvc
if %errorlevel% neq 0 (
    echo.
    echo Build failed! Make sure you are running this from the Developer Command Prompt for VS.
    exit /b %errorlevel%
)
echo Build succeeded! main.exe is in the test\ folder.
echo.
echo Running test with cutoff parameter...
test\main.exe parameter\input.txt .\output\

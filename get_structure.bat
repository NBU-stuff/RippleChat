@echo off
set OUTPUT=project_structure.txt

rem Clear the output file if it exists
if exist %OUTPUT% del %OUTPUT%

rem Loop through all files but exclude 'out' directory
for /r %%i in (*) do (
    echo %%i | findstr /V /I /C:"\out\" >> %OUTPUT%
)

echo Project structure saved to %OUTPUT%

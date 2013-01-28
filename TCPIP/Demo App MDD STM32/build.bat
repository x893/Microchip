@echo off
if "%1*"=="clean*" goto :clean
if "%1*"=="build*" goto :build
call :clean

:build
call "Demo App MDD.BAT" 2>&1 | %Windir%\System32\find.exe /i " Error:  " >errors.txt
for /F "tokens=1-15 delims=,: " %%l in (errors.txt) do echo %%~nxl(%%n,1): error %%p: %%q %%r %%s %%t %%u %%v %%w %%x %%y %%z
goto :EOF

:clean
del build\*.o
del build\*.crf
del build\*.d
goto :EOF

@ECHO OFF

IF "%1" == "/?" (
    ECHO Usage: tsmerge lang outputFileName sourcePath  
    EXIT /B 0;
)

SET lang=%1
IF _%1_ == __ (
    SET lang=ru
)

SET out=%2
IF _%2_ == __ (
    SET out=output
)

SET source=%3
IF _%3_ == __ (
    SET source=.
)

SET args=
FOR /F "delims==" %%d in ('FORFILES /S /P %source% /M *_%lang%.ts /C "cmd /c @echo @path"') DO (
    CALL :concat %%d
)

IF /I "%QTDIR%" == "" (
    ECHO ERROR: QTDIR variable not set!
    EXIT /B 1;
)

echo %QTDIR%\bin\lconvert.exe %args% -o %out%_%lang%.qm
%QTDIR%\bin\lconvert.exe %args% -o %out%_%lang%.qm

GOTO :EOF

:concat
SET args=%args% %1
GOTO :EOF

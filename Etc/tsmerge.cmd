@ECHO OFF
REM /****************************************************************************
REM ** This file is a part of Syncopate Limited GameNet Application or it parts.
REM **
REM ** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates. 
REM ** All rights reserved.
REM **
REM ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
REM ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
REM ****************************************************************************/

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

%QTDIR%\bin\lconvert.exe %args% -o %out%_%lang%.qm
GOTO :EOF

:concat
SET args=%args% %1
GOTO :EOF

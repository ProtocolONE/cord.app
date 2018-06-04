SET lupdate=%QTDIR%\bin\lupdate.exe
SET lconvert=%QTDIR%\bin\lconvert.exe
SET UpdateConfig= -no-obsolete -locations absolute
Set TsTool=..\etc\QtTsTool.exe


call :create_i18n qGNA
call :create_i18n Host
call :create_i18n Features

goto end

:create_i18n
set ProjectName=%1
echo Generate i18n for %ProjectName%

pushd %ProjectName%

mkdir i18n

%lupdate% %UpdateConfig% . -ts ./i18n/%ProjectName%_en.ts
%lupdate% %UpdateConfig% . -ts ./i18n/%ProjectName%_ru.ts

%TsTool% sort ./i18n/%ProjectName%_en.ts
%TsTool% sort ./i18n/%ProjectName%_ru.ts

%TsTool% fixKeys ./i18n/%ProjectName%_ru.ts

%lconvert% ./i18n/%ProjectName%_ru.ts -o ./i18n/%ProjectName%_ru.qm
%lconvert% ./i18n/%ProjectName%_en.ts -o ./i18n/%ProjectName%_en.qm

popd

echo Finish %ProjectName%
@goto :EOF

:end
exit /B 0

Set BuildFolder=%~1
echo %BuildFolder%

Set PATH=%PATH%;%~2

Set OutConfig="%BuildFolder%\Thetta\description.dat"
Set Hasher="%QGNACOMMONDIR%\GGS\SectionHasher\trunk\bin\SectionHasher.exe"

:: qGna 
:: We are not processing this file because of vmprot
:: Set qgnapath="%BuildFolder%\qGNA.exe"
:: Set qgnapathout="%BuildFolder%\qgna.json"

:: GameDownloaderx86d.dll

Set gamedownloader="%BuildFolder%GameDownloaderx86.dll"
Set gamedownloaderout="%BuildFolder%\gamedownloader.json"

:: GameExecutorX86d.dll
:: We are not processing this file because of vmprot
:: Set gameexecutor="%BuildFolder%\GameExecutorX86.dll"
:: Set gameexecutorout="%BuildFolder%\gameexecutor.json"

:: CoreX86d.dll

Set corex="%BuildFolder%\CoreX86.dll"
Set corexout="%BuildFolder%\core.json"

:: UpdateSystemX86d.dll

Set updatesystem="%BuildFolder%\UpdateSystemX86.dll"
Set updatesystemout="%BuildFolder%\updatesystem.json"

:: Qt5Networkd.dll

Set qtnetwork="%BuildFolder%\Qt5Network.dll"
Set qtnetworkout="%BuildFolder%\qt5network.json"

:: libeay32.dll

Set libeay="%BuildFolder%\libeay32.dll"
Set libeayout="%BuildFolder%\libeay32.json"

:: ssleay32.dll

Set ssleay="%BuildFolder%\ssleay32.dll"
Set ssleayout="%BuildFolder%\ssleay32.json"


del %OutConfig%

:: Generating jsons

call %Hasher% -c=%qgnapath% -o=%qgnapathout%

call %Hasher% -c=%gamedownloader% -o=%gamedownloaderout%
call %Hasher% -c=%gameexecutor% -o=%gameexecutorout%
call %Hasher% -c=%corex% -o=%corexout%
call %Hasher% -c=%updatesystem% -o=%updatesystemout%

:: call SectionHasher.exe -c=Qt5Cored.dll -o=qt5core.json

call %Hasher% -c=%qtnetwork% -o=%qtnetworkout%

call %Hasher% -c=%libeay% -o=%libeayout%
call %Hasher% -c=%ssleay% -o=%ssleayout%

:: Merge all configs
:: Without qtcore, qgna, executor
call %Hasher% -m=%gamedownloaderout%,%corexout%,%updatesystemout%,%qtnetworkout%,%libeayout%,%ssleayout% -o=%OutConfig%

:: Delete all files

:: del %qgnapathout%
del %gamedownloaderout%
:: del %gameexecutorout%
del %corexout%
del %updatesystemout%
del %qtnetworkout%
del %libeayout%
del %ssleayout%

if NOT EXIST %OutConfig% (
 exit /B -1
)


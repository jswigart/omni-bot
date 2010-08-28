echo off
echo Preparing Omni-bot release.
echo This should be done AFTER the windows and linux release builds have been done.
pause

echo -------------------------
echo Copying Windows Dlls
echo -------------------------

echo Copying omnibot_et.dll
xcopy /e /f /y "..\..\..\head\Omnibot\projects\msvc80\build\MOD_ET\Release\omnibot_et.dll" ".\"
if errorlevel 1 goto Finished_Errors

echo Copying ET server file
xcopy /e /f /y "..\..\..\head\GameInterfaces\ET\2.60\src\release\qagame_mp_x86.dll" ".\et\game"
if errorlevel 1 goto Finished_Errors

echo -------------------------
echo Copying Linux Dlls
echo -------------------------

echo Copying omnibot_et.so
xcopy /e /f /y "..\..\..\head\Omnibot\build\ET\gcc\release\libomnibot_et.so" ".\omnibot_et.so"
if errorlevel 1 goto Finished_Errors

echo Copying ET server file
xcopy /e /f /y "..\..\..\head\GameInterfaces\ET\2.60\src\build\game\gcc\release\libqagame.mp.i386.so" ".\et\game\qagame.mp.i386.so"
if errorlevel 1 goto Finished_Errors

echo ----------
echo Successful
echo ----------

goto Finished

:Finished_Errors
echo Failed
goto Finished

:Finished
pause

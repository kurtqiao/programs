@echo off

echo Press anykey to install python2.7, pywin32, pyreadline
pause
cd /D "%~dp0"
echo install python2.7
call python-2.7.amd64.msi

echo install pywin32
call pywin32-219.win-amd64-py2.7.exe
echo install pyreadline
call pyreadline-2.1.win-amd64.exe

echo Press anykey to copy shutdown long run script
pause
xcopy /d .\src\* c:\longrun\
copy ".\src\BatGotAdmin.bat" "%USERPROFILE%\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup" /y
pause
set Path=%Path%;c:\Python27
cd "%USERPROFILE%\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup"
call BatGotAdmin.bat

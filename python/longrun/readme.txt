This is a longrun shtudown/reboot program, enable by python.

Support OS:
Windows 10 64bit

how to install
1. right click install.bat and run as administrator
   this batch file will install request python 64bit package: 
   python2.7, pywin32, pyreadline
   
2. will copy src\* to foler c:\longrun\
   and BatGotAdmin.bat will be copy to "%USERPROFILE%\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup"
   which help program auto start.

how to uninstall
1. delete the BatGotAdmin.bat in "%USERPROFILE%\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup"
  in windows 10, you can press "win"+"R" to run "shell:startup", 
  which help to locate startup folder quickly
 
 
Notice:
1. currently only support S5 and reboot in windows10 64bit
2. if run finish or stop, RtcWakeData will reset to no wake up status
3. if close program during long run, the RtcWakeData will be keep as last setting
   which will wake up system if you shutdown.
@echo off

set CD=%~dp0
set OutDir=SYSTEM32
if exist %SYSTEMROOT%\SysWOW64 set OutDir=SysWOW64

%SYSTEMROOT%\%OutDir%\xcopy /D /Y "%CD%MSVBVM60.DLL" %SYSTEMROOT%\%OutDir%\
regsvr32 /s %SYSTEMROOT%\%OutDir%\MSVBVM60.DLL

%SYSTEMROOT%\%OutDir%\xcopy /D /Y "%CD%MSCOMCTL.OCX" %SYSTEMROOT%\%OutDir%\
regsvr32 /s %SYSTEMROOT%\%OutDir%\MSCOMCTL.OCX

%SYSTEMROOT%\%OutDir%\xcopy /D /Y "%CD%MSHFLXGD.OCX" %SYSTEMROOT%\%OutDir%\
regsvr32 /s %SYSTEMROOT%\%OutDir%\MSHFLXGD.OCX

pause Установка завершена - проверьте отсутствие сообщений об ошибках установки!

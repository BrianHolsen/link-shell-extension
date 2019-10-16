@echo off

set BJKHOME=%~dp0
set VERSION=_%1
set zip=%BJKHOME%\..\tools\zip.exe

set HARDLINKHOME=..
set MEDIA=%BJKHOME%\..\Media

REM x86
REM
set BINDIR=%HARDLINKHOME%\Bin\win32\Release
set ARCHIVE=dupemerge%VERSION%.zip
call :ZipBinary %MEDIA%\%ARCHIVE% %BINDIR%
call :ZipAllFiles %MEDIA%\%ARCHIVE%
%BINDIR%\ln.exe %MEDIA%\%ARCHIVE% %MEDIA%\dupemerge.zip > nul

set ARCHIVE=dupemerge%VERSION%_dbg.zip
call :ZipBinary %MEDIA%\%ARCHIVE% %BINDIR%
call :ZipDebug %MEDIA%\%ARCHIVE% %BINDIR%

REM x64
REM
set BINDIR=%HARDLINKHOME%\Bin\x64\Release
set ARCHIVE=dupemerge64%VERSION%.zip
call :ZipBinary %MEDIA%\%ARCHIVE% %BINDIR%
call :ZipAllFiles %MEDIA%\%ARCHIVE%
%BINDIR%\ln.exe %MEDIA%\%ARCHIVE% %MEDIA%\dupemerge64.zip > nul

set ARCHIVE=dupemerge64%VERSION%_dbg.zip
call :ZipBinary %MEDIA%\%ARCHIVE% %BINDIR%
call :ZipDebug %MEDIA%\%ARCHIVE% %BINDIR%

:ausmausraus

REM
REM wait 10 seconds
REM
REM @ping 127.0.0.1 -n 10 -w 1000 > nul

goto :EOF

:ZipAllFiles
%ZIP% %1 Doc\dupemerge.html
%ZIP% %1 Doc\dupemerge.png
%ZIP% %1 Doc\amazon.de.png
%ZIP% %1 Doc\bitcoinlogo.png
%ZIP% %1 Doc\bitcoinlseqr.png
%ZIP% %1 Doc\license.txt
%ZIP% %1 Doc\license_tre.txt
%ZIP% %1 Doc\license_ultragetop.txt
exit /b

:ZipDebug
pushd %2
%ZIP% %1 "dupemerge.pdb"
popd
exit /b

:ZipBinary
pushd %2
%ZIP% %1 "dupemerge.exe"
popd
exit /b


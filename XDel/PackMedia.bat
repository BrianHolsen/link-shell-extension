REM @echo off

call ..\bat\settings.bat
call ..\bat\CertificateSettings.bat

set BJKHOME=%~dp0
set VERSION=_%1
set zip=%BJKHOME%..\tools\zip.exe -q

set HARDLINKHOME=..
set MEDIA=%BJKHOME%..\Media

REM x86
REM
set BINDIR=%HARDLINKHOME%\Bin\win32\Release
set ARCHIVE=xdel%VERSION%.zip
call :ZipBinary %MEDIA%\%ARCHIVE% %BINDIR%
call :ZipAllFiles %MEDIA%\%ARCHIVE%
copy %MEDIA%\%ARCHIVE% %MEDIA%\xdel.zip

REM x64
REM
set BINDIR=%HARDLINKHOME%\Bin\x64\Release
set ARCHIVE=xdel64%VERSION%.zip
call :ZipBinary %MEDIA%\%ARCHIVE% %BINDIR%
call :ZipAllFiles %MEDIA%\%ARCHIVE%
copy %MEDIA%\%ARCHIVE% %MEDIA%\xdel64.zip

REM x86 static
REM
set BINDIR=%HARDLINKHOME%\Bin\win32\ReleaseStatic
set ARCHIVE=xdelstatic%VERSION%.zip
call :ZipBinary %MEDIA%\%ARCHIVE% %BINDIR%
call :ZipAllFiles %MEDIA%\%ARCHIVE%
copy %MEDIA%\%ARCHIVE% %MEDIA%\xdelstatic.zip

REM x64 static
REM
set BINDIR=%HARDLINKHOME%\Bin\x64\ReleaseStatic
set ARCHIVE=xdel64static%VERSION%.zip
call :ZipBinary %MEDIA%\%ARCHIVE% %BINDIR%
call :ZipAllFiles %MEDIA%\%ARCHIVE%
copy %MEDIA%\%ARCHIVE% %MEDIA%\xdel64static.zip

:ausmausraus

REM
REM wait 10 seconds
REM
REM @ping 127.0.0.1 -n 10 -w 5000 > nul

goto :EOF

:ZipAllFiles
%ZIP% %1 Doc\xdel.html
%ZIP% %1 Doc\xdel.png
%ZIP% %1 Doc\amazon.de.png
%ZIP% %1 Doc\license.txt
exit /b

:ZipBinary
pushd %2
@set FILE_TO_SIGN="xdel.exe"
echo #%CERTIFICATE_PASSWORD%#
%SIGNTOOL% sign /p "%CERTIFICATE_PASSWORD%" /v /fd sha256 /td sha256 /f  %CERTIFICATE% /tr %TIME_SERVER% %FILE_TO_SIGN%
if %ERRORLEVEL% == 0 goto ZipIt
	echo ### Error Signing of xdel.exe failed
	exit /b 2
:ZipIt
%ZIP% %1 %FILE_TO_SIGN%
popd
exit /b

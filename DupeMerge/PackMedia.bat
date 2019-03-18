
set BJKHOME=%~dp0
set zip=%BJKHOME%\..\tools\zip.exe

set HARDLINKHOME=..
set MEDIA=%BJKHOME%\..\Media

REM x86
REM
set BINDIR=%HARDLINKHOME%\Bin\win32\Release
set ARCHIVE=dupemerge.zip
pushd %BINDIR%
%ZIP% %MEDIA%\%ARCHIVE% "dupemerge.exe"
popd
pushd Doc
Call :ZipAllFiles %ARCHIVE%
popd

set ARCHIVE=dupemerge_dbg.zip
pushd %BINDIR%
%ZIP% %MEDIA%\%ARCHIVE% "dupemerge.exe"
%ZIP% %MEDIA%\%ARCHIVE% "dupemerge.pdb"
popd

REM x64
REM
set BINDIR=%HARDLINKHOME%\Bin\x64\Release
set ARCHIVE=dupemerge64.zip
pushd %BINDIR%
%ZIP% %MEDIA%\%ARCHIVE% "dupemerge.exe"
popd
pushd Doc
Call :ZipAllFiles %ARCHIVE%
popd

set ARCHIVE=dupemerge64_dbg.zip
pushd %BINDIR%
%ZIP% %MEDIA%\%ARCHIVE% "dupemerge.exe"
%ZIP% %MEDIA%\%ARCHIVE% "dupemerge.pdb"
popd

:ausmausraus

REM
REM wait 10 seconds
REM
@ping 127.0.0.1 -n 10 -w 1000 > nul

goto :EOF

:ZipAllFiles
%ZIP% %1 dupemerge.html
%ZIP% %1 dupemerge.png
%ZIP% %1 amazon.de.png
%ZIP% %1 bitcoinlogo.png
%ZIP% %1 bitcoinlseqr.png
%ZIP% %1 license.txt
%ZIP% %1 license_tre.txt
%ZIP% %1 license_ultragetop.txt

exit /b

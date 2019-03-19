
set BJKHOME=%~dp0
set zip=%BJKHOME%\..\tools\zip.exe

set HARDLINKHOME=..
set MEDIA=%BJKHOME%\..\Media

REM x86
REM
set BINDIR=%HARDLINKHOME%\Bin\win32\Release
set ARCHIVE=ln.zip
call :ZipBinary %MEDIA%\%ARCHIVE% %BINDIR%
call :ZipAllFiles %MEDIA%\%ARCHIVE%

set ARCHIVE=ln_dbg.zip
call :ZipBinary %MEDIA%\%ARCHIVE% %BINDIR%
call :ZipDebug %MEDIA%\%ARCHIVE% %BINDIR%

REM x64
REM
set BINDIR=%HARDLINKHOME%\Bin\x64\Release
set ARCHIVE=ln64.zip
call :ZipBinary %MEDIA%\%ARCHIVE% %BINDIR%
call :ZipAllFiles %MEDIA%\%ARCHIVE%

set ARCHIVE=ln64_dbg.zip
call :ZipBinary %MEDIA%\%ARCHIVE% %BINDIR%
call :ZipDebug %MEDIA%\%ARCHIVE% %BINDIR%

:ausmausraus

REM
REM wait 10 seconds
REM
REM @ping 127.0.0.1 -n 10 -w 5000 > nul

goto :EOF

:ZipAllFiles
%ZIP% %1 ln.html

%ZIP% %1 Doc\Doc\1023limit.png
%ZIP% %1 Doc\amazon.de.png
%ZIP% %1 Doc\anchorpath.png
%ZIP% %1 Doc\bitcoinlogo.png
%ZIP% %1 Doc\bitcoinlseqr.png
%ZIP% %1 Doc\deloreanmerge.png
%ZIP% %1 Doc\delete.png
%ZIP% %1 Doc\deletefollow.png
%ZIP% %1 Doc\hardlinkcloneshowto.png
%ZIP% %1 Doc\hardlinkclonessmart.png
%ZIP% %1 Doc\junctionshowto.png
%ZIP% %1 Doc\ln.png
%ZIP% %1 Doc\mappednetworkdrive.png
%ZIP% %1 Doc\mountpointssplice.png
%ZIP% %1 Doc\multiplesource.png
%ZIP% %1 Doc\nestedreparsepoints.png
%ZIP% %1 Doc\ontheflydupemerge.png
%ZIP% %1 Doc\outerjunctionscrop.png
%ZIP% %1 Doc\outerjunctionssplice.png
%ZIP% %1 Doc\outerjunctionsunroll.png
%ZIP% %1 Doc\outerjunctionsunrollcircularity.png
%ZIP% %1 Doc\outerjunctionsunrolldiskid.png
%ZIP% %1 Doc\outerjunctionsunrollinner.png
%ZIP% %1 Doc\outerjunctionsunrollinnernested.png
%ZIP% %1 Doc\outerjunctionsunrollinnersymlink.png
%ZIP% %1 Doc\outerjunctionsunrollmultitraverse.png
%ZIP% %1 Doc\smartcopyhowto.png
%ZIP% %1 Doc\smartmovehowto.png
%ZIP% %1 Doc\symboliclinkhowto.png
%ZIP% %1 Doc\symlinkssmart.png
%ZIP% %1 Doc\zeiteisen.png

%ZIP% %1 Doc\bat\DeleteAllHardlinks.bat
%ZIP% %1 Doc\bat\DeLoreanCopy.bat
%ZIP% %1 Doc\bat\DeLoreanHanoi.bat
%ZIP% %1 Doc\bat\vss-exec.cmd
%ZIP% %1 Doc\bat\vss_unc.cmd
%ZIP% %1 Doc\bat\vss_raw.cmd
%ZIP% %1 Doc\bat\vss_drivecopy.cmd
%ZIP% %1 Doc\bat\QueryPath.cmd
%ZIP% %1 Doc\bat\vshadow.zip
%ZIP% %1 Doc\bat\dosdev.exe
  
%ZIP% %1 Doc\license.txt
%ZIP% %1 Doc\license_tre.txt
%ZIP% %1 Doc\license_ultragetop.txt

%ZIP% %1 Doc\Blog\blog.html
%ZIP% %1 Doc\Blog\blog.png
%ZIP% %1 Doc\Blog\deloreandelete.png
%ZIP% %1 Doc\blog\deloreandelete_sequence.png
%ZIP% %1 Doc\Blog\lnbirthday.png
%ZIP% %1 Doc\Blog\blog.html
exit /b

:ZipDebug
pushd %2
%ZIP% %1 "ln.pdb"
popd
exit /b

:ZipBinary
pushd %2
%ZIP% %1 "ln.exe"
popd
exit /b

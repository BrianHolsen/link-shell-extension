
set BJKHOME=%~dp0
set zip=%BJKHOME%\..\tools\zip.exe


set HARDLINKHOME=..
set MEDIA=%BJKHOME%\..\Media

REM x86
REM
set BINDIR=%HARDLINKHOME%\Bin\win32\Release
set ARCHIVE=ln.zip
pushd %BINDIR%
%ZIP% %MEDIA%\%ARCHIVE% "ln.exe"
popd
pushd Doc
Call :ZipAllFiles %MEDIA%\%ARCHIVE%
popd

set ARCHIVE=ln_dbg.zip
pushd %BINDIR%
%ZIP% %MEDIA%\%ARCHIVE% "ln.exe"
%ZIP% %MEDIA%\%ARCHIVE% "ln.pdb"
popd


REM x64
REM
set BINDIR=%HARDLINKHOME%\Bin\x64\Release
set ARCHIVE=ln64.zip
pushd %BINDIR%
%ZIP% %MEDIA%\%ARCHIVE% "ln.exe"
popd
pushd Doc
Call :ZipAllFiles %MEDIA%\%ARCHIVE%
popd

set ARCHIVE=ln64_dbg.zip
pushd %BINDIR%
%ZIP% %MEDIA%\%ARCHIVE% "ln.exe"
%ZIP% %MEDIA%\%ARCHIVE% "ln.pdb"
popd

:ausmausraus

REM
REM wait 10 seconds
REM
@ping 127.0.0.1 -n 10 -w 5000 > nul

goto :EOF

:ZipAllFiles
%ZIP% %1 ln.html

%ZIP% %1 1023limit.png
%ZIP% %1 amazon.de.png
%ZIP% %1 anchorpath.png
%ZIP% %1 bitcoinlogo.png
%ZIP% %1 bitcoinlseqr.png
%ZIP% %1 deloreanmerge.png
%ZIP% %1 delete.png
%ZIP% %1 deletefollow.png
%ZIP% %1 hardlinkcloneshowto.png
%ZIP% %1 hardlinkclonessmart.png
%ZIP% %1 junctionshowto.png
%ZIP% %1 ln.png
%ZIP% %1 mappednetworkdrive.png
%ZIP% %1 mountpointssplice.png
%ZIP% %1 multiplesource.png
%ZIP% %1 nestedreparsepoints.png
%ZIP% %1 ontheflydupemerge.png
%ZIP% %1 outerjunctionscrop.png
%ZIP% %1 outerjunctionssplice.png
%ZIP% %1 outerjunctionsunroll.png
%ZIP% %1 outerjunctionsunrollcircularity.png
%ZIP% %1 outerjunctionsunrolldiskid.png
%ZIP% %1 outerjunctionsunrollinner.png
%ZIP% %1 outerjunctionsunrollinnernested.png
%ZIP% %1 outerjunctionsunrollinnersymlink.png
%ZIP% %1 outerjunctionsunrollmultitraverse.png
%ZIP% %1 smartcopyhowto.png
%ZIP% %1 smartmovehowto.png
%ZIP% %1 symboliclinkhowto.png
%ZIP% %1 symlinkssmart.png
%ZIP% %1 zeiteisen.png

%ZIP% %1 bat\DeleteAllHardlinks.bat
%ZIP% %1 bat\DeLoreanCopy.bat
%ZIP% %1 bat\DeLoreanHanoi.bat
%ZIP% %1 bat\vss-exec.cmd
%ZIP% %1 bat\vss_unc.cmd
%ZIP% %1 bat\vss_raw.cmd
%ZIP% %1 bat\vss_drivecopy.cmd
%ZIP% %1 bat\QueryPath.cmd
%ZIP% %1 bat\vshadow.zip
%ZIP% %1 bat\dosdev.exe
  
%ZIP% %1 license.txt
%ZIP% %1 license_tre.txt
%ZIP% %1 license_ultragetop.txt

%ZIP% %1 Blog\blog.html
%ZIP% %1 Blog\blog.png
%ZIP% %1 Blog\deloreandelete.png
%ZIP% %1 blog\deloreandelete_sequence.png
%ZIP% %1 Blog\lnbirthday.png
%ZIP% %1 Blog\blog.html

exit /b

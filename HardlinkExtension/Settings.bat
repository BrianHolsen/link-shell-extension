REM goto Win10_arachnida
REM goto Win10_sleipnir
REM goto atgrzwn501360
REM goto atgrzwn501360_d
REM goto quadtatzw10
REM goto xp_nbuild16
goto Win7_quadtatz
REM goto VmWareW2K12
REM goto xp_nbuild16

REM goto atgrzwd511850
REM goto VmWareVista
REM goto VmWareW764
REM goto VmWareW2K8
REM goto VmWareNt4


:atgrzwn501360
REM
REM atgrzwn501360
REM
set HOME=N:\data\cpp\hardlinks\HardlinkExtension
set INST=C:\Program Files\LinkShellExtension
set TOOLS=c:\tools
set PLATT=X64
set OS=Win7
set NSISDIR=C:\Program Files (x86)\NSIS
set LH=atgrzwn501360
set USERNAME=hermann
set VS2005TOOLS_PATH=c:\vs2005\Common7\Tools\Bin
set TRUECRYPT="C:\Program Files\TrueCrypt\TrueCrypt.exe"  /force 
goto eof

:atgrzwn501360_d
REM
REM atgrzwn501360 on Drive D:
REM
set HOME=D:\data\cpp\hardlinks\HardlinkExtension
set INST=C:\Program Files\LinkShellExtension
set TOOLS=c:\tools
set PLATT=X64
set OS=Win7
set NSISDIR=C:\Program Files (x86)\NSIS
set LH=atgrzwn501360
set USERNAME=hermann
set VS2005TOOLS_PATH=c:\vs2005\Common7\Tools\Bin
set TRUECRYPT="C:\Program Files\TrueCrypt\TrueCrypt.exe"  /force 
goto eof

:Win7_quadtatz
REM
REM Win7_quadtatz
REM
set HOME=c:\Data\cpp\hardlinks\HardlinkExtension
set INST=c:\Program Files\LinkShellExtension
set TOOLS=d:\tools
set PLATT=X64
set OS=Win7
REM set OS=Xp
set NSISDIR=c:\Program Files\NSIS
set LH=quadtatz
set USERNAME=schinagl
set VS2005TOOLS_PATH=C:\vs2005\Common7\Tools\Bin
set TRUECRYPT="C:\Program Files\TrueCrypt\TrueCrypt.exe" /force 
goto eof


:Win10_sleipnir
REM
REM Win10_sleipnir
REM
set HOME=D:\Data\cpp\hardlinks\HardlinkExtension
set INST=c:\Program Files\LinkShellExtension
set TOOLS=d:\tools
set PLATT=X64
set OS=Win7
REM set OS=Xp
set NSISDIR=c:\Program Files\NSIS
set LH=sleipnir
set USERNAME=schinagl
set VS2005TOOLS_PATH=C:\vs2005\Common7\Tools\Bin
set TRUECRYPT="C:\Program Files\TrueCrypt\TrueCrypt.exe" /force 
goto eof

:Win10_arachnida
REM
REM Win10_arachnida
REM
set HOME=c:\Data\cpp\hardlinks\HardlinkExtension
set INST=c:\Program Files\LinkShellExtension
set TOOLS=c:\tools
set PLATT=X64
set OS=Win7
REM set OS=Xp
set NSISDIR=c:\Program Files\NSIS
set LH=arachnida
set USERNAME=schinagl
set VS2005TOOLS_PATH=C:\vs2005\Common7\Tools\Bin
set TRUECRYPT="C:\Program Files\TrueCrypt\TrueCrypt.exe" /force 
goto eof


:quadtatzw10
REM
REM Win10_quadtatz on Drive c:
REM
set HOME=c:\data\cpp\hardlinks\HardlinkExtension
set INST=C:\Program Files\LinkShellExtension
set TOOLS=c:\tools
set PLATT=X64
set OS=Win7
set NSISDIR=C:\Program Files\NSIS
set LH=quadtatzw10
set USERNAME=schinagl
set VS2005TOOLS_PATH=c:\vs2005\Common7\Tools\Bin
set TRUECRYPT="C:\Program Files\TrueCrypt\TrueCrypt.exe"  /force 
goto eof

:xp_nbuild16
REM
REM xp_nbuild16
REM
set HOME=C:\Data\cpp\hardlinks\HardlinkExtension
set INST=C:\Program Files\LinkShellExtension
set TOOLS=C:\tools
set PLATT=Win32
set NSISDIR=C:\Program Files\NSIS
set LH=nbuild16
set OS=Xp
set USERNAME=Administrator
set VS2005TOOLS_PATH=C:\vs2005\Common7\Tools\Bin
set TRUECRYPT="C:\Program Files\TrueCrypt\TrueCrypt.exe" /force 
goto eof

:atgrzwd511850
REM
REM atgrzwd511850
REM
set HOME=m:\data\hardlinks\HardlinkExtension
set INST=C:\Program Files\LinkShellExtension
set TOOLS=c:\tools
set PLATT=X64
set OS=Win7
set NSISDIR=c:\Program Files\NSIS
set LH=atgrzwd511850
set USERNAME=schinagl
set VS2005TOOLS_PATH=c:\vs2005\Common7\Tools\Bin
goto eof



:VmWareW2K8
REM
REM VmWareW2K8
REM
set HOME=C:\data\cpp\hardlinks\HardlinkExtension
set INST=C:\tmp
set TOOLS=c:\tools
set PLATT=Win32
set OS=Win7
set NSISDIR=c:\Program Files\NSIS
set LH=NONONONONO
set USERNAME=NONONONONO
set VS2005TOOLS_PATH=c:\vs2005\Common7\Tools\Bin
goto eof

:VmWareW2K12
REM
REM VmWareW2K12
REM
set HOME=C:\data\hardlinks\HardlinkExtension
set INST=C:\tmp
set TOOLS=c:\tools
set PLATT=X64
set OS=Win7
set NSISDIR=c:\Program Files\NSIS
set LH=ws2012
set USERNAME=administrator
set VS2005TOOLS_PATH=c:\vs2005\Common7\Tools\Bin
goto eof

:VmWareNt4
REM
REM VmWareNt4
REM
set HOME=C:\data\cpp\hardlinks\HardlinkExtension
set INST=C:\Program Files\LinkShellExtension
set TOOLS=c:\tools
set PLATT=Win32
set NSISDIR=c:\Program Files\NSIS
set LH=NONONONONO
set USERNAME=NONONONONO
set VS2005TOOLS_PATH=c:\vs2005\Common7\Tools\Bin
goto eof

:eof
set PSKILL=%TOOLS%\pskill.exe

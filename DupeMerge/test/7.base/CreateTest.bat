set LN=ln.exe
set DEST=..\7

rmdir /Q /S %DEST%

mkdir %DEST%
copy 0000 %DEST%
%LN% %DEST%\0000 %DEST%\0010
%LN% %DEST%\0000 %DEST%\0020
%LN% %DEST%\0000 %DEST%\0030

copy 0000 %DEST%\0100
%LN% %DEST%\0100 %DEST%\0110
%LN% %DEST%\0100 %DEST%\0120

copy 0000 %DEST%\0200
%LN% %DEST%\0200 %DEST%\0210

copy 0000 %DEST%\0300

copy 0000 %DEST%\0400


copy 1000 %DEST%
copy 1000 %DEST%\1100


copy 2000 %DEST%\2000
%LN% %DEST%\2000 %DEST%\2010
%LN% %DEST%\2000 %DEST%\2020
%LN% %DEST%\2000 %DEST%\2030


copy 3000 %DEST%


copy 5000 %DEST%\5000
%LN% %DEST%\5000 %DEST%\5010
%LN% %DEST%\5000 %DEST%\5020
%LN% %DEST%\5000 %DEST%\5030

copy 5000 %DEST%\5100

copy 6000 %DEST%

copy 7000 %DEST%

copy 8000 %DEST%
copy 8000 %DEST%\8100
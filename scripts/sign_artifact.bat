REM TO use certificate, first install it on Winddows (double click and install with Wizzard, as not exportable)
REM it will be stored in local certificate store, and can be accessed without password

REM USAGE: sign_artifact.bat "Some description" "file.exe"

set DESCRIPTION=%1
set FILE_PATH=%2


set SIGN_TOOL="C:\Program Files (x86)\Windows Kits\10\bin\x64\signtool.exe"
set TIME_SERVER="http://timestamp.entrust.net/TSS/RFC3161sha2TS"
set CERTIFICATE="WizzDev"

%SIGN_TOOL% sign /tr %TIME_SERVER% /v /sm /s My /n %CERTIFICATE% /d %DESCRIPTION% %FILE_PATH%

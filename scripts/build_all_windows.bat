set CMAKE="C:\Program Files\CMake\bin\cmake.exe"
set INNOSETUP="C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
rem set MSBUILD="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe"
set VISUAL_STUDIO_USED= "Visual Studio 15 2017 Win64"
rem set VISUAL_STUDIO_USED= "Visual Studio 16 2019"
set SOURCE_DIR=%~dp0..
set BUILD_DIR="%SOURCE_DIR%\build"
set SCRIPTS_DIR="%SOURCE_DIR%\scripts"
set SIGN_SCRIPT="%SCRIPTS_DIR%\sign_artifact.bat"
set CL=/MP

rem Default build configuration
set DEBUG_BUILD=1
set SIGN_SOFTWARE=0
set CREATE_INSTALER=0

echo on
setlocal


goto :ParseParameters

:PrintHelp
echo.
echo Compile TrackHat driver with tools (default as Debug).
echo.
echo Usage: %1 [OPTION]
echo.
echo General options:
echo.
echo    --help or /?    Show this help.
echo    --release       Build as a Release without debug symbols.
echo    --sign          Sign the software with a certificate (only for Release).
echo    --installer     Create installer for the driver after build (only for Release).
echo.
exit /b


:ParseParameters
    for %%A in (%*) do (
        if "%%A"=="--help" (
            call :PrintHelp %0
            goto :exit

        ) else if "%%A"=="/?" (
            call :PrintHelp %0
            goto :exit

        ) else if "%%A"=="--release" (
            echo "Create release build."
            set DEBUG_BUILD=0

        ) else if "%%A"=="--sign" (
            echo "Sign the software with a certificate."
            set SIGN_SOFTWARE=1

        ) else if "%%A"=="--installer" (
            echo "Create installer."
            set CREATE_INSTALER=1

        ) else (
            echo "Wrong parameter. Printing help"
            call :PrintHelp %0
            goto :showerror
        )
    )

echo Script variables:
echo DEBUG_BUILD=%DEBUG_BUILD%
echo SIGN_SOFTWARE=%SIGN_SOFTWARE%
echo CREATE_INSTALER=%CREATE_INSTALER%


if %DEBUG_BUILD%==1 (
    set CONFIGURATION=Debug
) else (
    set CONFIGURATION=Release
)


echo Create Visual Studio projects

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"
cmake -G %VISUAL_STUDIO_USED% ..^
    -UINSTALL_BIN_DIR^
    -UINSTALL_INC_DIR^
    -UINSTALL_LIB_DIR^
    -UINSTALL_MAN_DIR^
    -UINSTALL_PKGCONFIG_DIR^
    -DCMAKE_CXX_FLAGS_RELEASE:string="/MT"^
    -DCMAKE_CXX_FLAGS_DEBUG:string="/MTd"^
    -DCMAKE_C_FLAGS_RELEASE:string="/MT"^
    -DCMAKE_C_FLAGS_DEBUG:string="/MTd"
if %errorlevel% neq 0 goto :showerror


echo Compile Visual Studio projects

rem %MSBUILD% ALL_BUILD.vcxproj /property:Configuration=%CONFIGURATION% /p:Platform="x64"
msbuild ALL_BUILD.vcxproj /property:Configuration=%CONFIGURATION% /p:Platform="x64"
if %errorlevel% neq 0 goto :showerror


rem Signing binaries

if %SIGN_SOFTWARE%==1 (
    if %DEBUG_BUILD%==0 (
        echo Signing binaries
        call "%SIGN_SCRIPT%" "TrackHat dll" "%BUILD_DIR%\src\Release\track-hat.dll"
    ) else (
        echo Error: software signing is available only for Release.
        goto :showerror
    )
)
if %errorlevel% neq 0 goto :showerror


rem Create installer

if %CREATE_INSTALER%==1 (
    if %DEBUG_BUILD%==0 (
        echo Create installer
        %INNOSETUP% %SCRIPTS_DIR%/installer.iss
    ) else (
        echo Error: creating installer is avalible only for Release.
        goto :showerror
    )
)
if %errorlevel% neq 0 goto :showerror


rem Signing installer

if %SIGN_SOFTWARE%==1 (
    if %CREATE_INSTALER%==1 (
        if %DEBUG_BUILD%==0 (
            echo Signing installer
            call "%SIGN_SCRIPT%" "TrackHat" "%BUILD_DIR%\installer\TrackHat-driver-installer-*.exe"
        ) else (
            echo Error: software signing is available only for Release.
            goto :showerror
        )
    )
)
if %errorlevel% neq 0 goto :showerror

endlocal
goto :exit

:showerror
echo Build ERROR occurred
exit /b 1

:exit
echo Building SUCCESSFUL
exit /b 0

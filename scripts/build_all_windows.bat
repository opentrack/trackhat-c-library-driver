set CMAKE="C:\Program Files\CMake\bin\cmake.exe"
set SOURCE_DIR=%~dp0..
set BUILD_DIR="%SOURCE_DIR%\build"
set CL=/MP
set VISUAL_STUDIO_USED= "Visual Studio 16 2019"
set DEBUG=0

echo on
setlocal
@if [%1]==[/?] goto :Help
goto :Main

:Help
echo Compile TrackHat driver with tools^

Usage: %0 [OPTION]^

General options:^

    /? Show this help^

    /DEBUG:1 Build in debug mode. Otherwise mode RelWithDebInfo will be used.

exit /b 0


:ParseParameters
    If "%~1"=="" exit /b

    SET TempVar=%~1
    IF "%TempVar:~0,7%"=="/DEBUG:" (
        set DEBUG=%TempVar:~7,250%
        exit /b
    )
exit /b


:Main
FOR %%A IN (%*) DO (
echo A="%%A"
    call :ParseParameters %%A
)

if %DEBUG%==1 (
    set CONFIGURATION=Debug
) else (
    set CONFIGURATION=Release
)

rem Create Visual Studio projects

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

rem rem Compile Visual Studio projects

msbuild ALL_BUILD.vcxproj /property:Configuration=%CONFIGURATION% /p:Platform="x64"
if %errorlevel% neq 0 goto :showerror

endlocal
goto :exit

:showerror
echo Build error occurred
pause

:exit

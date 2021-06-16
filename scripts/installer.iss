; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "TrackHat driver"
#define MyAppVersion "0.1"
#define MyAppPublisher "WizzDev"
#define MyAppURL "https://www.trackhat.org/"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{D142B095-224D-461F-8A04-B4F06D51FDFD}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
LicenseFile=C:\Users\pnowicki\Desktop\My\TrackHat\proj\trackhat-c-library-driver\LICENSE.txt
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
OutputDir=C:\Users\pnowicki\Desktop\My\TrackHat\proj\trackhat-c-library-driver\build\installer
OutputBaseFilename=TrackHat-driver-installer-{#MyAppVersion}
SetupIconFile=C:\Users\pnowicki\Desktop\My\TrackHat\proj\trackhat-c-library-driver\others\trackhat.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern
; Install as x64
ArchitecturesInstallIn64BitMode=x64
; Tell Windows Explorer to reload the environment
ChangesEnvironment=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "C:\Users\pnowicki\Desktop\My\TrackHat\proj\trackhat-c-library-driver\build\src\Release\track-hat.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "C:\Users\pnowicki\Desktop\My\TrackHat\proj\trackhat-c-library-driver\src\track_hat_driver.h"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\pnowicki\Desktop\My\TrackHat\proj\trackhat-c-library-driver\src\track_hat_types.h"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

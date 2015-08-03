; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "OpenNIC Wizard"
#define MyAppServiceName "OpenNIC Service"
#define MyAppVersion "0.4.rc1"
#define MyAppPublisher "The OpenNIC Project"
#define MyAppURL "http://www.opennicproject.org"
#define MyAppExeName "OpenNIC.exe"
#define MyAppServiceName "OpenNICServer.exe"

; !!! Inno Include seems to be broken !!
; #include "opennic-include.iss"
#if 1
; My Windows 7 64-bit configuration
#define MyOutputDir       "C:\Users\mike\Documents\GitHub"
#define MyOpenNICSource   "C:\Users\mike\Documents\GitHub\OpenNIC-Wizard"
#define MyOpenNICBuild    "C:\Users\mike\Documents\GitHub\OpenNIC-Wizard\OpenNIC-build-desktop_Debug"
#define MyQtLib           "C:\Qt\4.8.4\mingw\bin"
#define MyMingwBin        "C:\MinGW\bin"
#else
; My Windows XP 32-bit configuration
#define MyOutputDir       "C:\Documents and Settings\Mike\My Documents"
#define MyOpenNICSource   "C:\Documents and Settings\Mike\My Documents\OpenNIC\trunk"
#define MyOpenNICBuild    "C:\Documents and Settings\Mike\My Documents\OpenNIC\OpenNIC-build-desktop_Debug"
#define MyQtLib           "C:\QtSDK\Desktop\Qt\4.7.4\mingw\lib"
#define MyMingwBin        "C:\QtSDK\mingw\bin"
#endif

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={1D9BA5F4-E722-4670-804D-4BED7B4A26DA}
PrivilegesRequired=admin
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\OpenNIC
DefaultGroupName=OpenNIC
LicenseFile={#MyOpenNICSource}\COPYING
OutputDir={#MyOutputDir}
OutputBaseFilename=OpenNIC Wizard {#MyAppVersion}
WizardImageFile={#MyOpenNICSource}\installer.bmp
SetupIconFile={#MyOpenNICSource}\client\images\opennic.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"

[Files]
Source: {#MyOpenNICBuild}\client\debug\OpenNIC.exe; DestDir: {app}; Flags: ignoreversion; Permissions: admins-full; MinVersion: 0,5.1.2600;
Source: {#MyOpenNICBuild}\server\debug\OpenNICServer.exe; DestDir: {app}; Flags: ignoreversion; Permissions: admins-full; MinVersion: 0,5.1.2600;
Source: {#MyQtLib}\QtCored4.dll; DestDir: "{app}"; Flags: ignoreversion
Source: {#MyQtLib}\QtGuid4.dll; DestDir: "{app}"; Flags: ignoreversion
Source: {#MyQtLib}\QtNetworkd4.dll; DestDir: "{app}"; Flags: ignoreversion
Source: {#MyQtLib}\QtScriptd4.dll; DestDir: "{app}"; Flags: ignoreversion

Source: {#MyOpenNICSource}\server\dig\install\bindevt.dll; DestDir: "{app}"; Flags: ignoreversion
Source: {#MyOpenNICSource}\server\dig\install\dig.exe; DestDir: "{app}"; Flags: ignoreversion
Source: {#MyOpenNICSource}\server\dig\install\libbind9.dll; DestDir: "{app}"; Flags: ignoreversion
Source: {#MyOpenNICSource}\server\dig\install\libdns.dll; DestDir: "{app}"; Flags: ignoreversion
Source: {#MyOpenNICSource}\server\dig\install\libeay32.dll; DestDir: "{app}"; Flags: ignoreversion
Source: {#MyOpenNICSource}\server\dig\install\libisc.dll; DestDir: "{app}"; Flags: ignoreversion
Source: {#MyOpenNICSource}\server\dig\install\libisccc.dll; DestDir: "{app}"; Flags: ignoreversion
Source: {#MyOpenNICSource}\server\dig\install\libisccfg.dll; DestDir: "{app}"; Flags: ignoreversion
Source: {#MyOpenNICSource}\server\dig\install\liblwres.dll; DestDir: "{app}"; Flags: ignoreversion
Source: {#MyOpenNICSource}\unins000.exe.manifest; DestDir: "{app}"; Flags: ignoreversion
Source: {#MyOpenNICSource}\COPYING; DestDir: "{app}"; Flags: ignoreversion
Source: {#MyOpenNICSource}\README; DestDir: "{app}"; Flags: ignoreversion
Source: {#MyOpenNICSource}\server\bootstrap.t1; DestDir: {app}; Flags: ignoreversion; 
Source: {#MyOpenNICSource}\server\bootstrap.domains; DestDir: {app}; Flags: ignoreversion; 
Source: {#MyOpenNICSource}\server\OpenNICServer.exe.manifest; DestDir: {app}; Flags: ignoreversion; 

Source: {#MyMingwBin}\mingwm10.dll; DestDir: "{app}"; Flags: ignoreversion
Source: {#MyMingwBin}\libgcc_s_dw2-1.dll; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commonstartup}\OpenNIC Wizard"; Filename: "{app}\{#MyAppExeName}"

[Run]
Filename: "{app}\{#MyAppServiceName}"; Parameters: "-install"; WorkingDir: "{app}"; Flags:  runascurrentuser; Description: "{cm:LaunchProgram,{#StringChange(MyAppServiceName, "&", "&&")}}"; StatusMsg: "Installing OpenNIC Service...";
Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"; Flags: nowait runasoriginaluser; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, "&", "&&")}}"; StatusMsg: "Starting OpenNIC Task Tray Applet...";
FileName: "{sys}\sc.exe"; Parameters: "config OpenNIC start= auto"; Flags: runascurrentuser; StatusMsg: "Configuring OpenNIC Service...";
Filename: "{sys}\sc.exe"; Parameters: "start OpenNIC"; Flags: runascurrentuser; StatusMsg: "Starting OpenNIC Service...";
Filename: "http://grep.geek"; Description: "Open http://grep.geek in a web browser"; Flags: shellexec postinstall runasoriginaluser

[UninstallRun]
Filename: "{app}\{#MyAppServiceName}"; WorkingDir: "{app}"; Parameters: "-terminate";
Filename: "{app}\{#MyAppServiceName}"; WorkingDir: "{app}"; Parameters: "-uninstall";
Filename: "{sys}\sc.exe"; Parameters: "stop OpenNIC";
Filename: "{sys}\sc.exe"; Parameters: "delete OpenNIC";
Filename: "{sys}\sc.exe"; Parameters: "delete OpenNIC";

[Code]
function InitializeSetup(): Boolean;
begin
  Log('InitializeSetup() called');
  Result := True;
  if not IsAdminLoggedOn then
  begin
    MsgBox('This installer must be run as administrator',mbCriticalError,MB_OK);
    Result := False;
  end;
end;

#define MHWIAppName "MHWISaveEditor"
#define MHWIAppVersion "0.1.6"
#define MHWIAppPublisher "EnderHDMC"
#define MHWIAppURL "https://github.com/EnderHDMC/MHWISaveEditor"
#define MHWIAppExeName "MHWISaveEditor.exe"
#define MHWISubversion "0"

; The relative directory where the app is located.
#define RelDir "x64\Deploy"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{D1FB3774-D951-4645-BEB8-12831744823D}
AppName={#MHWIAppName}
AppVersion={#MHWIAppVersion}
;AppVerName={#MHWIAppName} {#MHWIAppVersion}
AppPublisher={#MHWIAppPublisher}
AppPublisherURL={#MHWIAppURL}
AppSupportURL={#MHWIAppURL}
AppUpdatesURL={#MHWIAppURL}
DefaultDirName=.\{#MHWIAppName}
DefaultGroupName={#MHWIAppName}
AllowNoIcons=yes
; Remove the following line to run in administrative install mode (install for all users.)
PrivilegesRequired=lowest
OutputDir={#RelDir}
OutputBaseFilename={#MHWIAppName}Setup-v{#MHWIAppVersion}
; NOTE: The app needs to be compiled in release mode before this script will work.
SetupIconFile={#RelDir}\res\icon.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern
Uninstallable=no
VersionInfoVersion={#MHWIAppVersion}.{#MHWISubversion}

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#RelDir}\{#MHWIAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#RelDir}\D3Dcompiler_47.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#RelDir}\libcurl.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#RelDir}\opengl32sw.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#RelDir}\Qt6Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#RelDir}\Qt6Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#RelDir}\Qt6Svg.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#RelDir}\Qt6Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#RelDir}\vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: ignoreversion dontcopy deleteafterinstall
Source: "{#RelDir}\iconengines\*"; DestDir: "{app}\iconengines"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#RelDir}\imageformats\*"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#RelDir}\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#RelDir}\res\*"; DestDir: "{app}\res"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#RelDir}\styles\*"; DestDir: "{app}\styles"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#RelDir}\translations\*"; DestDir: "{app}\translations"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Code]
function VCRedistNeedsInstall: Boolean;
var 
  Version: String;
begin
  if RegQueryStringValue(HKEY_LOCAL_MACHINE,
       'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Version',
       Version) then
  begin
    // Is the installed version at least 14.38 ? 
    Log('VC Redist Version check : found ' + Version);
    Result := (CompareStr(Version, 'v14.38.33130.00') < 0);
  end
  else 
  begin
    // Not even an old version installed
    Result := True;
  end;
  if (Result) then
  begin
    ExtractTemporaryFile('vc_redist.x64.exe');
  end;
end;

[Run]
Filename: {tmp}\vc_redist.x64.exe; \
    Parameters: "/quiet"; \
    Check: VCRedistNeedsInstall; \
    StatusMsg: "Installing VC++ 2015-2022 Redistributables..."
Filename: "{app}\{#MHWIAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MHWIAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!
#define MajorVersion 2
#define MinorVersion "05"
#define MyAppName "Astrolog32"
#define MyAppPublisher "Roberto Luporini"
#define MyAppURL "http://www.astrolog32.altervista.org/index.html"
#define MyAppExeName "astrolog32.exe"
#define MyAppUrlName "astrolog32.url"

[Setup]
AppName={#MyAppName}
AppVerName={#MyAppName} {#MajorVersion}.{#MinorVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputBaseFilename=Astrolog32_v2_{#MinorVersion}_setup
Compression=lzma/fast
SolidCompression=true
VersionInfoVersion={#MajorVersion}.{#MinorVersion}
VersionInfoCompany=Roberto Luporini
VersionInfoDescription=Astrolog32 Setup
VersionInfoCopyright=© 2014 Roberto Luporini
AppCopyright=© 2014 Roberto Luporini
ShowLanguageDialog=yes
LanguageDetectionMethod=uilanguage
ShowUndisplayableLanguages =true
AppVersion=2.05
AppID={{54BCBDE4-6822-43D1-8326-1DF8B5227B85}}
UninstallRestartComputer=true
InternalCompressLevel=max

[Tasks]
;Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[Files]
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: int\main\*.*; DestDir: {app}\main\; Flags: recursesubdirs createallsubdirs; Tasks: ; Languages: en
Source: int\main\*.*; DestDir: {app}\main\; Flags: recursesubdirs createallsubdirs; Tasks: ; Languages: cz
Source: int\main\*.*; DestDir: {app}\principal; Flags: recursesubdirs createallsubdirs; Tasks: ; Languages: fr
Source: int\main\*.*; DestDir: {app}\principale; Flags: recursesubdirs createallsubdirs; Tasks: ; Languages: it

Source: int\ephemeris\*.*; DestDir: {app}\ephemeris\; Flags: recursesubdirs createallsubdirs; Tasks: ; Languages: en
Source: int\ephemeris\*.*; DestDir: {app}\ephemeris\; Flags: recursesubdirs createallsubdirs; Tasks: ; Languages: cz
Source: int\ephemeris\*.*; DestDir: {app}\éphémérides; Flags: recursesubdirs createallsubdirs; Tasks: ; Languages: fr
Source: int\ephemeris\*.*; DestDir: {app}\effemeridi; Flags: recursesubdirs createallsubdirs; Tasks: ; Languages: it

Source: en\*.*; DestDir: {app}; Flags: recursesubdirs createallsubdirs; Languages: en
Source: cz\*.*; DestDir: {app}; Flags: recursesubdirs createallsubdirs; Languages: cz
Source: fr\*.*; DestDir: {app}; Flags: recursesubdirs createallsubdirs; Languages: fr
Source: it\*.*; DestDir: {app}; Flags: recursesubdirs createallsubdirs; Languages: it

[INI]
Filename: {app}\{#MyAppUrlName}; Section: InternetShortcut; Key: URL; String: {#MyAppURL}

[Icons]
Name: {group}\{#MyAppName}; Filename: {app}\main\{#MyAppExeName}; Languages: en; IconFilename: {app}\main\{#MyAppExeName}; Flags: useapppaths
Name: {group}\{#MyAppName}; Filename: {app}\main\{#MyAppExeName}; Languages: cz
Name: {group}\{#MyAppName}; Filename: {app}\principal\{#MyAppExeName}; Languages: fr
Name: {group}\{#MyAppName}; Filename: {app}\principale\{#MyAppExeName}; Languages: it

Name: {group}\{cm:ProgramOnTheWeb,{#MyAppName}}; Filename: {app}\{#MyAppUrlName}
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}

[Run]
Filename: {app}\main\{#MyAppExeName}; Description: {cm:LaunchProgram,{#MyAppName}}; Flags: nowait postinstall skipifsilent; Languages: en
Filename: {app}\main\{#MyAppExeName}; Description: {cm:LaunchProgram,{#MyAppName}}; Flags: nowait postinstall skipifsilent; Languages: cz
Filename: {app}\principal\{#MyAppExeName}; Description: {cm:LaunchProgram,{#MyAppName}}; Flags: nowait postinstall skipifsilent; Languages: fr
Filename: {app}\principale\{#MyAppExeName}; Description: {cm:LaunchProgram,{#MyAppName}}; Flags: nowait postinstall skipifsilent; Languages: it

[UninstallDelete]
Type: files; Name: {app}\main\astrolog32.dat; Languages: en
Type: files; Name: {app}\main\astrolog32.dat; Languages: cz
Type: files; Name: {app}\principal\astrolog32.dat; Languages: fr
Type: files; Name: {app}\principale\astrolog32.dat; Languages: it

Type: files; Name: {app}\{#MyAppUrlName}

[Languages]
Name: en; MessagesFile: compiler:Default.isl; LicenseFile: en\main\license.txt; InfoAfterFile: en\main\note.rtf
Name: fr; MessagesFile: compiler:Languages\French.isl; LicenseFile: fr\principal\license.txt; InfoAfterFile: fr\principal\note.rtf
Name: it; MessagesFile: compiler:Languages\Italian.isl; LicenseFile: it\principale\license.txt; InfoAfterFile: it\principale\note.rtf
Name: cz; MessagesFile: compiler:Languages\Czech.isl

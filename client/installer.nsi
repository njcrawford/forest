; example2.nsi
;
; This script is based on example1.nsi, but it remember the directory, 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install example2.nsi into a directory that the user selects,

!include LogicLib.nsh

;--------------------------------

; The name of the installer
Name "Forest Client for Windows"

; The file to write
OutFile "forest-client-install-x.y.z.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\forest-client"
; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Forest Client" "Install_Dir"

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Forest Client for Windows (required)"

  SectionIn RO

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "forest-client\bin\Release\forest-client.exe"
  File "forest-client\bin\Release\Interop.WUApiLib.dll"
  File "forest-client\bin\Release\IniFile.dll"
  File "ConfigurationGUI\bin\Release\ConfigurationGUI.exe"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM "SOFTWARE\Forest Client" "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Forest Client" "DisplayName" "Forest Client for Windows"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Forest Client" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Forest Client" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Forest Client" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Forest Client"
  CreateShortCut "$SMPROGRAMS\Forest Client\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Forest Client\Configuration.lnk" "$INSTDIR\ConfigurationGUI.exe" "" "$INSTDIR\ConfigurationGUI.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Forest Client"
  DeleteRegKey HKLM "SOFTWARE\3ds Viewer"

  ; Remove files and uninstaller
  Delete "$INSTDIR\forest-client.exe"
  Delete "$INSTDIR\Interop.WUApiLib.dll"
  Delete "$INSTDIR\IniFile.dll"
  Delete "$INSTDIR\ConfigurationGUI.exe"
  Delete "$INSTDIR\uninstall.exe"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Forest Client\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Forest Client"
  RMDir "$INSTDIR"

SectionEnd

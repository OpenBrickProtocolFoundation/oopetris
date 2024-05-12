;--------------------------------
; Includes

!include "MUI2.nsh"
!include "logiclib.nsh"
!include "FileFunc.nsh"

; test if required arguments were given in the command line
!ifndef VERSION
!error "VERSION not specified"
!endif

!ifndef PROJECT_SOURCE_DIR
!error "PROJECT_SOURCE_DIR not specified"
!endif

!ifndef PROJECT_BUILD_DIR
!error "PROJECT_BUILD_DIR not specified"
!endif

!ifndef NAME
!error "NAME not specified"
!endif

!ifndef AUTHOR
!error "AUTHOR not specified"
!endif

;--------------------------------
; Custom defines
!define APPFILE "oopetris.exe"
!define APP_UNINSTALLER_FILE "Uninstall.exe"
!define SLUG "${NAME} v${VERSION}"
!define ARP "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"
;--------------------------------
; General

Name "${NAME}"
OutFile "${NAME} Setup.exe"
InstallDir "$PROGRAMFILES\${NAME}"
InstallDirRegKey HKCU "Software\${NAME}" "InstallDir"
RequestExecutionLevel admin

;--------------------------------
; UI

!define MUI_ICON "${PROJECT_SOURCE_DIR}\assets\icon\icon.ico"
!define MUI_UNICON "${PROJECT_SOURCE_DIR}\assets\icon\icon.ico"
!define MUI_HEADERIMAGE
!define MUI_ABORTWARNING
!define MUI_WELCOMEPAGE_TITLE "${SLUG} Setup"

;--------------------------------
; Pages
  
; Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set UI language
!insertmacro MUI_LANGUAGE "English"


; Return on top of stack the total size of the selected (installed) sections, formated as DWORD
; Assumes no more than 256 sections are defined
;; source: https://nsis.sourceforge.io/Add_uninstall_information_to_Add/Remove_Programs
Var GetInstalledSize.total
Function GetInstalledSize
	Push $0
	Push $1
	StrCpy $GetInstalledSize.total 0
	${ForEach} $1 0 256 + 1
		${if} ${SectionIsSelected} $1
			SectionGetSize $1 $0
			IntOp $GetInstalledSize.total $GetInstalledSize.total + $0
		${Endif}
 
		; Error flag is set when an out-of-bound section is referenced
		${if} ${errors}
			${break}
		${Endif}
	${Next}
 
	ClearErrors
	Pop $1
	Pop $0
	IntFmt $GetInstalledSize.total "0x%08X" $GetInstalledSize.total
	Push $GetInstalledSize.total
FunctionEnd

;--------------------------------
; Section - Install App, always installed

Section "Core App" CoreApp
    SectionIn RO ; Read only, always installed

    ; install assets
    SetOutPath "$INSTDIR\assets\authors"
    File /r "${PROJECT_SOURCE_DIR}\assets\authors\*.*"

    SetOutPath "$INSTDIR\assets\fonts"
    File /r "${PROJECT_SOURCE_DIR}\assets\fonts\*.*"

    SetOutPath "$INSTDIR\assets\icons"
    File /r "${PROJECT_SOURCE_DIR}\assets\icons\*.*"
    
    ; install the windows icon to use it
    SetOutPath "$INSTDIR"
    File "/oname=${NAME}.ico" "${PROJECT_SOURCE_DIR}\assets\icon\icon.ico" 

    ; install executable
    SetOutPath "$INSTDIR"
    File /a "${PROJECT_BUILD_DIR}\${APPFILE}"

    ; install dynamic libraries
    SetOutPath "$INSTDIR"
    File "${PROJECT_SOURCE_DIR}\subprojects\discord_game_sdk-3.2.1\lib\x86_64\discord_game_sdk.dll"

    ; install default settings (DO NOT Override)
    SetOutPath "$APPDATA\${AUTHOR}\${NAME}"
    SetOverwrite off
    File "${PROJECT_SOURCE_DIR}\settings.json"
    SetOverwrite on

    ; write some regestry keys, just for information
    WriteRegStr HKCU "Software\${NAME}" "Version" "${VERSION}"

    ; write the uninstaller and needed information
    WriteRegStr HKCU "Software\${NAME}" "InstallDir" $INSTDIR
    WriteUninstaller "$INSTDIR\${APP_UNINSTALLER_FILE}"

    ; create start menu entry
    CreateDirectory '$SMPROGRAMS\${NAME}'
    CreateShortCut '$SMPROGRAMS\${NAME}\${NAME}.lnk' '$INSTDIR\${APPFILE}' "" '$INSTDIR\${APPFILE}' 0
    CreateShortCut '$SMPROGRAMS\${NAME}\Uninstall ${NAME}.lnk' '$INSTDIR\${APP_UNINSTALLER_FILE}' "" '$INSTDIR\${APP_UNINSTALLER_FILE}' 0

    ; create metadata for some windows features to work correctly

    ; Write the uninstall keys for Windows
    WriteRegStr HKCU "${ARP}" "UninstallString" '"$INSTDIR\${APP_UNINSTALLER_FILE}"'
    WriteRegStr HKCU "${ARP}" "QuietUninstallString" '"$INSTDIR\${APP_UNINSTALLER_FILE}"'

    ; write some generic metadata about the uninstaller
    WriteRegStr HKCU "${ARP}" "DisplayName" "${NAME}"
    WriteRegStr HKCU "${ARP}" "DisplayVersion" "${VERSION}"
    WriteRegStr HKCU "${ARP}" "Publisher" "${AUTHOR}"
    WriteRegStr HKCU "${ARP}" "DisplayIcon" "$INSTDIR\${NAME}.ico"

    ; get the estimated size and use it
    Call GetInstalledSize
    WriteRegDWORD HKCU "${ARP}" "EstimatedSize" "$GetInstalledSize.total"
    
    ; no modify or repair functionality given
    WriteRegDWORD HKCU "${ARP}" "NoModify" 1
    WriteRegDWORD HKCU "${ARP}" "NoRepair" 1
SectionEnd


;--------------------------------
; Section - Install Music assets, optional, can be selected by user if he wants to have music

Section "Music" Music
    ; install music assets (these are larger)
    SetOutPath "$INSTDIR\assets\music"
    File /r "${PROJECT_SOURCE_DIR}\assets\music\*.*"
    
    ; store that this is installed
    WriteRegDWORD HKCU "Software\${NAME}" "MusicInstalled" 1
SectionEnd
    

;--------------------------------
; Section - Install Binary helper, optional, can be selected by user if he wants to have one

Section "Additional Binaries" AdditionalBinaries
    ; install helper binaries
    SetOutPath "$INSTDIR\bin"
    File /a "${PROJECT_BUILD_DIR}\oopetris_recordings_utility.exe"
    
    ; add the bin dir to the path
    EnVar::SetHKCU 
    EnVar::AddValue "Path" "$INSTDIR\bin"

    ; store that this is installed
    WriteRegDWORD HKCU "Software\${NAME}" "AddBin" 1
SectionEnd

;--------------------------------
; Descriptions

;Language strings
LangString DESC_CoreApp ${LANG_ENGLISH} "Install Core App"
LangString DESC_BinaryFiles ${LANG_ENGLISH} "Install Additional Binaries"
LangString DESC_Music ${LANG_ENGLISH} "Install Music Assets"

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${CoreApp} $(DESC_CoreApp)
  !insertmacro MUI_DESCRIPTION_TEXT ${AdditionalBinaries} $(DESC_BinaryFiles)
  !insertmacro MUI_DESCRIPTION_TEXT ${Music} $(DESC_Music)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
; Remove empty parent directories (on uninstall)

Function un.RMDirUP
    !define RMDirUP '!insertmacro RMDirUPCall'

    !macro RMDirUPCall _PATH
          push '${_PATH}'
          Call un.RMDirUP
    !macroend

    ; $0 - current folder
    ClearErrors

    Exch $0
    ;DetailPrint "ASDF - $0\.."
    RMDir "$0\.."

    IfErrors Skip
    ${RMDirUP} "$0\.."
    Skip:

    Pop $0
FunctionEnd

;--------------------------------
; Section - Uninstaller

Section "Uninstall"

  ; Delete assets
  RMDir /r "$INSTDIR\assets"

  ; Delete executable
  Delete "$INSTDIR\${APPFILE}"

  ; Delete Uninstall file
  Delete "$INSTDIR\${APP_UNINSTALLER_FILE}"

  ; Delete the rest of the Folder
  RMDir /r "$INSTDIR"
  ${RMDirUP} "$INSTDIR"

  ; note settings are NOT removed!

  ; delete start menu entry
  Delete '$SMPROGRAMS\${NAME}\${NAME}.lnk' 
  Delete '$SMPROGRAMS\${NAME}\Uninstall ${NAME}.lnk'
  
  RMDir /r '$SMPROGRAMS\${NAME}'
  ${RMDirUP} '$SMPROGRAMS\${NAME}'

  ReadRegDWORD $0 HKCU "Software\${NAME}" "AddBin"

  ${If} $0 != 0
    ; we have additional binaries installed

    ; remove the bin dir from the path
    EnVar::SetHKCU 
    EnVar::DeleteValue "Path" "$INSTDIR\bin"
  ${EndIf}

  ; delete the whole reg key
  DeleteRegKey HKCU "Software\${NAME}"

  ; delete the whole reg keyfor the uninstaller
  DeleteRegKey HKCU "${ARP}"
SectionEnd

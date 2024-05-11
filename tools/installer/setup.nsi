;--------------------------------
; Includes

!include "MUI2.nsh"
!include "logiclib.nsh"

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

    SetOutPath "$INSTDIR\assets\music"
    File /r "${PROJECT_SOURCE_DIR}\assets\music\*.*"

    ; install executable
    SetOutPath "$INSTDIR"
    File /a "${PROJECT_BUILD_DIR}\${APPFILE}"

    ; install dynamic libraries
    SetOutPath "$INSTDIR"
    File /a "${PROJECT_SOURCE_DIR}\subprojects\discord_game_sdk-3.2.1\lib\x86_64\discord_game_sdk.dll"

    ; install default settings (DO NOT Override)
    SetOutPath "$APPDATA\${AUTHOR}\${NAME}"
    SetOverwrite off
    File  "${PROJECT_SOURCE_DIR}\settings.json"
    SetOverwrite on

    WriteRegStr HKCU "Software\${NAME}" "InstallDir" $INSTDIR
    WriteUninstaller "$INSTDIR\${APP_UNINSTALLER_FILE}"

    ; create start menu entry
    CreateDirectory '$SMPROGRAMS\${NAME}'
    CreateShortCut '$SMPROGRAMS\${NAME}\${NAME}.lnk' '$INSTDIR\${APPFILE}' "" '$INSTDIR\${APPFILE}' 0
    CreateShortCut '$SMPROGRAMS\${NAME}\Uninstall ${NAME}.lnk' '$INSTDIR\${APP_UNINSTALLER_FILE}' "" '$INSTDIR\${APP_UNINSTALLER_FILE}' 0
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
LangString DESC_BinaryFiles ${LANG_ENGLISH} "Install (Optional) Additional Binaries"


;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${CoreApp} $(DESC_CoreApp)
  !insertmacro MUI_DESCRIPTION_TEXT ${AdditionalBinaries} $(DESC_BinaryFiles)
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
SectionEnd

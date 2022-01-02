; Script generated by the HM NIS Edit Script Wizard.
; $Id: lqn-solvers.nsi 14978 2021-09-14 01:31:42Z greg $
; $HeadURL$

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "LQN Solvers"
!define PRODUCT_VERSION "5.23"
!define PRODUCT_PUBLISHER "Systems Engineering, Carleton University"
!define PRODUCT_WEB_SITE "http://www.layeredqueues.org"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\lqns.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "..\license.txt"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\readme.txt"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "lqn_nt4_5_23.exe"
InstallDir "$PROGRAMFILES\LQN Solvers"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "Executables" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  CreateDirectory "$SMPROGRAMS\LQN Solvers"
  File "..\..\lqns\lqns.exe"
;  CreateShortCut "$SMPROGRAMS\LQN Solvers\LQN Solvers.lnk" "$INSTDIR\lqns.exe"
;  CreateShortCut "$DESKTOP\LQN Solvers.lnk" "$INSTDIR\lqns.exe"
  File "..\..\lqsim\lqsim.exe"
  File "..\..\lqn2ps\lqn2ps.exe"
  File /oname=lqn2xml.exe "..\..\lqn2ps\lqn2ps.exe"
  File /oname=lqn2svg.exe "..\..\lqn2ps\lqn2ps.exe"
  File /oname=lqn2emf.exe "..\..\lqn2ps\lqn2ps.exe"
  File /oname=lqn2lqn.exe "..\..\lqn2ps\lqn2ps.exe"
;  CreateShortCut "$INSTDIR\lqn2xml.lnk" "$INSTDIR\lqn2ps.exe" "-Oxml"
;  CreateShortCut "$INSTDIR\lqn2out.lnk" "$INSTDIR\lqn2ps.exe" "-Oout"
;  CreateShortCut "$INSTDIR\lqn2emf.lnk" "$INSTDIR\lqn2ps.exe" "-Oemf"
;  CreateShortCut "$INSTDIR\lqn2lqn.lnk" "$INSTDIR\lqn2ps.exe" "-Olqn"
  File "..\..\srvndiff\srvndiff.exe"
  File "..\..\lqngen\lqngen.exe"
  File "..\..\qnsolver\qnsolver.exe"
  File /oname=lqn2lqx.exe "..\..\lqngen\lqngen.exe"
;;; MinGW stuff.
;  File "C:\mingw\bin\mingwm10.dll"
;;; Xerces XML stuff.
;  File "C:\msys\1.0\local\bin\libxerces-c-3-0.dll"
;  File "C:\MinGW\bin\libgnurx-0.dll"
;  File "C:\MinGW\bin\libexpat-1.dll"
;  File "..\..\lqiolib\src\liblqio-0.dll"
;  File "..\..\lqx\libsrc\liblqx-0.dll"
  File "..\..\xml\lqn-core.xsd"
  File "..\..\xml\lqn-sub.xsd"
  File "..\..\xml\lqn.xsd"
SectionEnd

Section "Documentation" SEC02
  SetOutPath "$INSTDIR\doc"
  File "..\..\lqns\lqns.pdf"
  File "..\..\lqsim\lqsim.pdf"
  File "..\..\srvndiff\srvndiff.pdf"
  File "..\..\lqn2ps\lqn2ps.pdf"
  File "..\..\lqngen\lqngen.pdf"
  File "..\..\lqngen\lqn2lqx.pdf"
  File "..\..\doc\tutorial\tutorial.pdf"
  File "..\..\doc\userman\userman.pdf"
SectionEnd

Section "Examples" SEC03
  SetOutPath "$INSTDIR\examples"
  File "..\..\dist\examples\cmpdesign.lqn"
  File "..\..\dist\examples\database.lqn"
  File "..\..\dist\examples\dbcase.lqn"
  File "..\..\dist\examples\dbcasea.lqn"
  File "..\..\dist\examples\dbcaseb.lqn"
  File "..\..\dist\examples\ex1-1.lqn"
  File "..\..\dist\examples\ex1-2.lqn"
  File "..\..\dist\examples\ex1-3.lqn"
  File "..\..\dist\examples\pipeline.lqn"
  File "..\..\dist\examples\test.lqn"
SectionEnd

Section "Release Notes" SEC04
  SetOutPath "$INSTDIR"
  File /oname=Release-Notes "..\..\ChangeLog"
  File "readme.txt"
SectionEnd

Section "Add to path"
  Push $INSTDIR
  Call AddToPath
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\lqns.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\lqns.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "Analytic and Simulation solvers, LQN converter and display program, Schema files."
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC04} "(Optional) Release notes."
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

!verbose 3
!include "WinMessages.NSH"
!verbose 4

; AddToPath - Adds the given dir to the search path.
;        Input - head of the stack
;        Note - Win9x systems requires reboot

Function AddToPath
  Exch $0
  Push $1
  Push $2
  Push $3

  # don't add if the path doesn't exist
  IfFileExists $0 "" AddToPath_done

  ReadEnvStr $1 PATH
  Push "$1;"
  Push "$0;"
  Call StrStr
  Pop $2
  StrCmp $2 "" "" AddToPath_done
  Push "$1;"
  Push "$0\;"
  Call StrStr
  Pop $2
  StrCmp $2 "" "" AddToPath_done
  GetFullPathName /SHORT $3 $0
  Push "$1;"
  Push "$3;"
  Call StrStr
  Pop $2
  StrCmp $2 "" "" AddToPath_done
  Push "$1;"
  Push "$3\;"
  Call StrStr
  Pop $2
  StrCmp $2 "" "" AddToPath_done

  Call IsNT
  Pop $1
  StrCmp $1 1 AddToPath_NT
    ; Not on NT
    StrCpy $1 $WINDIR 2
    FileOpen $1 "$1\autoexec.bat" a
    FileSeek $1 -1 END
    FileReadByte $1 $2
    IntCmp $2 26 0 +2 +2 # DOS EOF
      FileSeek $1 -1 END # write over EOF
    FileWrite $1 "$\r$\nSET PATH=%PATH%;$3$\r$\n"
    FileClose $1
    SetRebootFlag true
    Goto AddToPath_done

  AddToPath_NT:
    ReadRegStr $1 HKCU "Environment" "PATH"
    StrCpy $2 $1 1 -1 # copy last char
    StrCmp $2 ";" 0 +2 # if last char == ;
      StrCpy $1 $1 -1 # remove last char
    StrCmp $1 "" AddToPath_NTdoIt
      StrCpy $0 "$1;$0"
    AddToPath_NTdoIt:
      WriteRegExpandStr HKCU "Environment" "PATH" $0
      SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000

  AddToPath_done:
    Pop $3
    Pop $2
    Pop $1
    Pop $0
FunctionEnd

; RemoveFromPath - Remove a given dir from the path
;     Input: head of the stack

Function un.RemoveFromPath
  Exch $0
  Push $1
  Push $2
  Push $3
  Push $4
  Push $5
  Push $6

  IntFmt $6 "%c" 26 # DOS EOF

  Call un.IsNT
  Pop $1
  StrCmp $1 1 unRemoveFromPath_NT
    ; Not on NT
    StrCpy $1 $WINDIR 2
    FileOpen $1 "$1\autoexec.bat" r
    GetTempFileName $4
    FileOpen $2 $4 w
    GetFullPathName /SHORT $0 $0
    StrCpy $0 "SET PATH=%PATH%;$0"
    Goto unRemoveFromPath_dosLoop

    unRemoveFromPath_dosLoop:
      FileRead $1 $3
      StrCpy $5 $3 1 -1 # read last char
      StrCmp $5 $6 0 +2 # if DOS EOF
        StrCpy $3 $3 -1 # remove DOS EOF so we can compare
      StrCmp $3 "$0$\r$\n" unRemoveFromPath_dosLoopRemoveLine
      StrCmp $3 "$0$\n" unRemoveFromPath_dosLoopRemoveLine
      StrCmp $3 "$0" unRemoveFromPath_dosLoopRemoveLine
      StrCmp $3 "" unRemoveFromPath_dosLoopEnd
      FileWrite $2 $3
      Goto unRemoveFromPath_dosLoop
      unRemoveFromPath_dosLoopRemoveLine:
        SetRebootFlag true
        Goto unRemoveFromPath_dosLoop

    unRemoveFromPath_dosLoopEnd:
      FileClose $2
      FileClose $1
      StrCpy $1 $WINDIR 2
      Delete "$1\autoexec.bat"
      CopyFiles /SILENT $4 "$1\autoexec.bat"
      Delete $4
      Goto unRemoveFromPath_done

  unRemoveFromPath_NT:
    ReadRegStr $1 HKCU "Environment" "PATH"
    StrCpy $5 $1 1 -1 # copy last char
    StrCmp $5 ";" +2 # if last char != ;
      StrCpy $1 "$1;" # append ;
    Push $1
    Push "$0;"
    Call un.StrStr ; Find `$0;` in $1
    Pop $2 ; pos of our dir
    StrCmp $2 "" unRemoveFromPath_done
      ; else, it is in path
      # $0 - path to add
      # $1 - path var
      StrLen $3 "$0;"
      StrLen $4 $2
      StrCpy $5 $1 -$4 # $5 is now the part before the path to remove
      StrCpy $6 $2 "" $3 # $6 is now the part after the path to remove
      StrCpy $3 $5$6

      StrCpy $5 $3 1 -1 # copy last char
      StrCmp $5 ";" 0 +2 # if last char == ;
        StrCpy $3 $3 -1 # remove last char

      WriteRegExpandStr HKCU "Environment" "PATH" $3
      SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000

  unRemoveFromPath_done:
    Pop $6
    Pop $5
    Pop $4
    Pop $3
    Pop $2
    Pop $1
    Pop $0
FunctionEnd

###########################################
#            Utility Functions            #
###########################################

; IsNT
; no input
; output, top of the stack = 1 if NT or 0 if not
;
; Usage:
;   Call IsNT
;   Pop $R0
;  ($R0 at this point is 1 or 0)

!macro IsNT un
Function ${un}IsNT
  Push $0
  ReadRegStr $0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
  StrCmp $0 "" 0 IsNT_yes
  ; we are not NT.
  Pop $0
  Push 0
  Return

  IsNT_yes:
    ; NT!!!
    Pop $0
    Push 1
FunctionEnd
!macroend
!insertmacro IsNT ""
!insertmacro IsNT "un."

; StrStr
; input, top of stack = string to search for
;        top of stack-1 = string to search in
; output, top of stack (replaces with the portion of the string remaining)
; modifies no other variables.
;
; Usage:
;   Push "this is a long ass string"
;   Push "ass"
;   Call StrStr
;   Pop $R0
;  ($R0 at this point is "ass string")

!macro StrStr un
Function ${un}StrStr
Exch $R1 ; st=haystack,old$R1, $R1=needle
  Exch    ; st=old$R1,haystack
  Exch $R2 ; st=old$R1,old$R2, $R2=haystack
  Push $R3
  Push $R4
  Push $R5
  StrLen $R3 $R1
  StrCpy $R4 0
  ; $R1=needle
  ; $R2=haystack
  ; $R3=len(needle)
  ; $R4=cnt
  ; $R5=tmp
  loop:
    StrCpy $R5 $R2 $R3 $R4
    StrCmp $R5 $R1 done
    StrCmp $R5 "" done
    IntOp $R4 $R4 + 1
    Goto loop
done:
  StrCpy $R1 $R2 "" $R4
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Exch $R1
FunctionEnd
!macroend
!insertmacro StrStr ""
!insertmacro StrStr "un."

Section Uninstall
  Push $INSTDIR
  Call un.RemoveFromPath
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\readme.txt"
  Delete "$INSTDIR\Release-Notes"
  Delete "$INSTDIR\examples\test.lqn"
  Delete "$INSTDIR\examples\pipeline.lqn"
  Delete "$INSTDIR\examples\ex1-3.lqn"
  Delete "$INSTDIR\examples\ex1-2.lqn"
  Delete "$INSTDIR\examples\ex1-1.lqn"
  Delete "$INSTDIR\examples\dbcaseb.lqn"
  Delete "$INSTDIR\examples\dbcasea.lqn"
  Delete "$INSTDIR\examples\dbcase.lqn"
  Delete "$INSTDIR\examples\database.lqn"
  Delete "$INSTDIR\examples\cmpdesign.lqn"
  Delete "$INSTDIR\doc\lqn2ps.pdf"
  Delete "$INSTDIR\doc\lqxargs.pdf"
  Delete "$INSTDIR\doc\lqsim.pdf"
  Delete "$INSTDIR\doc\lqns.pdf"
  Delete "$INSTDIR\doc\tutorial.pdf"
  Delete "$INSTDIR\doc\userman.pdf"
  Delete "$INSTDIR\lqn2srvn.exe"
  Delete "$INSTDIR\srvndiff.exe"
  Delete "$INSTDIR\lqn2ps.exe"
  Delete "$INSTDIR\lqngen.exe"
  Delete "$INSTDIR\lqsim.exe"
  Delete "$INSTDIR\lqns.exe"

;  Delete "$DESKTOP\LQN Solvers.lnk"
;  Delete "$SMPROGRAMS\LQN Solvers\LQN Solvers.lnk"

  RMDir "$SMPROGRAMS\LQN Solvers"
  RMDir "$INSTDIR\examples"
  RMDir "$INSTDIR\doc"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd

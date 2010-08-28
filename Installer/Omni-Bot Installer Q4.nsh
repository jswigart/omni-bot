; Enemy Territory
Icon "Quake4.ico"
UninstallIcon "Quake4.ico"

!define GAME_ABBREV "Q4"
; ---------------------------------------------------------------
Function InstallBotGame
  SetOutPath "$GAME_DIR\omnibot"
  File "Files\q4\game\omnibot_q4.pk4"

  ; create shortcuts
  SetOutPath "$GAME_DIR"
  CreateDirectory "$SMPROGRAMS\Omni-Bot"
  CreateShortCut "$SMPROGRAMS\Omni-Bot\Omni-Bot Q4.lnk" "$GAME_DIR\Quake4.exe" "+set fs_game omnibot"
  CreateShortCut "$DESKTOP\Omni-Bot Q4.lnk" "$GAME_DIR\Quake4.exe" "+set fs_game omnibot"
FunctionEnd
; ---------------------------------------------------------------
Function un.InstallBotGame
  Delete "$GAME_DIR\omnibot\omnibot_q4.pk4"
  RMDir "$GAME_DIR\omnibot"

  ; remove shortcuts
  Delete "$SMPROGRAMS\Omni-Bot\Omni-Bot Q4.lnk"
  Delete "$DESKTOP\Omni-Bot Q4.lnk"
FunctionEnd
; ---------------------------------------------------------------
Function InstallBot
  SetOutPath "$INSTDIR"
  #File "Files\GMDebugger.exe"
  File "Files\omnibot_q4.dll"

  SetOutPath "$INSTDIR\q4\nav"
  File "Files\q4\nav\q4_official.zip"

  SetOutPath "$INSTDIR\q4\scripts"
  File "Files\q4\scripts\def_bot.gm"
  File "Files\q4\scripts\difficulty.gm"
  File "Files\q4\scripts\q4_autoexec.gm"
  File "Files\q4\scripts\q4_navconfig.gm"
  File "Files\q4\scripts\q4_utilities.gm"

  SetOutPath "$INSTDIR\q4\scripts\weapons"
  File "Files\q4\scripts\weapons\weapons.zip"
  
  SetOutPath "$INSTDIR\q4\user"
  File "Files\q4\user\omni-bot.cfg"

  Call InstallBotGame
FunctionEnd
; ---------------------------------------------------------------
Function un.InstallBot
  Delete "$INSTDIR\omnibot_et.dll"
  #Delete "$INSTDIR\GMDebugger.exe"

  Delete "$INSTDIR\q4\nav\q4_official.zip"

  Delete "$INSTDIR\q4\scripts\def_bot.gm"
  Delete "$INSTDIR\q4\scripts\difficulty.gm"
  Delete "$INSTDIR\q4\scripts\et_autoexec.gm"
  Delete "$INSTDIR\q4\scripts\et_navconfig.gm"
  Delete "$INSTDIR\q4\scripts\et_utilities.gm"

  Delete "$INSTDIR\q4\scripts\weapons\weapons.zip"
  
  Delete "$INSTDIR\q4\user\omni-bot.cfg"

  RMDir "$INSTDIR\q4\scripts\weapons"
  RMDir "$INSTDIR\q4\scripts"
  RMDir "$INSTDIR\q4\user"
  RMDir "$INSTDIR\q4\nav"
  RMDir "$INSTDIR\q4"

  Call un.InstallBotGame
FunctionEnd
; ---------------------------------------------------------------
Function InitInstallBot
  # if in silent mode, read directories from parameters
  IfSilent 0 default

    Push 'GAME_DIR'
    Call GetCmdLineParameter
    Pop $GAME_DIR
    StrCmp $GAME_DIR "" default alldone

  default:
  # set default value for GAME_DIR
  ClearErrors

  # Find the ET installation folder if it exists, or use the default.
  ReadRegStr $GAME_DIR HKLM "Software\Raven\Quake 4" "InstallPath"
  IfErrors 0 doneBot
    StrCpy $GAME_DIR "$PROGRAMFILES\Quake 4"
  doneBot:

  alldone:
  SetOverwrite on
FunctionEnd
; ---------------------------------------------------------------
Function un.InitInstallBot
  # if in silent mode, read directories from parameters
  IfSilent 0 default

    Push 'GAME_DIR'
    Call un.GetCmdLineParameter
    Pop $GAME_DIR
    StrCmp $GAME_DIR "" default alldone

  default:
  # set default value for GAME_DIR
  ClearErrors

  # Find the ET installation folder if it exists, or use the default.
  ReadRegStr $GAME_DIR HKLM "Software\Raven\Quake 4" "InstallPath"
  IfErrors 0 doneBot
    StrCpy $GAME_DIR "$PROGRAMFILES\Quake 4"
  doneBot:

  alldone:
FunctionEnd
; ---------------------------------------------------------------

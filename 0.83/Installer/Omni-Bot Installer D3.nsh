; Enemy Territory
Icon "Doom3.ico"
UninstallIcon "Doom3.ico"

!define GAME_ABBREV "D3"
; ---------------------------------------------------------------
Function InstallBotGame
  SetOutPath "$GAME_DIR\omnibot"
  File "Files\d3\game\omnibot_d3.pk4"

  ; create shortcuts
  SetOutPath "$GAME_DIR"
  CreateDirectory "$SMPROGRAMS\Omni-Bot"
  CreateShortCut "$SMPROGRAMS\Omni-Bot\Omni-Bot D3.lnk" "$GAME_DIR\Doom3.exe" "+set fs_game omnibot"
  CreateShortCut "$DESKTOP\Omni-Bot D3.lnk" "$GAME_DIR\Doom3.exe" "+set fs_game omnibot"
FunctionEnd
; ---------------------------------------------------------------
Function un.InstallBotGame
  Delete "$GAME_DIR\omnibot\omnibot_d3.pk4"
  RMDir "$GAME_DIR\omnibot"

  ; remove shortcuts
  Delete "$SMPROGRAMS\Omni-Bot\Omni-Bot D3.lnk"
  Delete "$DESKTOP\Omni-Bot D3.lnk"
FunctionEnd
; ---------------------------------------------------------------
Function InstallBot
  SetOutPath "$INSTDIR"
  #File "Files\GMDebugger.exe"
  File "Files\omnibot_d3.dll"

  SetOutPath "$INSTDIR\d3\nav"
  File "Files\d3\nav\d3_official.zip"

  SetOutPath "$INSTDIR\d3\scripts"
  File "Files\d3\scripts\def_bot.gm"
  File "Files\d3\scripts\difficulty.gm"
  File "Files\d3\scripts\d3_autoexec.gm"
  File "Files\d3\scripts\d3_navconfig.gm"
  File "Files\d3\scripts\d3_utilities.gm"

  SetOutPath "$INSTDIR\d3\scripts\weapons"
  File "Files\d3\scripts\weapons\weapons.zip"
  
  SetOutPath "$INSTDIR\d3\user"
  File "Files\d3\user\omni-bot.cfg"

  Call InstallBotGame
FunctionEnd
; ---------------------------------------------------------------
Function un.InstallBot
  Delete "$INSTDIR\omnibot_d3.dll"
  #Delete "$INSTDIR\GMDebugger.exe"

  Delete "$INSTDIR\d3\nav\d3_official.zip"

  Delete "$INSTDIR\d3\scripts\def_bot.gm"
  Delete "$INSTDIR\d3\scripts\difficulty.gm"
  Delete "$INSTDIR\d3\scripts\d3_autoexec.gm"
  Delete "$INSTDIR\d3\scripts\d3_navconfig.gm"
  Delete "$INSTDIR\d3\scripts\d3_utilities.gm"

  Delete "$INSTDIR\d3\scripts\weapons\weapons.zip"
  
  Delete "$INSTDIR\d3\user\omni-bot.cfg"

  RMDir "$INSTDIR\d3\scripts\weapons"
  RMDir "$INSTDIR\d3\scripts"
  RMDir "$INSTDIR\d3\user"
  RMDir "$INSTDIR\d3\nav"
  RMDir "$INSTDIR\d3"

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
  ReadRegStr $GAME_DIR HKLM "Software\id Software\Doom 3" "InstallPath"
  IfErrors 0 doneBot
    StrCpy $GAME_DIR "$PROGRAMFILES\Doom 3"
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
  ReadRegStr $GAME_DIR HKLM "Software\id Software\Doom 3" "InstallPath"
  IfErrors 0 doneBot
    StrCpy $GAME_DIR "$PROGRAMFILES\Doom 3"
  doneBot:

  alldone:
FunctionEnd
; ---------------------------------------------------------------

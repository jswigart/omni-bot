; Enemy Territory
Icon "ET.ico"
UninstallIcon "ET.ico"

!define GAME_ABBREV "ET"
; ---------------------------------------------------------------
Function InstallBotGame
  SetOutPath "$GAME_DIR\omnibot"
  File "Files\et\game\omnibot_et.pk3"
  File "Files\et\game\qagame_mp_x86.dll"

  ; create shortcuts
  SetOutPath "$GAME_DIR"
  CreateDirectory "$SMPROGRAMS\Omni-Bot"
  CreateShortCut "$SMPROGRAMS\Omni-Bot\Omni-Bot ET.lnk" "$GAME_DIR\ET.exe" "+set fs_game omnibot"
  CreateShortCut "$DESKTOP\Omni-Bot ET.lnk" "$GAME_DIR\ET.exe" "+set fs_game omnibot"
FunctionEnd
; ---------------------------------------------------------------
Function un.InstallBotGame
  Delete "$GAME_DIR\omnibot\qagame_mp_x86.dll"
  Delete "$GAME_DIR\omnibot\omnibot_et.pk3"
  RMDir "$GAME_DIR\omnibot"
  
  ; remove shortcuts
  Delete "$SMPROGRAMS\Omni-Bot\Omni-Bot ET.lnk"
  Delete "$DESKTOP\Omni-Bot ET.lnk"
FunctionEnd
; ---------------------------------------------------------------
Function InstallBot
  SetOutPath "$INSTDIR"
  #File "Files\GMDebugger.exe"
  File "Files\omnibot_et.dll"

  SetOutPath "$INSTDIR\et\nav"
  File "Files\et\nav\et_official.zip"
  File "Files\et\nav\et_custom.zip"

  SetOutPath "$INSTDIR\et\scripts"
  File "Files\et\scripts\def_bot.gm"
  File "Files\et\scripts\difficulty.gm"
  File "Files\et\scripts\et_autoexec.gm"
  File "Files\et\scripts\et_commands.gm"
  File "Files\et\scripts\et_navconfig.gm"
  File "Files\et\scripts\et_utilities.gm"

  SetOutPath "$INSTDIR\et\scripts\weapons"
  File "Files\et\scripts\weapons\weapons.zip"

  SetOutPath "$INSTDIR\et\scripts\goals"
  File "Files\et\scripts\goals\goals.zip"

  SetOutPath "$INSTDIR\et\scripts\testmap"
  File "Files\et\scripts\testmap\*.gm"
  
  SetOutPath "$INSTDIR\et\user"
  File "Files\et\user\omni-bot.cfg"
  
  Call InstallBotGame
FunctionEnd
; ---------------------------------------------------------------
Function un.InstallBot
  Delete "$INSTDIR\omnibot_et.dll"
  #Delete "$INSTDIR\GMDebugger.exe"
  
  Delete "$INSTDIR\et\nav\et_official.zip"
  Delete "$INSTDIR\et\nav\et_custom.zip"
  
  Delete "$INSTDIR\et\scripts\def_bot.gm"
  Delete "$INSTDIR\et\scripts\difficulty.gm"
  Delete "$INSTDIR\et\scripts\et_autoexec.gm"
  Delete "$INSTDIR\et\scripts\et_commands.gm"
  Delete "$INSTDIR\et\scripts\et_navconfig.gm"
  Delete "$INSTDIR\et\scripts\et_utilities.gm"
  
  Delete "$INSTDIR\et\scripts\weapons\weapons.zip"
  Delete "$INSTDIR\et\scripts\goals\goals.zip"
  Delete "$INSTDIR\et\scripts\testmap\*.gm"

  Delete "$INSTDIR\et\user\omni-bot.cfg"
  
  RMDir "$INSTDIR\et\scripts\testmap"
  RMDir "$INSTDIR\et\scripts\goals"
  RMDir "$INSTDIR\et\scripts\weapons"
  RMDir "$INSTDIR\et\scripts"
  RMDir "$INSTDIR\et\user"
  RMDir "$INSTDIR\et\nav"
  RMDir "$INSTDIR\et"

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
  ReadRegStr $GAME_DIR HKLM "Software\Activision\Wolfenstein - Enemy Territory" "InstallPath"
  IfErrors 0 doneBot
    StrCpy $GAME_DIR "$PROGRAMFILES\Wolfenstein - Enemy Territory"
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
  ReadRegStr $GAME_DIR HKLM "Software\Activision\Wolfenstein - Enemy Territory" "InstallPath"
  IfErrors 0 doneBot
    StrCpy $GAME_DIR "$PROGRAMFILES\Wolfenstein - Enemy Territory"
  doneBot:

  alldone:
FunctionEnd
; ---------------------------------------------------------------

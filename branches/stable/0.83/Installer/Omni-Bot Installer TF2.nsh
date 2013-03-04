; Enemy Territory
;Icon "TF2.ico"
;UninstallIcon "TF2.ico"

!define GAME_ABBREV "TF2"
; ---------------------------------------------------------------
Function InstallBotGame
  SetOutPath "$GAME_DIR\omnibot"
  File "Files\tf2\game\omnibot_tf2.pk3"
  File "Files\tf2\game\qagame_mp_x86.dll"

  ; create shortcuts
  SetOutPath "$GAME_DIR"
  CreateDirectory "$SMPROGRAMS\Omni-Bot"
  CreateShortCut "$SMPROGRAMS\Omni-Bot\Omni-Bot TF2.lnk" "$GAME_DIR\ET.exe" "+set fs_game omnibot"
  CreateShortCut "$DESKTOP\Omni-Bot TF2.lnk" "$GAME_DIR\ET.exe" "+set fs_game omnibot"
FunctionEnd
; ---------------------------------------------------------------
Function un.InstallBotGame
  Delete "$GAME_DIR\omnibot\qagame_mp_x86.dll"
  Delete "$GAME_DIR\omnibot\omnibot_tf2.pk3"
  RMDir "$GAME_DIR\omnibot"
  
  ; remove shortcuts
  Delete "$SMPROGRAMS\Omni-Bot\Omni-Bot TF2.lnk"
  Delete "$DESKTOP\Omni-Bot TF2.lnk"
FunctionEnd
; ---------------------------------------------------------------
Function InstallBot
  SetOutPath "$INSTDIR"
  #File "Files\GMDebugger.exe"
  File "Files\omnibot_tf2.dll"

  SetOutPath "$INSTDIR\tf2\nav"
  File "Files\tf2\nav\tf2_official.zip"
  File "Files\tf2\nav\tf2_custom.zip"

  SetOutPath "$INSTDIR\tf2\scripts"
  File "Files\tf2\scripts\def_bot.gm"
  File "Files\tf2\scripts\difficulty.gm"
  File "Files\tf2\scripts\tf2_autoexec.gm"
  File "Files\tf2\scripts\tf2_commands.gm"
  File "Files\tf2\scripts\tf2_navconfig.gm"
  File "Files\tf2\scripts\tf2_utilities.gm"
  File "Files\tf2\scripts\sample.gm"

  SetOutPath "$INSTDIR\tf2\scripts\weapons"
  File "Files\tf2\scripts\weapons\weapons.zip"
  
  SetOutPath "$INSTDIR\tf2\user"
  File "Files\tf2\user\omni-bot.cfg"
  
  Call InstallBotGame
FunctionEnd
; ---------------------------------------------------------------
Function un.InstallBot
  Delete "$INSTDIR\omnibot_tf2.dll"
  #Delete "$INSTDIR\GMDebugger.exe"
  
  Delete "$INSTDIR\tf2\nav\tf2_official.zip"
  Delete "$INSTDIR\tf2\nav\tf2_custom.zip"
  
  Delete "$INSTDIR\tf2\scripts\def_bot.gm"
  Delete "$INSTDIR\tf2\scripts\difficulty.gm"
  Delete "$INSTDIR\tf2\scripts\tf2_autoexec.gm"
  Delete "$INSTDIR\tf2\scripts\tf2_commands.gm"
  Delete "$INSTDIR\tf2\scripts\tf2_navconfig.gm"
  Delete "$INSTDIR\tf2\scripts\tf2_utilities.gm"
  Delete "$INSTDIR\tf2\scripts\sample.gm"
  
  Delete "$INSTDIR\tf2\scripts\weapons\weapons.zip"
  
  Delete "$INSTDIR\tf2\user\omni-bot.cfg"

  RMDir "$INSTDIR\tf2\scripts\weapons"
  RMDir "$INSTDIR\tf2\scripts"
  RMDir "$INSTDIR\tf2\user"
  RMDir "$INSTDIR\tf2\nav"
  RMDir "$INSTDIR\tf2"

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

  # Find the TF2 installation folder if it exists, or use the default.
  ReadRegStr $GAME_DIR HKCU "Software\Valve\Steam" "ModInstallPath"
  IfErrors 0 doneBot
    StrCpy $GAME_DIR ""
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
  ReadRegStr $GAME_DIR HKCU "Software\Valve\Steam" "ModInstallPath"
  IfErrors 0 doneBot
    StrCpy $GAME_DIR ""
  doneBot:

  alldone:
FunctionEnd
; ---------------------------------------------------------------

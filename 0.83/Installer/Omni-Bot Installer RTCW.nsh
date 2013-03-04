; RTCW directory page
Icon "RTCW.ico"
UninstallIcon "RTCW.ico"

!define GAME_ABBREV "RTCW"
; ---------------------------------------------------------------
Function InstallBotGame
  SetOutPath "$GAME_DIR\omnibot"
  File "Files\rtcw\game\omnibot_rtcw.pk3"
  File "Files\rtcw\game\ob_media.pk3"
  File "Files\rtcw\game\qagame_mp_x86.dll"

  ; create shortcuts
  SetOutPath "$GAME_DIR"
  CreateDirectory "$SMPROGRAMS\Omni-Bot"
  CreateShortCut "$SMPROGRAMS\Omni-Bot\Omni-Bot RTCW.lnk" "$GAME_DIR\WolfMp.exe" "+set fs_game omnibot"
  CreateShortCut "$DESKTOP\Omni-Bot RTCW.lnk" "$GAME_DIR\WolfMp.exe" "+set fs_game omnibot"
FunctionEnd
; ---------------------------------------------------------------
Function un.InstallBotGame
  Delete "$GAME_DIR\omnibot\qagame_mp_x86.dll"
  Delete "$GAME_DIR\omnibot\omnibot_rtcw.pk3"
  Delete "$GAME_DIR\omnibot\ob_media.pk3"
  RMDir "$GAME_DIR\omnibot"
  
  ; remove shortcuts
  Delete "$SMPROGRAMS\Omni-Bot\Omni-Bot RTCW.lnk"
  Delete "$DESKTOP\Omni-Bot RTCW.lnk"
FunctionEnd
; ---------------------------------------------------------------
Function InstallBot
  SetOutPath "$INSTDIR"
  #File "Files\GMDebugger.exe"
  File "Files\omnibot_rtcw.dll"

  SetOutPath "$INSTDIR\rtcw\nav"
  File "Files\rtcw\nav\rtcw_official.zip"
  File "Files\rtcw\nav\rtcw_custom.zip"

  SetOutPath "$INSTDIR\rtcw\scripts"
  File "Files\rtcw\scripts\def_bot.gm"
  File "Files\rtcw\scripts\difficulty.gm"
  File "Files\rtcw\scripts\rtcw_autoexec.gm"
  File "Files\rtcw\scripts\rtcw_commands.gm"
  File "Files\rtcw\scripts\rtcw_navconfig.gm"
  File "Files\rtcw\scripts\rtcw_utilities.gm"
  File "Files\rtcw\scripts\dumpstats.gm"

  SetOutPath "$INSTDIR\rtcw\scripts\weapons"
  File "Files\rtcw\scripts\weapons\weapons.zip"

  SetOutPath "$INSTDIR\rtcw\scripts\goals"
  File "Files\rtcw\scripts\goals\goals.zip"

  SetOutPath "$INSTDIR\rtcw\scripts\testmap"
  File "Files\rtcw\scripts\testmap\*.gm"
  
  SetOutPath "$INSTDIR\rtcw\user"
  File "Files\rtcw\user\omni-bot.cfg"
  
  Call InstallBotGame
FunctionEnd
; ---------------------------------------------------------------
Function un.InstallBot
  Delete "$INSTDIR\omnibot_rtcw.dll"
  #Delete "$INSTDIR\GMDebugger.exe"
  
  Delete "$INSTDIR\rtcw\nav\rtcw_official.zip"
  Delete "$INSTDIR\rtcw\nav\rtcw_custom.zip"
  
  Delete "$INSTDIR\rtcw\scripts\def_bot.gm"
  Delete "$INSTDIR\rtcw\scripts\difficulty.gm"
  Delete "$INSTDIR\rtcw\scripts\rtcw_autoexec.gm"
  Delete "$INSTDIR\rtcw\scripts\rtcw_commands.gm"
  Delete "$INSTDIR\rtcw\scripts\rtcw_navconfig.gm"
  Delete "$INSTDIR\rtcw\scripts\rtcw_utilities.gm"
  Delete "$INSTDIR\rtcw\scripts\dumpstats.gm"
  
  Delete "$INSTDIR\rtcw\scripts\weapons\weapons.zip"
  Delete "$INSTDIR\rtcw\scripts\goals\goals.zip"
  Delete "$INSTDIR\rtcw\scripts\testmap\*.gm"
  
  Delete "$INSTDIR\rtcw\user\omni-bot.cfg"

  RMDir "$INSTDIR\rtcw\scripts\testmap"
  RMDir "$INSTDIR\rtcw\scripts\goals"
  RMDir "$INSTDIR\rtcw\scripts\weapons"
  RMDir "$INSTDIR\rtcw\scripts"
  RMDir "$INSTDIR\rtcw\user"
  RMDir "$INSTDIR\rtcw\nav"
  RMDir "$INSTDIR\rtcw"

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

  # Find the RTCW installation folder if it exists, or use the default.
  ReadRegStr $GAME_DIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Return to Castle Wolfenstein" "DisplayIcon"
  IfErrors 0 doneBot
    StrCpy $GAME_DIR "$PROGRAMFILES\Return to Castle Wolfenstein"
  doneBot:
          ${StrStrip} "WolfMP.exe,-0" $GAME_DIR $GAME_DIR
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

  # Find the RTCW installation folder if it exists, or use the default.
  ReadRegStr $GAME_DIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Return to Castle Wolfenstein" "DisplayIcon"
  IfErrors 0 doneBot
    StrCpy $GAME_DIR "$PROGRAMFILES\Return to Castle Wolfenstein"
  doneBot:
          ${UnStrStrip} "WolfMP.exe,-0" $GAME_DIR $GAME_DIR
  alldone:
FunctionEnd
; ---------------------------------------------------------------


Function InstallGlobalScripts
  SetOutPath "$INSTDIR\global_scripts"
  
  File "Files\global_scripts\commands.gm"
  File "Files\global_scripts\makegm.gm"
  File "Files\global_scripts\server_manager.gm"
  File "Files\global_scripts\testbot.gm"
  File "Files\global_scripts\testdebugwindows.gm"
  File "Files\global_scripts\testfilesystem.gm"
  File "Files\global_scripts\testmap.gm"
  File "Files\global_scripts\testmath.gm"
  File "Files\global_scripts\testmisc.gm"
  File "Files\global_scripts\testrequire.gm"
  File "Files\global_scripts\utilities.gm"

  SetOutPath "$INSTDIR\global_scripts\goals"
  File "Files\global_scripts\goals\goal_testbot.gm"

  SetOutPath "$INSTDIR\global_scripts\weapons"
  File "Files\global_scripts\weapons\weapon_sample.gm"
  
  SetOutPath "$INSTDIR\gui"
  File "Files\gui\default_font.bmp"
  
  SetOutPath "$INSTDIR"
  File "Files\credits.txt"
FunctionEnd

Function un.InstallGlobalScripts

  Delete "$INSTDIR\global_scripts\commands.gm"
  Delete "$INSTDIR\global_scripts\makegm.gm"
  Delete "$INSTDIR\global_scripts\server_manager.gm"
  Delete "$INSTDIR\global_scripts\testbot.gm"
  Delete "$INSTDIR\global_scripts\testdebugwindows.gm"
  Delete "$INSTDIR\global_scripts\testfilesystem.gm"
  Delete "$INSTDIR\global_scripts\testmap.gm"
  Delete "$INSTDIR\global_scripts\testmath.gm"
  Delete "$INSTDIR\global_scripts\testmisc.gm"
  Delete "$INSTDIR\global_scripts\testrequire.gm"
  Delete "$INSTDIR\global_scripts\utilities.gm"

  Delete "$INSTDIR\global_scripts\goals\goal_testbot.gm"
  Delete "$INSTDIR\global_scripts\weapons\weapon_sample.gm"
  
  Delete "$INSTDIR\gui\default_font.bmp"
  
  Delete "$INSTDIR\credits.txt"
  
  RMDir "$INSTDIR\gui"
  RMDir "$INSTDIR\global_scripts\goals"
  RMDir "$INSTDIR\global_scripts\weapons"
  RMDir "$INSTDIR\global_scripts"
FunctionEnd
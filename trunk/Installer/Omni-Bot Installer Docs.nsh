
Function InstallDocs
  SetOutPath "$INSTDIR\docs"
  File "Files\docs\Omni-bot Wiki.pdf"
  CreateShortCut "$SMPROGRAMS\Omni-Bot\Documentation.lnk" "$INSTDIR\docs\"
FunctionEnd

Function un.InstallDocs
  Delete "$SMPROGRAMS\Omni-Bot\Documentation.lnk"
  Delete "$INSTDIR\docs\Omni-bot Wiki.pdf"
  RMDir "$INSTDIR\docs"
FunctionEnd

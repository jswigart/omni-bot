Section "Add to path"
  Push $INSTDIR
  Call AddToPath
SectionEnd

# ...

Section "uninstall"
  # ...
  Push $INSTDIR
  Call un.RemoveFromPath
  # ...
SectionEnd
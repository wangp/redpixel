;; NSI script for Red Pixel, based on example2.nsi from NSIS distr.
;; Note to self: move this to parent directory (i.e. root of redpixel)
;; then right-click to make installer (need NSIS installed, of course :)

Name "Red Pixel"
OutFile "redinst.exe"

InstallDir $PROGRAMFILES\RedPixel
;; Registry key to check for directory (so if you install again, it will 
;; overwrite the old one automatically)
InstallDirRegKey HKLM SOFTWARE\Psyk_Redpixel "Install_Dir"

ComponentText "This will install Red Pixel on your computer."
DirText "Choose a directory to install in to:"


;;; Install.

Section "Red Pixel"
	SetOutPath $INSTDIR
	File "redwin.exe"
	;File "reddos.exe"
	File "blood.dat" "LICENCE.txt"
	File /r demos docs maps music stats
	;; Write the installation path into the registry
	WriteRegStr HKLM SOFTWARE\Psyk_Redpixel "Install_Dir" "$INSTDIR"
	;; Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\RedPixel" "DisplayName" "Red Pixel (remove only)"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\RedPixel" "UninstallString" '"$INSTDIR\uninstall.exe"'
SectionEnd

;; Optional section.
Section "Start Menu Shortcuts"
	CreateDirectory "$SMPROGRAMS\Red Pixel"
	CreateShortCut "$SMPROGRAMS\Red Pixel\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
	CreateShortCut "$SMPROGRAMS\Red Pixel\Red Pixel.lnk" "$INSTDIR\redwin.exe" "" "$INSTDIR\redwin.exe" 0
	;CreateShortCut "$SMPROGRAMS\Red Pixel\Red Pixel DOS.lnk" "$INSTDIR\reddos.exe" "" "$INSTDIR\reddos.exe" 0
SectionEnd


;;; Uninstall.

UninstallText "This will uninstall Red Pixel.  Warning: if you have any demos or other files of your own, back them up now or they may be removed."
UninstallExeName "uninstall.exe"

Section "Uninstall"
	;; Remove registry keys.
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\RedPixel"
	DeleteRegKey HKLM SOFTWARE\Psyk_Redpixel
	;; Remove files.
	Delete $INSTDIR\redwin.exe
	; Delete $INSTDIR\reddos.exe
	Delete $INSTDIR\blood.dat
	Delete $INSTDIR\LICENCE.txt
	RMDir /r $INSTDIR\demos
	RMDir /r $INSTDIR\docs
	RMDir /r $INSTDIR\maps
	RMDir /r $INSTDIR\music
	RMDir /r $INSTDIR\stats
	Delete $INSTDIR\uninstall.exe
	;; Remove shortcuts, if any.
	Delete "$SMPROGRAMS\Red Pixel\*.*"
	;; Remove directories used.
	RMDir "$SMPROGRAMS\Red Pixel"
	RMDir "$INSTDIR"
SectionEnd

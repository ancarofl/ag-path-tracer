del *.ncb
del *.ilk
del *.sdf
del *.exp
del *.map
del *.pdb
del Upgradelog.xml
del *.exe
del *.bsc
del applog.txt
del *.suo /AH
del *.user
del *.o
del *.VC.db
rd ipch /S /Q
rd release /S /Q
rd debug /S /Q
rd .vs /S /Q

REM Delete all output files, except precompiled DLL's:
for %%D in (x64 Win32) do @for /f %%F in ('dir /s /b %%D ^| findstr /vile ".dll"') do del "%%F" /Q

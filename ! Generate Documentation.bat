@echo off
del Doxyout.log


echo ---------------------PREP--------------------- >> Doxyout.log
::: physically remove old files (marked Missing) later replacements of same name will get be marked Modified
del .\documentation\html\*.html
del .\documentation\html\*.js
del .\documentation\html\*.css
del .\documentation\html\*.png
del .\documentation\html\*.jpg


echo ---------------------DOXY--------------------- >> Doxyout.log
doxygen.exe Doxyfile.ini >> Doxyout.log


echo ---------------------CLEANUP------------------ >> Doxyout.log
::: okay so, we Deleted the old files (still in WC history, marked all missing)
::: then created new files (any matching files from before are now marked "Modified" instead of "Missing")
::: so now we have phantom-files (Missing) from the past, New files (?Not Versioned), and matching-name files (Modified)

::: ... then add any files that aren't already under version control (would be Modified if it were)
::svn add --force documentation
echo ---------------------CLEANUP3------------------ >> Doxyout.log


::: ... we need to remove the "Missing" files so the next revision doesn't have useless files.
cd documentation
::svnremovemissing.bat

echo ---------------------CLEANUP2------------------ >> Doxyout.log



pause
exit
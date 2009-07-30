call %QTDIR%/bin/qtvars.bat

lupdate -noobsolete src/pdf -ts bin/locale/plutopdf_en.ts
lupdate -noobsolete src/pdf -ts bin/locale/plutopdf_zh_CN.ts

pause
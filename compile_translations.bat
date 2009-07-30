call %QTDIR%/bin/qtvars.bat

lrelease -compress -nounfinished bin/locale/plutopdf_en.ts -qm bin/locale/plutopdf_en.qm
lrelease -compress -nounfinished bin/locale/plutopdf_zh_CN.ts -qm bin/locale/plutopdf_zh_CN.qm

pause
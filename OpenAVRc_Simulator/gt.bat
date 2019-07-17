md po

Path "C:\OpenAVRc_Builder\G_TXT"

for /r %%a in ("*.cpp") do echo %%a >> filelist
for /r %%a in ("*.h") do echo %%a >> filelist

xgettext -s --from-code=CP1252 --keyword=_ -p ./po -o Export.pot -f filelist

del filelist

pause

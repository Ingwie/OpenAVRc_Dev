REM Clean Directory

rmdir /S /Q VOICEMP3
rmdir /S /Q VOICEAD4

REM Normalise and cut silence

for /r %%a in ("*.wav") do CALL tools\sox --norm=-1 %%a %%~na.aif silence 1 0.1 0.1% reverse highpass 300
REM Cut silence
for /r %%a in ("*.aif") do CALL tools\sox %%a %%~na.wav silence 1 0.1 0.01% reverse pad 0.0 0.1 
del *.aif /s

REM Convert to mp3 (JQ6500)

for %%a in ("*.wav") do CALL tools\lame -q0 --abr 256 %%a %%~na.mp3
md VOICEMP3
move /y *.mp3 VOICEMP3

REM Convert to ADPCM 4 bits (WTV20)

for %%a in ("*.wav") do CALL tools\AD4CONVERTER -E4 %%a
md VOICEAD4
move /y *.AD4 VOICEAD4

REM Clean

del *.wav /s

REM Splash screen finished !

echo off
cls
echo ********************************************
echo ********************************************
echo                 OpenAVRc
echo ********************************************
echo ********************************************
echo LES FICHIERS SONT DANS LES REPERTOIRES VOICE
echo ********************************************
echo ********************************************
pause
:end

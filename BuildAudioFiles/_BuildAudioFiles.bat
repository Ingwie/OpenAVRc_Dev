start "NEXTSTEPRC" /wait OpenTXspeaker.exe
REM Normalise and cut silence
for /r %%a in ("*.wav") do CALL SOUNDS\tools\sox --norm=-1 %%a %%~na.aif silence 1 0.1 0.1% reverse
REM Cut silence
for /r %%a in ("*.aif") do CALL SOUNDS\tools\sox %%a %%~na.wav silence 1 0.1 0.1% reverse 
del *.aif /s
REM Convert to mp3
for %%a in ("*.wav") do CALL SOUNDS\tools\lame -q0 --highpass 0.2 --abr 256 %%a %%~na.mp3
md VOICEMP3
move /y *.mp3 VOICEMP3
rem del *.mp3 /s
REM Convert to ADPCM 4 bits
for %%a in ("*.wav") do CALL SOUNDS\tools\AD4CONVERTER -E4 %%a
md VOICEAD4
move /y *.AD4 VOICEAD4
del *.wav /s
echo off
cls
echo ********************************************
echo ********************************************
echo                 NEXSTEPRC
echo ********************************************
echo ********************************************
echo LES FICHIERS SONT DANS LES REPERTOIRES VOICE
echo ********************************************
echo ********************************************
pause
:end

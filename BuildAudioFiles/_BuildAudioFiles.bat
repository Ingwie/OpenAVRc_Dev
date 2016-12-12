
rmdir /S /Q VOICEMP3
rmdir /S /Q VOICEAD4

REM Normalise and cut silence
for /r %%a in ("*.wav") do CALL C:\OpenAVRc\BuildAudioFiles\SOUNDS\tools\sox --norm=-1 %%a %%~na.aif silence 1 0.1 0.1% reverse highpass 300
REM Cut silence
REM pause
for /r %%a in ("*.aif") do CALL C:\OpenAVRc\BuildAudioFiles\SOUNDS\tools\sox %%a %%~na.wav silence 1 0.1 1% reverse pad 0.0 0.1 
del *.aif /s
REM pause
REM Convert to mp3
for %%a in ("*.wav") do CALL C:\OpenAVRc\BuildAudioFiles\SOUNDS\tools\lame -q0 --abr 256 %%a %%~na.mp3
md VOICEMP3
move /y *.mp3 VOICEMP3
rem del *.mp3 /s
REM Convert to ADPCM 4 bits
REM REM for %%a in ("*.wav") do CALL SOUNDS\tools\AD4CONVERTER -E4 %%a
REM REM md VOICEAD4
REM REM move /y *.AD4 VOICEAD4
del *.wav /s
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

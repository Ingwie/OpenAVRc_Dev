Path C:\OpenAVRc\AVR-GCC\bin;C:\OpenAVRc\AVR-GCC\utils\bin
avr-size -A OpenAVRc.elf
avr-size -B OpenAVRc.elf
pause
avr-nm --size-sort OpenAVRc.elf>AnalyseMem.txt
call AnalyseMem.txt



Path C:\NextStepRc\AVR-GCC\bin;C:\NextStepRc\AVR-GCC\utils\bin
avr-size -A nextsteprc.elf
avr-size -B nextsteprc.elf
pause
avr-nm --size-sort nextsteprc.elf>AnalyseMem.txt
call AnalyseMem.txt



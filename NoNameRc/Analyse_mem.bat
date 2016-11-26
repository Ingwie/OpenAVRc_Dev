Path C:\NoNameRc\AVR-GCC\bin;C:\NoNameRc\AVR-GCC\utils\bin
avr-size -A NoNameRc.elf
avr-size -B NoNameRc.elf
pause
avr-nm --size-sort NoNameRc.elf>AnalyseMem.txt
call AnalyseMem.txt



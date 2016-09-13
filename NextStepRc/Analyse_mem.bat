avr-size -A nextsteprc.elf
avr-size -B nextsteprc.elf
pause
avr-nm --size-sort nextsteprc.elf>AnalyseMem.txt
call AnalyseMem.txt



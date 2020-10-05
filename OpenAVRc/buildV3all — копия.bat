Path C:\OpenAVRc_Builder\AVR-GCC\bin;C:\OpenAVRc_Builder\AVR-GCC\utils\bin;C:\OpenAVRc_Builder\Python34

make clean
make 
C:\OpenAVRcV3.00\avrdude\avrdude.exe -c stk500 -p m2560 -D -P COM10 -V -U flash:w:D:\OpenAVRc_Dev\OpenAVRc\OpenAVRc.hex:i
pause
avrdude -p m2560 -c usbasp -P usb -F -e -u -Ulfuse:w:0xFF:m -Uhfuse:w:0xD8:m -Uefuse:w:0xFD:m -v
pause
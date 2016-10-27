avrdude -p m2560 -c usbasp -P usb -U lock:w:0x3F:m -U flash:w:mega2560_stk500v2boot_opentx.hex -U lock:w:0x0F:m
pause
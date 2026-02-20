#!/bin/bash

echo "Compile AVR1009bootloader"
make clean MCU=atmega2560

cp conf/m2560fram.conf.mk  config.mk
make  MCU=atmega2560  LCD=KS108
mv xboot.hex openavrc_bootloader_avr109_m2560_KS108.hex
make clean MCU=atmega2560

cp conf/m2560fram.conf.mk  config.mk
make  MCU=atmega2560  LCD=ST7565P
mv xboot.hex openavrc_bootloader_avr109_m2560_ST7565P.hex
make clean MCU=atmega2560

cp conf/m2560fram.conf.mk  config.mk
make  MCU=atmega2560  LCD=ST7565R
mv xboot.hex openavrc_bootloader_avr109_m2560_ST7565R.hex
make clean MCU=atmega2560

cp conf/m2560fram.conf.mk  config.mk
make  MCU=atmega2560  LCD=ERC12864FSF
mv xboot.hex openavrc_bootloader_avr109_m2560_ERC12864FSF.hex
make clean MCU=atmega2560

cp conf/m2560fram.conf.mk  config.mk
make  MCU=atmega2560  LCD=ST7920
mv xboot.hex openavrc_bootloader_avr109_m2560_ST7920.hex
make clean MCU=atmega2560

cp conf/m2560fram.conf.mk  config.mk
make  MCU=atmega2560  LCD=SSD1306
mv xboot.hex openavrc_bootloader_avr109_m2560_SSD1306.hex
make clean MCU=atmega2560

cp conf/m2560fram.conf.mk  config.mk
make  MCU=atmega2560  LCD=SH1106
mv xboot.hex openavrc_bootloader_avr109_m2560_SH1106.hex
make clean MCU=atmega2560

echo "Done !"


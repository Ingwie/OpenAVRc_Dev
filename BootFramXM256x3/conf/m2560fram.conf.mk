# XBoot Configuration for (Arduino) Mega 2560 core.

# use config.h
USE_CONFIG_H = yes

USE_FRAM_EE = yes

# MCU
MCU = atmega2560

# Select boot size
# Note: if boot size is too small, XBoot may not fit.
# Generally, it should be left on largest
# See part datasheet for specific values
# Largest
BOOTSZ=0
# Large
#BOOTSZ=1
# Medium
#BOOTSZ=2
# Small
#BOOTSZ=3

# Clock Speed
# Use 16 MHz external crystal oscillator
F_CPU = 16000000



# Programmer settings
OVERRIDE_AVRDUDE_PROGRAMMER = yes
AVRDUDE_PROGRAMMER = avrisp2
AVRDUDE_PORT = usb

# Fuse settings
# If you wish to override the default fuse settings
# determined in main Makefile, change them here
# and then uncomment OVERRIDE_AVRDUDE_FUSES

AVRDUDE_FUSES += -U lfuse:w:0xff:m
AVRDUDE_FUSES += -U hfuse:w:0xd0:m
AVRDUDE_FUSES += -U efuse:w:0xfc:m
#AVRDUDE_FUSES += -U lock:w:0xFF:m

# Uncomment to override default fuse configurations
# from main Makefile
OVERRIDE_AVRDUDE_FUSES = yes

# XBoot settings

# Entry
USE_ENTER_DELAY = yes
USE_ENTER_PIN = yes
USE_ENTER_UART = no
USE_ENTER_I2C = no
USE_ENTER_FIFO = no

# Exit
LOCK_SPM_ON_EXIT = no

# Communication
USE_LED = yes
USE_UART = yes
USE_UART_EN_PIN = no
USE_I2C = no
USE_I2C_ADDRESS_NEGOTIATION = no
USE_ATTACH_LED = no
USE_FIFO = no

# General Options
USE_INTERRUPTS = no
USE_WATCHDOG = no

# Bootloader Features
ENABLE_BLOCK_SUPPORT = yes
ENABLE_FLASH_BYTE_SUPPORT = yes
ENABLE_EEPROM_BYTE_SUPPORT = no
ENABLE_LOCK_BITS = yes
ENABLE_FUSE_BITS = yes
ENABLE_FLASH_ERASE_WRITE = yes
ENABLE_CRC_SUPPORT = yes

# API
ENABLE_API = no
USE_API_VERSION =
ENABLE_API_LOW_LEVEL_FLASH =
ENABLE_API_SPM_WRAPPER =
ENABLE_API_FIRMWARE_UPDATE =

# Code Protection
ENABLE_CODE_PROTECTION = no
ENABLE_EEPROM_PROTECTION = no
ENABLE_BOOTLOADER_PROTECTION = no

# ENTER_PIN
# Left hand stick - horzontal right trim button.
ENTER_PORT_NAME       = F
ENTER_PIN             = 6
ENTER_PIN_STATE       = 0
ENTER_PIN_PUEN        = yes

# ENTER_DELAY
ENTER_BLINK_COUNT     = 3
ENTER_BLINK_WAIT      = 50000

# ENTER_UART
ENTER_UART_NEED_SYNC = yes

# USE_WATCHDOG
# Select only one
#WATCHDOG_TIMEOUT      = WDT_PER_8CLK_gc
#WATCHDOG_TIMEOUT      = WDT_PER_16CLK_gc
#WATCHDOG_TIMEOUT      = WDT_PER_32CLK_gc
#WATCHDOG_TIMEOUT      = WDT_PER_64CLK_gc
#WATCHDOG_TIMEOUT      = WDT_PER_128CLK_gc
#WATCHDOG_TIMEOUT      = WDT_PER_256CLK_gc
#WATCHDOG_TIMEOUT      = WDT_PER_512CLK_gc
#WATCHDOG_TIMEOUT      = WDT_PER_1KCLK_gc
#WATCHDOG_TIMEOUT      = WDT_PER_2KCLK_gc
#WATCHDOG_TIMEOUT      = WDT_PER_4KCLK_gc
#WATCHDOG_TIMEOUT      = WDT_PER_8KCLK_gc

# LED
LED_PORT_NAME         = B
LED_PIN               = 7
LED_INV               = 0

# UART
# Select BAUD rate and UART number
# For ATMEGA, uart number is usually 0
# UART_U2X will double clock rate for ATMEGA
# Needed for high baud rates
UART_BAUD_RATE        = 115200
UART_NUMBER           = 0
UART_U2X              = yes
UART_RX_PUEN          = yes
UART_REMAP            = no


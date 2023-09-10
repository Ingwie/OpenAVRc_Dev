# xmega32e5 configuration

# use config.h
USE_CONFIG_H = yes

# MCU
MCU = atxmega32e5

# Clock Speed
# Use 2 MHz or 32 MHz internal RC oscillator
F_CPU = 32000000

# DFLL for better stability
USE_DFLL = no

# Programmer settings
OVERRIDE_AVRDUDE_PROGRAMMER = yes
AVRDUDE_PROGRAMMER = avrispmkii
AVRDUDE_PORT = usb

# Fuse settings

# If you wish to override the default fuse settings
# determined in main Makefile, change them here
# and then uncomment OVERRIDE_AVRDUDE_FUSES
# See XMega A series datasheet (Atmel doc8077) section 4.16

# Fuse byte 0: JTAG User ID
# If a custom JTAG User ID is required, uncomment
# and set it here
# AVRDUDE_FUSES += -U fuse0:w:0x00:m

# Fuse byte 1: Watchdog
# Set WDPER and WDWPER
# See datasheet sections 4.16.2, 11.7.1, and 11.7.2
# for more information
AVRDUDE_FUSES += -U fuse1:w:0x00:m

# Fuse byte 2: Reset configuration
# Spike detector, reset vector location, and BOD
# in power down configuration
# See datasheet section 4.16.3 for more information
AVRDUDE_FUSES += -U fuse2:w:0xBF:m

# There is no fuse byte 3.....

# Fuse byte 4: Start-up configuration
# See datasheet section 4.16.4
# Configures external reset disable, start-up time,
# watchdog timer lock, and jtag enable
AVRDUDE_FUSES += -U fuse4:w:0xFF:m

# Fuse byte 5
# See datasheet section 4.16.5
# Configures BOD operation in active mode,
# EEPROM preserved through chip erase, and
# BOD detection level
AVRDUDE_FUSES += -U fuse5:w:0xF0:m

# Fuse byte 6
# See datasheet section 4.16.5
# Configures BOD operation in active mode,
# EEPROM preserved through chip erase, and
# BOD detection level
# AVRdude.conf may need to edited to be made aware of fuse6
AVRDUDE_FUSES += -U fuse6:w:0xFF:m

# Lock byte
# See datasheet section 4.16.6
# Lock bits for boot loader, application,
# and application table sections via internal
# SPM commands and external programming interface

# lockbits - must have BOTH bits 0 and 1 SET or you'll have to chip erase to get them back
# bit assignments:  11 = NOLOCK, 10 = WLOCK, 01 = RLOCK, 00 = RWLOCK (typically)
# bit 7:6 bootloader   bit 5:4 application   bit 3:2 app table   bit 1:0  programming interface.
# To ONLY lock the bootloader, set LOCKBITS to 0xBF (bit 6 clear, all others set)
#AVRDUDE_FUSES += -U lock:w:0xBF:m

# Write user sig row (256 bytes max)
# Uncomment to initialize user sig row with custom data
##AVRDUDE_USERSIG = -U usersig:w:0x01,0x02,0x03:m
##AVRDUDE_USERSIG = -U usersig:w:filename
#AVRDUDE_USERSIG = -U usersig:w:...:m

# Uncomment to override default fuse configurations
# from main Makefile
OVERRIDE_AVRDUDE_FUSES = yes

# XBoot settings

# AVR1008 fixes
# Really only applicable to XMEGA 256a3 rev A and B devices
USE_AVR1008_EEPROM = no

# Entry
USE_ENTER_DELAY = yes
USE_ENTER_PIN = no
USE_ENTER_UART = yes
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
ENABLE_EEPROM_BYTE_SUPPORT = yes
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
ENTER_PORT_NAME       = 
ENTER_PIN             = 
ENTER_PIN_STATE       = 
ENTER_PIN_PUEN        = 

# ENTER_DELAY
ENTER_BLINK_COUNT     = 3
ENTER_BLINK_WAIT      = 100000

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
LED_PORT_NAME         = D
LED_PIN               = 5
LED_INV               = 0

# UART
# Select BAUD rate, port name, and UART number
UART_BAUD_RATE        = 115200
UART_PORT_NAME        = D
UART_NUMBER           = 0
UART_RX_PUEN          = yes
UART_REMAP            = yes


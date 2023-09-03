#----------------------------------------------------------------------------
# On command line:
#
# make  = Make software.
#
# make clean = Clean out built project files.
#
#
# To rebuild project do "make clean" then "make".
#----------------------------------------------------------------------------

#----------- BUILD OPTIONS ---------------------------

# PCB version
# Values: MEGAMINI, MEGA2560
PCB = MEGAMINI

# LCD
# Values: ST7565P, ST7565R, ERC12864FSF, ST7920, KS108, SSD1306, SH1106, NT7538SPI
LCD = ST7565P

# LCD 180Deg rotation
# Values: = YES, NO
LCDROT180 = NO

# Enable FrSky Telemetry reception on UART0 or SPI
# Values = STD, FRSKY
EXT = STD

# FAI mode
# Values = YES, NO, CHOICE
FAI = NO

# SD card is present
# Values = YES, NO
SDCARD = NO

# Use Bluetooth
# Values = NO, HC05
BLUETOOTH = NO

# Use xmodem
# Values = YES, NO
XMODEM = NO

# Enable SCC menu
# Values = YES, NO
SCC = YES

# Enable heli menu
# Values = YES, NO
HELI = NO

# Enable templates menu
# Values = YES, NO
TEMPLATES = NO

# Enable navigation with Pots / Sticks / Rotary encoders
# Values = NO
#          POT1 (only POT1 available for fields modification),
#          POT2 (menus scroll),
#          POT3 (cursor down/up),
#          POTS (POT1, POT2, POT3),
#          STICKS
#          ROTENC (Rotary encoder)
NAVIGATION = NO

# Switches automatic selection
# Values = YES, NO
AUTOSWITCH = NO

# Sources automatic selection
# Values = YES, NO
AUTOSOURCE = NO

# Double keys support (+/- means invert, +/up means max, -/dwn means min, up/down means reset to default)
# Values = YES, NO
DBLKEYS = NO

# AUDIO Mods
# Values = YES, NO
AUDIO = NO

# VOICE Mods
# Values = YES, NO (WTV20SD,JQ6500,NO on MEGA2560 DIY)
VOICE = NO

# corrects different rounding for negative values. So instead of -99.9 you will see again -100.0
# Values = YES, NO
CORRECT_NEGATIVE_VALUES = YES

# HAPTIC Mods
# Values = YES, NO
HAPTIC = NO

# Fade Backlight using PWM
# Values = YES, NO
PWM_BACKLIGHT = NO

# SPLASH on START
# Values = YES, NO, FIXED
SPLASH = YES

# GRAPHICS ( bold / batt graph)
BOLD = YES
BATTGRAPH = NO

# FONT TYPE
# Values = STD, SQT5
FONT = STD

# PPM center adjustable
# Values = YES, NO
PPM_CENTER_ADJUSTABLE = NO

# PPM symmetrical limits
# Values = YES, NO
PPM_LIMITS_SYMETRICAL = NO

# PPM display unit
# Values = PERCENT_PREC1, PERCENT_PREC0, US
PPM_UNIT = PERCENT_PREC1

# OverrideCHxx functions enable
# Values = YES, NO
OVERRIDE_CHANNEL_FUNCTION = YES

# WS HowHigh
# Values = YES, NO
WS_HOW_HIGH = YES

# GPS
# Values = YES, NO
GPS = YES

# Vario
# Values = YES, NO
VARIO = YES

# RTCLOCK
# Values = YES, NO
# default = YES (means enabled when available)
RTCLOCK = YES

# Serial protocol
# Values = NO, DSM, MULTIMODULE, CRSF, SBUS, SUMD
SERIAL_PROTOCOL = NO

# Pcm protocol
# Values = NO, YES
PCM_PROTOCOL = NO

# TRANSLATIONS
# Values = cz, de, en, fi, fr, it, se, es, pl, pt, nl, sk
TRANSLATIONS = EN

# TTS
# Values = en, fr, hu
TTS = $(TRANSLATIONS)

# UNITS
# Values = imperial, metric
UNITS = METRIC

# Default Tx Mode
# Values = NO / 1 / 2 / 3 / 4
DEFAULT_MODE = NO

# Toggle trim usage
# -> Left trim for right stick and right trim for left stick
TOGGLETRIM = NO

# Prompt time with "and" seconds or not
# Values = YES, NO
NOANDSECONDE = NO

# Uses JivaroFad mixer (EX: Curve(source + offset + trim) * weight)
# Values = YES, NO
OFFSET_ON_INPUT = YES

# EEPROM_PROGRESS_BAR
# Values = YES, NO
EEPROM_PROGRESS_BAR = NO

# Debug with TinyDbg
# Values = NO, UART_BT
DEBUG = NO

# Timers Count
# Values = 2
TIMERS = 2

# Flight Modes
# Values = YES, NO
FLIGHT_MODES = YES

# Curves
# Values = YES, NO
CURVES = YES

# Global Vars
# Values = YES, NO
GVARS = NO

# Choose the other 3 positions switch build with a pot
# Values = POT1, POT2, POT3
3POS = POT2

# Allow to change buttons names
# Values = NO, YES with personames.h edited
PERSONAMES = NO

# Show a bouble vertical stick (For boats, tanks, ...)
# Values = NO, YES
DBLGAZSTICK = NO

# Allow to add extra compilation option
# Values = NO, ....
OTHERCOMPOPTIONS = NO

#Use external eeprom FM24W256
EXTERNALEEPROM = NO

# Menu Lock modes
# Values = NO, READONLY, NOMENUS
MENUS_LOCK = NO

# Enable SPIMODULES protocols
# Values = NO, YES
SPIMODULES = NO

# Enable CC2500 SPI protocols
# Values = NO, YES
CC2500 = NO

# Set CC2500 PA gain in DB
# Values = 20,22
CC2500PAG = 20

# Enable CYRF6936 SPI protocols
# Values = NO, YES
CYRF6936 = NO

# Set CYRF6936 PA gain in DB
# Values = 20,22
CYRF6936PAG = 20

# Enable NRF24l01 SPI protocols
# Values = NO, YES
NRF24L01 = NO

# Set NRF24l01 PA gain in DB
# Values = 20,22
NRF24L01PAG = 20

# Enable A7105 SPI protocols
# Values = NO, YES
A7105 = NO

# Set A7105 PA gain in DB
# Values = 20,22
A7105PAG = 20

# Enable X-ANY feature
# Values = NO, 1, 2, 3 ,4
X_ANY = NO

# Invert STICK_RH analog value
# Values = NO, YES
INV_STICK_RH = NO

# Invert STICK_LV analog value
# Values = NO, YES
INV_STICK_LV = NO

# Invert STICK_RV analog value
# Values = NO, YES
INV_STICK_RV = NO

# Invert STICK_LH analog value
# Values = NO, YES
INV_STICK_LH = NO

# Common resources
need_Serial1 = NO
need_uCli = NO

#------- END BUILD OPTIONS ---------------------------

# Define programs and commands.
SHELL = sh

IMG2LBM = -python ../util/img2lbm.py
BIN2LBM = -python ../util/bin2lbm.py
FONT2PNG = -python ../util/font2png.py
TRANSLATE = -python ../util/translate.py

VERSION_MAJOR = 3
VERSION_MINOR = 1
VERSION_REVISION = 0

VERSION = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_REVISION)$(OpenAVRc_VERSION_SUFFIX)

THIRDPARTY = thirdparty
RRSDDIR = $(THIRDPARTY)/rrsd
XMODEMDIR = $(THIRDPARTY)/xmodem

DEBUGDIR = debug


SRC =
CPPSRC =
INCDIRS = . translations

# PCB name
ifneq ($(PCB), $(filter $(PCB), MEGA2560 MEGAMINI EVO_V2))
  $(error Sorry, the PCB $(PCB) is not supported yet in OpenAVRc)
endif

# 3POS
ifneq ($(3POS), $(filter $(3POS), POT1 POT2 POT3))
  $(error Sorry, the 3POS $(3POS) is not supported yet in OpenAVRc)
endif

# XMODEM
ifeq ($(XMODEM), YES)
	ifneq ($(SDCARD), YES)
 	 $(error Sorry, XMODEM need a SDCARD in OpenAVRc)
	endif
endif


GUIMODELSRC = gui/menu_model.cpp gui/menu_model_select.cpp gui/menu_model_setup.cpp gui/menu_model_inputs_mixes.cpp gui/menu_model_limits.cpp gui/menu_model_logical_switches.cpp gui/menu_model_custom_functions.cpp
GUIGENERALSRC = gui/menu_general.cpp gui/menu_general_setup.cpp gui/menu_general_trainer.cpp gui/menu_general_version.cpp gui/menu_general_calib.cpp

BITMAPS = bitmaps/splash.lbm bitmaps/asterisk.lbm bitmaps/sticks.lbm bitmaps/XanyCh.lbm bitmaps/bt.lbm bitmaps/ppm.lbm bitmaps/evo_splash.lbm bitmaps/desktop.lbm

FONTS = $(patsubst %.png,%.lbm,$(wildcard fonts/*/*.png))

ifeq ($(PCB), MEGAMINI)
  # Antho & Pierro's Shield

  FLAVOUR = MegaMini
  ARCH = AVR
  LCDSIZE = 128
  TRGT = avr-
  MCU = atmega2560
  OPT = 2
  # Processor frequency.
  F_CPU = 16000000
  CPPDEFS = -DF_CPU=$(F_CPU)UL -DPCBMEGAMINI -DCPUM2560
  BUZZER = YES
  THR_TRACE = YES
  PWRMANAGE = NO
  INCDIRS += targets/megamini targets/stock $(RRSDDIR)
  BOARDSRC += main_avr.cpp i2c_master.cpp spi.c targets/megamini/board_megamini.cpp
  EEPROMSRC = eeprom_common.cpp eeprom_rlc.cpp
  PULSESSRC = pulses/pulses_avr.cpp
  GUIGENERALSRC += gui/menu_general_diagkeys.cpp gui/menu_general_diaganas.cpp
  EXTRABOARDSRC = adc_driver.cpp targets/lcd/lcd_driver.cpp

  ASRC =

	ifneq ($(DEBUG), NO)
		need_Serial1=YES
		CPPSRC += $(DEBUGDIR)/TinyDbg.cpp
		ifeq ($(DEBUG), UART_BT)
		CPPDEFS += -DTINY_DBG_UART_BT
		endif
	endif

  needSerial0 = FALSE
  ifneq ($(SERIAL_PROTOCOL), NO)
   needSerial0 = YES
  endif
  ifeq ($(EXT), FRSKY)
   needSerial0 = YES
  endif
  ifeq ($(needSerial0), YES)
  EXTRABOARDSRC += Serial0.cpp
  endif

 ifeq ($(PWRMANAGE), YES)
    CPPDEFS += -DPWRMANAGE
  endif

  ifeq ($(PWM_BACKLIGHT), YES)
    CPPDEFS += -DPWM_BACKLIGHT
  endif

  ifeq ($(SDCARD), YES)
    EXTRABOARDSRC += $(RRSDDIR)/fat.c $(RRSDDIR)/partition.c $(RRSDDIR)/sd_raw.c $(RRSDDIR)/byteordering.c
    CPPDEFS += -DSDCARD
    GUIGENERALSRC += gui/menu_general_sdmanager.cpp
    CPPSRC += sdcard.cpp logs.cpp
    RTCLOCK = YES
  endif

  ifeq ($(AUDIO), YES)
    CPPDEFS += -DAUDIO
    CPPSRC += audio_avr.cpp
    BUZZER = NO
  else
    BUZZER = YES
  endif

  ifeq ($(BUZZER), YES)
    CPPDEFS += -DBUZZER
    CPPSRC += buzzer.cpp
  endif

  ifeq ($(RTCLOCK), YES)
    CPPDEFS += -DRTCLOCK
    CPPSRC += rtc_driver.cpp
  endif

  ifeq ($(VOICE), JQ6500)
  CPPDEFS += -DVOICE
  CPPDEFS += -DVOICE_JQ6500
  CPPSRC += targets/mega2560/voice_jq6500.cpp
  endif

  ifeq ($(HAPTIC), YES)
   CPPDEFS += -DHAPTIC
   CPPSRC += haptic.cpp
  endif

  ifeq ($(EXTERNALEEPROM), YES)
   CPPDEFS += -DEXTERNALEEPROM
  endif

  ifeq ($(PERSONAMES), YES)
   CPPDEFS += -DPERSONAMES
  endif

  ifeq ($(SPIMODULES), YES)
   CPPDEFS += -DSPIMODULES

		ifeq ($(CC2500), YES)
 	  	CPPDEFS += -DPROTO_HAS_CC2500
 	  	CPPDEFS += -DCC2500PA_GAIN=$(CC2500PAG)
		endif

		ifeq ($(CYRF6936), YES)
   		CPPDEFS += -DPROTO_HAS_CYRF6936
 	  	CPPDEFS += -DCYRF6936PA_GAIN=$(CYRF6936PAG)
  	endif

  	ifeq ($(NRF24L01), YES)
   		CPPDEFS += -DPROTO_HAS_NRF24L01
 	  	CPPDEFS += -DNRF24L01PA_GAIN=$(NRF24L01PAG)
  	endif

  	ifeq ($(A7105), YES)
   		CPPDEFS += -DPROTO_HAS_A7105
 	  	CPPDEFS += -DA7105PA_GAIN=$(A7105PAG)
  	endif

  endif

	ifneq ($(X_ANY), NO)
	CPPSRC += Xany.cpp gui/menu_model_Xany.cpp
		ifeq ($(X_ANY), 1)
		CPPDEFS += -DX_ANY=1
		endif
		ifeq ($(X_ANY), 2)
		CPPDEFS += -DX_ANY=2
		endif
		ifeq ($(X_ANY), 3)
		CPPDEFS += -DX_ANY=3
		endif
		ifeq ($(X_ANY), 4)
		CPPDEFS += -DX_ANY=4
		endif
	endif

	ifeq ($(INV_STICK_RH), YES)
	CPPDEFS += -DINV_STICK_RH
	endif

	ifeq ($(INV_STICK_LV), YES)
	CPPDEFS += -DINV_STICK_LV
	endif

	ifeq ($(INV_STICK_RV), YES)
	CPPDEFS += -DINV_STICK_RV
	endif

	ifeq ($(INV_STICK_LH), YES)
	CPPDEFS += -DINV_STICK_LH
	endif

endif


ifeq ($(PCB), EVO_V2)
  # EVO V2 PCB with XMega256a3u/c3/d3

  FLAVOUR = Evo
  ARCH = AVR
  LCDSIZE = 128
  TRGT = avr-
  MCU = atxmega256a3u
  # atxmega256d3 libc header files have errors.
  # Processor frequency.
  F_CPU = 32000000
  OPT = s
  CPPDEFS = -DF_CPU=$(F_CPU)UL -DCPUXMEGA -DPCB_EVO -DREV_EVO_V2

  AUDIO = YES
  BUZZER = NO
  PWM_BACKLIGHT = NO
  EXTERNALEEPROM = YES
  EXT = FRSKY
  HAPTIC = NO
  PERSONAMES = YES
  PWRMANAGE = NO
  RTCLOCK = YES
  SCC = NO
  SDCARD = YES
  SPLASH = YES
  THR_TRACE = YES
  VOICE = YES
  SPIMODULES = YES
  A7105 = YES
  CC2500 = YES
  CYRF6936 = YES
  NRF24L01 = YES 
  SERIAL_PROTOCOL = MULTIMODULE
  INV_STICK_RV = YES
  INV_STICK_LH = YES

  INCDIRS += targets/evo_v2 targets/stock $(RRSDDIR)
  BOARDSRC += main_avr.cpp targets/evo_v2/xmega.cpp
  EEPROMSRC = eeprom_common.cpp eeprom_rlc.cpp
  PULSESSRC = pulses/pulses_avr.cpp
  GUIGENERALSRC += gui/menu_general_diagkeys.cpp targets/evo_v2/menu_general_diaganas.cpp
  EXTRABOARDSRC = targets/evo_v2/lcd_ssd1309_driver.cpp targets/evo_v2/xmega_adc_driver.cpp targets/evo_v2/Serial1.cpp

  needSerial0 = FALSE
  ifneq ($(SERIAL_PROTOCOL), NO)
   needSerial0 = YES
  endif
  ifeq ($(EXT), FRSKY)
   needSerial0 = YES
  endif
  ifeq ($(needSerial0), YES)
  EXTRABOARDSRC += Serial0.cpp
  endif

  ifeq ($(PWM_BACKLIGHT), YES)
    CPPDEFS += -DPWM_BACKLIGHT
  endif

  ifeq ($(PWRMANAGE), YES)
    CPPDEFS += -DPWRMANAGE
  endif

  ifeq ($(SDCARD), YES)
    EXTRABOARDSRC += $(RRSDDIR)/fat.c $(RRSDDIR)/partition.c $(RRSDDIR)/sd_raw.c $(RRSDDIR)/byteordering.c
    CPPDEFS += -DSDCARD
    GUIGENERALSRC += gui/menu_general_sdmanager.cpp
    CPPSRC += sdcard.cpp logs.cpp spi.c
    RTCLOCK = YES
  endif

  ifeq ($(RTCLOCK), YES)
    CPPDEFS += -DRTCLOCK
    CPPSRC += rtc_pcf8523.cpp i2c_master.cpp
    EXTERNALEEPROM = YES
  endif

  ifeq ($(EXTERNALEEPROM), YES)
   CPPDEFS += -DEXTERNALEEPROM
  endif

  ifeq ($(AUDIO), YES)
    CPPDEFS += -DAUDIO
    CPPSRC += audio_avr.cpp
    BUZZER = NO
  else
    BUZZER = YES
  endif

  ifeq ($(BUZZER), YES)
    CPPDEFS += -DBUZZER
    CPPSRC += buzzer.cpp
  endif

  ifeq ($(VOICE), YES)
    CPPDEFS += -DVOICE
    CPPDEFS += -DVOICE_JQ6500
    CPPDEFS += -DVOICE_BY8001
    CPPSRC += targets/evo_v2/voice.cpp
  endif

  ifeq ($(HAPTIC), YES)
    CPPDEFS += -DHAPTIC
    CPPSRC += haptic.cpp
  endif

  ifeq ($(PERSONAMES), YES)
    CPPDEFS += -DPERSONAMES
  endif

  ifeq ($(SPIMODULES), YES)
    CPPDEFS += -DSPIMODULES

    ifeq ($(CC2500), YES)
      CPPDEFS += -DPROTO_HAS_CC2500
      CPPDEFS += -DCC2500PA_GAIN=$(CC2500PAG)
	 endif

    ifeq ($(CYRF6936), YES)
      CPPDEFS += -DPROTO_HAS_CYRF6936
      CPPDEFS += -DCYRF6936PA_GAIN=$(CYRF6936PAG)
    endif

    ifeq ($(NRF24L01), YES)
      CPPDEFS += -DPROTO_HAS_NRF24L01
      CPPDEFS += -DNRF24L01PA_GAIN=$(NRF24L01PAG)
    endif

    ifeq ($(A7105), YES)
      CPPDEFS += -DPROTO_HAS_A7105
      CPPDEFS += -DA7105PA_GAIN=$(A7105PAG)
    endif
  endif

  ifeq ($(INV_STICK_RH), YES)
    CPPDEFS += -DINV_STICK_RH
  endif

  ifeq ($(INV_STICK_LV), YES)
    CPPDEFS += -DINV_STICK_LV
  endif

  ifeq ($(INV_STICK_RV), YES)
    CPPDEFS += -DINV_STICK_RV
  endif

  ifeq ($(INV_STICK_LH), YES)
    CPPDEFS += -DINV_STICK_LH
  endif

endif


ifeq ($(PCB), MEGA2560)
  # ARDUINO2560 DIY RADIO

  FLAVOUR = Mega2560
  ARCH = AVR
  LCDSIZE = 128
  TRGT = avr-
  MCU = atmega2560
  OPT = s
  # Processor frequency.
  F_CPU = 16000000
  CPPDEFS = -DF_CPU=$(F_CPU)UL -DPCBMEGA2560 -DCPUM2560
  BUZZER = YES
  THR_TRACE = YES
  PWRMANAGE = NO
  INCDIRS += targets/mega2560 targets/stock $(RRSDDIR)
  BOARDSRC = main_avr.cpp i2c_master.cpp spi.c targets/mega2560/board_mega2560.cpp
  EEPROMSRC = eeprom_common.cpp eeprom_rlc.cpp
  PULSESSRC = pulses/pulses_avr.cpp
  GUIGENERALSRC += gui/menu_general_diagkeys.cpp gui/menu_general_diaganas.cpp

  EXTRABOARDSRC = adc_driver.cpp targets/lcd/lcd_driver.cpp

	ifneq ($(DEBUG), NO)
		need_Serial1=YES
		CPPSRC += $(DEBUGDIR)/TinyDbg.cpp
		ifeq ($(DEBUG), UART_BT)
		CPPDEFS += -DTINY_DBG_UART_BT
		endif
	endif

  needSerial0 = FALSE
  ifneq ($(SERIAL_PROTOCOL), NO)
   needSerial0 = YES
  endif
  ifeq ($(EXT), FRSKY)
   needSerial0 = YES
  endif
  ifeq ($(needSerial0), YES)
  EXTRABOARDSRC += Serial0.cpp
  endif

  ifeq ($(PWRMANAGE), YES)
    CPPDEFS += -DPWRMANAGE
  endif

  ifeq ($(SDCARD), YES)
    EXTRABOARDSRC += $(RRSDDIR)/fat.c $(RRSDDIR)/partition.c $(RRSDDIR)/sd_raw.c $(RRSDDIR)/byteordering.c
    CPPDEFS += -DSDCARD -DRTCLOCK
    GUIGENERALSRC += gui/menu_general_sdmanager.cpp
    CPPSRC += sdcard.cpp logs.cpp
  endif

  ifeq ($(AUDIO), YES)
    CPPDEFS += -DAUDIO
    CPPSRC += audio_avr.cpp
    BUZZER = NO
  else
    BUZZER = YES
  endif

  ifeq ($(BUZZER), YES)
   CPPDEFS += -DBUZZER
   CPPSRC += buzzer.cpp
  endif

  ifeq ($(RTCLOCK), YES)
    CPPDEFS += -DRTCLOCK
    CPPSRC += rtc_driver.cpp
  endif

  ifeq ($(VOICE), JQ6500)
   CPPDEFS += -DVOICE
   CPPDEFS += -DVOICE_JQ6500
   CPPSRC += targets/mega2560/voice_jq6500.cpp
  endif

  ifeq ($(HAPTIC), YES)
   CPPDEFS += -DHAPTIC
   CPPSRC += haptic.cpp
  endif

  ifeq ($(EXTERNALEEPROM), YES)
   CPPDEFS += -DEXTERNALEEPROM
  endif

  ifeq ($(PERSONAMES), YES)
   CPPDEFS += -DPERSONAMES
  endif

  ifeq ($(SPIMODULES), YES)
   CPPDEFS += -DSPIMODULES

		ifeq ($(CC2500), YES)
 	  	CPPDEFS += -DPROTO_HAS_CC2500
 	  	CPPDEFS += -DCC2500PA_GAIN=$(CC2500PAG)
		endif

		ifeq ($(CYRF6936), YES)
   		CPPDEFS += -DPROTO_HAS_CYRF6936
 	  	CPPDEFS += -DCYRF6936PA_GAIN=$(CYRF6936PAG)
  	endif

  	ifeq ($(NRF24L01), YES)
   		CPPDEFS += -DPROTO_HAS_NRF24L01
 	  	CPPDEFS += -DNRF24L01PA_GAIN=$(NRF24L01PAG)
  	endif

  	ifeq ($(A7105), YES)
   		CPPDEFS += -DPROTO_HAS_A7105
 	  	CPPDEFS += -DA7105PA_GAIN=$(A7105PAG)
  	endif

  endif


	ifneq ($(X_ANY), NO)
	CPPSRC += Xany.cpp gui/menu_model_Xany.cpp
		ifeq ($(X_ANY), 1)
		CPPDEFS += -DX_ANY=1
		endif
		ifeq ($(X_ANY), 2)
		CPPDEFS += -DX_ANY=2
		endif
		ifeq ($(X_ANY), 3)
		CPPDEFS += -DX_ANY=3
		endif
		ifeq ($(X_ANY), 4)
		CPPDEFS += -DX_ANY=4
		endif
	endif

endif

CC      = $(TRGT)g++
CPP     = $(TRGT)cpp
AS      = $(TRGT)as
OBJCOPY = $(TRGT)objcopy
OBJDUMP = $(TRGT)objdump
SIZE    = $(TRGT)size
NM      = $(TRGT)nm

REMOVE = rm -f
REMOVEDIR = rm -rf

SLEEP = sleep

# Processor frequency.
#F_CPU = 16000000

# Output format. (can be srec, ihex, binary)
FORMAT = ihex

# Target file name (without extension).
TARGET = OpenAVRc

# List C++ source files here. (C dependencies are automatically generated.)
  STD_TTS_SRC = translations/tts_$(shell sh -c "echo $(TTS) | tr '[:upper:]' '[:lower:]'").cpp
  TTS_SRC = $(shell sh -c "if test -f $(STD_TTS_SRC); then echo $(STD_TTS_SRC); else echo translations/tts_en.cpp; fi")

GUISRC = gui/navigation.cpp gui/popups.cpp gui/widgets.cpp gui/menus.cpp $(GUIMODELSRC) $(GUIGENERALSRC) gui/view_main.cpp gui/view_statistics.cpp

CPPSRC += OpenAVRc.cpp functions.cpp strhelpers.cpp $(PULSESSRC) switches.cpp curves.cpp mixer.cpp stamp.cpp $(GUISRC) $(EEPROMSRC) timers.cpp trainer_input.cpp

GUISRC += gui/lcd.cpp gui/splash.cpp gui/fonts.cpp

GUISRC += keys.cpp maths.cpp translations.cpp $(TTS_SRC)

# Debugging format.
#     Native formats for AVR-GCC's -g are dwarf-2 [default] or stabs.
#     AVR Studio 4.10 requires dwarf-2.
#     AVR [Extended] COFF format requires stabs, plus an avr-objcopy run.
DBGFMT = dwarf-2

CPPDEFS += -DTRANSLATIONS_$(TRANSLATIONS)

# TTS language
CPPDEFS += -DTTS_$(TTS)

ifeq ($(UNITS), IMPERIAL)
  CPPDEFS += -DIMPERIAL_UNITS
endif

ifneq ($(DEFAULT_MODE), NO)
  CPPDEFS += -DDEFAULT_MODE=$(DEFAULT_MODE)
endif

ifeq ($(TOGGLETRIM), YES)
  CPPDEFS += -DTOGGLETRIM
endif

ifeq ($(NOANDSECONDE), YES)
  CPPDEFS += -DNOANDSECONDE
endif

ifeq ($(OFFSET_ON_INPUT), YES)
  CPPDEFS += -DOFFSET_ON_INPUT
endif

ifeq ($(LCDROT180), YES)
  CPPDEFS += -DLCDROT180
endif

# If POTS are used for fields modification
ifeq ($(NAVIGATION), POT1)
  CPPDEFS += -DNAVIGATION_POT1
endif
ifeq ($(NAVIGATION), POT2)
  CPPDEFS += -DNAVIGATION_POT2
endif
ifeq ($(NAVIGATION), POT3)
  CPPDEFS += -DNAVIGATION_POT3
endif
ifeq ($(NAVIGATION), POTS)
  CPPDEFS += -DNAVIGATION_POT1 -DNAVIGATION_POT2 -DNAVIGATION_POT3
endif
ifeq ($(NAVIGATION), STICKS)
  CPPDEFS += -DNAVIGATION_STICKS
endif
ifeq ($(NAVIGATION), ROTENC)
  CPPDEFS += -DROTARY_ENCODER_NAVIGATION
endif

ifeq ($(LCD), ST7565R)
  CPPDEFS += -DLCD_ST7565R
else ifeq ($(LCD), ERC12864FSF)
  CPPDEFS += -DLCD_ERC12864FSF
else ifeq ($(LCD), ST7920)
  CPPDEFS += -DLCD_ST7920
else ifeq ($(LCD), KS108)
  CPPDEFS += -DLCD_KS108
else ifeq ($(LCD), SSD1306)
  CPPDEFS += -DLCD_SSD1306
else ifeq ($(LCD), SH1106)
  CPPDEFS += -DLCD_SH1106
else ifeq ($(LCD), NT7538SPI)
  CPPDEFS += -DLCD_NT7538SPI
endif

ifeq ($(AUTOSWITCH), YES)
  CPPDEFS += -DAUTOSWITCH
endif

ifeq ($(AUTOSOURCE), YES)
  CPPDEFS += -DAUTOSOURCE
endif

ifeq ($(DBLKEYS), YES)
  CPPDEFS += -DDBLKEYS
endif

ifeq ($(SPLASH), YES)
  CPPDEFS += -DSPLASH
endif

ifeq ($(SPLASH), FIXED)
  CPPDEFS += -DSPLASH -DFSPLASH
endif

ifeq ($(BOLD), YES)
  CPPDEFS += -DBOLD_FONT
endif

ifeq ($(BATTGRAPH), YES)
  CPPDEFS += -DBATTGRAPH
endif

ifeq ($(FONT), STD)
  CPPDEFS += -DFONTSTD
endif

INCDIRS += gui
INCDIRS += fonts/std
INCDIRS += fonts/sqt5

ifeq ($(THR_TRACE), YES)
  CPPDEFS += -DTHRTRACE
endif

CPPDEFS += -DTIMERS=$(TIMERS)

ifeq ($(FLIGHT_MODES), YES)
  CPPDEFS += -DFLIGHT_MODES
  GUIMODELSRC += gui/menu_model_flightmodes.cpp
endif

ifeq ($(CURVES), YES)
  CPPDEFS += -DCURVES
  GUIMODELSRC += gui/menu_model_curves.cpp
endif

ifeq ($(GVARS), YES)
  CPPDEFS += -DGVARS
endif

ifeq ($(MENUS_LOCK), READONLY)
  CPPDEFS += -DMENUS_LOCK=1
endif

ifeq ($(MENUS_LOCK), NOMENUS)
  CPPDEFS += -DMENUS_LOCK=2
endif

ifeq ($(CORRECT_NEGATIVE_VALUES), YES)
  CPPDEFS += -DCORRECT_NEGATIVE_SHIFTS
endif

# EXTENSIONS (for Telemetry protocols)
ifneq ($(EXT), $(filter $(EXT), STD FRSKY))
  $(error Sorry, the EXT $(EXT) is not supported yet in OpenAVRc)
endif


ifeq ($(EXT), $(filter $(EXT), FRSKY ))
  CPPDEFS += -DFRSKY
  CPPSRC += telemetry/frsky.cpp
  GUIMODELSRC +=  gui/menu_model_telemetry.cpp

  CPPSRC += gui/view_telemetry.cpp

  ifeq ($(WS_HOW_HIGH), YES)
    CPPDEFS += -DWS_HOW_HIGH
  endif

  ifeq ($(GPS), YES)
    CPPDEFS += -DGPS
  endif

  ifeq ($(VARIO), YES)
    CPPSRC += vario.cpp
    CPPDEFS += -DVARIO
  endif
endif

ifeq ($(EEPROM_PROGRESS_BAR), YES)
  CPPDEFS += -DEEPROM_PROGRESS_BAR
endif


### Global Build-Option Directives ###

ifeq ($(PPM_CENTER_ADJUSTABLE), YES)
  CPPDEFS += -DPPM_CENTER_ADJUSTABLE
endif

ifeq ($(PPM_LIMITS_SYMETRICAL), YES)
  CPPDEFS += -DPPM_LIMITS_SYMETRICAL
endif

ifeq ($(OVERRIDE_CHANNEL_FUNCTION), YES)
  CPPDEFS += -DOVERRIDE_CHANNEL_FUNCTION
endif

ifeq ($(PPM_UNIT), US)
  CPPDEFS += -DPPM_UNIT_US
endif

ifeq ($(PPM_UNIT), PERCENT_PREC1)
  CPPDEFS += -DPPM_UNIT_PERCENT_PREC1
endif

ifeq ($(PPM_UNIT), PERCENT_PREC0)
  CPPDEFS += -DPPM_UNIT_PERCENT_PREC0
endif

ifeq ($(FAI), YES)
  CPPDEFS += -DFAI
endif
ifeq ($(FAI), CHOICE)
  CPPDEFS += -DFAI_CHOICE
endif

ifneq ($(BLUETOOTH), NO)
  CPPDEFS += -DBLUETOOTH=$(BLUETOOTH)
  need_Serial1=YES
  need_uCli = YES
  GUIMODELSRC += gui/menu_general_bluetooth.cpp bluetooth.cpp
endif

ifeq ($(XMODEM), YES)
  CPPDEFS += -DXMODEM
  need_Serial1=YES
  need_uCli = YES
  GUIMODELSRC += $(XMODEMDIR)/xmodem.cpp
endif

ifeq ($(SCC), YES)
  CPPDEFS += -DSCC
  GUIMODELSRC += gui/menu_general_scc.cpp scc.cpp
endif

ifeq ($(PCM_PROTOCOL), YES)
  CPPDEFS += -DPCM_PROTOCOL
endif

ifeq ($(need_Serial1), YES)
	CPPSRC += Serial1.cpp
endif

ifeq ($(need_uCli), YES)
	CPPDEFS += -DU_CLI
	CPPSRC += uCli.cpp
endif

ifeq ($(HELI), YES)
  CPPDEFS += -DHELI
  GUIMODELSRC += gui/menu_model_heli.cpp
endif

ifeq ($(TEMPLATES), YES)
    CPPDEFS += -DTEMPLATES
    CPPSRC += templates.cpp
    GUIMODELSRC += gui/menu_model_templates.cpp
endif

CPPDEFS += -DDSM=1
CPPDEFS += -DMULTIMODULE=2
CPPDEFS += -DCRSF=3
CPPDEFS += -DSBUS=4
CPPDEFS += -DSUMD=5
ifneq ($(SERIAL_PROTOCOL), NO)
  CPPDEFS += -DSERIAL_PROTOCOL=$(SERIAL_PROTOCOL)
endif

ifeq ($(3POS), POT1)
  CPPDEFS += -DEXTRA_3POS=1
endif
ifeq ($(3POS), POT2)
  CPPDEFS += -DEXTRA_3POS=2
endif
ifeq ($(3POS), POT3)
  CPPDEFS += -DEXTRA_3POS=3
endif

ifneq ($(DBLGAZSTICK), NO)
  CPPDEFS += -DDBLGAZSTICK
endif

ifneq ($(OTHERCOMPOPTIONS), NO)
  CPPDEFS += $(subst *, ,$(OTHERCOMPOPTIONS))
endif

#---------------- Compiler Options C++ ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
#CPPFLAGS = -g$(DBGFMT)
CPPFLAGS += $(CPPDEFS)
CPPFLAGS += -Wall
CPPFLAGS += -mrelax
CPPFLAGS += -fno-exceptions
#CPFLAGS += -fno-threadsafe-statics
CPPFLAGS += -Wno-strict-aliasing
CPPFLAGS += -Wformat -Wreturn-type -Wunused -Wuninitialized -Wunknown-pragmas -Wno-switch -Wtype-limits
INCFLAGS = $(patsubst %,-I%,$(INCDIRS))

MATH_LIB = -lm

#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(MATH_LIB)

# Define Messages
# English
MSG_BEGIN = -------- Process OpenAVRc compilation --------
MSG_END = --------  OpenAVRc compilation done  --------
MSG_FLASH = Creating OpenAVRc Firmware:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_COMPILING = Compiling C++:
MSG_COMPILINGASM = Compiling ASM:
MSG_CLEANING = Cleaning project:

# Combine all necessary flags and optional flags.
# Add target processor to flags.

CPPFLAGS += -DFLAVOUR=\"$(FLAVOUR)\"
AVRCPPFLAGS = -mmcu=$(MCU) -I. -x c++ -O$(OPT) $(CPPFLAGS) $(INCFLAGS) $(GENDEPFLAGS) -fwhole-program -mstrict-X


.SECONDARY:

# Default target.
all: begin gccversion build end

# Change the build target to build a HEX file or a library.
  build: stamp_header tra lbm allsrc.cpp allasm.S elf remallsrc hex sizeoffirmware

tra: translations/en.h translations/cz.h translations/sk.h translations/de.h translations/es.h translations/fi.h translations/fr.h translations/it.h translations/nl.h translations/pl.h translations/pt.h translations/se.h
lbm: fonts bitmaps

elf: $(TARGET).elf
bin: $(TARGET).bin
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym

# Build stamp-file
stamp_header:
	@echo "// Automatically generated file (Makefile) - do not edit" > stamp-OpenAVRc.h
	@echo "#define DATE_STR \"`date +%Y-%m-%d | tr -d '\r\n'`\"" >> stamp-OpenAVRc.h
	@echo "#define TIME_STR \"`date +%H:%M:%S | tr -d '\r\n'`\"" >> stamp-OpenAVRc.h
	@echo "#define VERS_STR \"$(VERSION)\"" >> stamp-OpenAVRc.h
	@echo "#define FLAVOUR \"$(FLAVOUR)\"" >> stamp-OpenAVRc.h
	@echo "#define VERSION_MAJOR     $(VERSION_MAJOR)" >> stamp-OpenAVRc.h
	@echo "#define VERSION_MINOR     $(VERSION_MINOR)" >> stamp-OpenAVRc.h
	@echo "#define VERSION_REVISION  $(VERSION_REVISION)" >> stamp-OpenAVRc.h

OpenAVRc-stamp:
	@echo "#define DATE_STR \"`date +%Y-%m-%d`\"" > stamp-OpenAVRc.txt
	@echo "#define TIME_STR \"`date +%H:%M:%S`\"" >> stamp-OpenAVRc.txt
	@echo "#define VERS_STR \"$(VERSION)\"" >> stamp-OpenAVRc.txt
	@cat stamp-OpenAVRc.txt

translations/en.h: translations/en.h.txt
	@echo
	@echo "Create English language specific header files"
	$(TRANSLATE) translations/en.h.txt translations/en.h en

translations/cz.h: translations/cz.h.txt
	@echo
	@echo "Create Czech language specific header files"
	$(TRANSLATE) translations/cz.h.txt translations/cz.h cz

translations/sk.h: translations/sk.h.txt
	@echo
	@echo "Create Slovak language specific header files"
	$(TRANSLATE) translations/sk.h.txt translations/sk.h sk

translations/de.h: translations/de.h.txt
	@echo
	@echo "Create German language specific header files"
	$(TRANSLATE) translations/de.h.txt translations/de.h de

translations/es.h: translations/es.h.txt
	@echo
	@echo "Create Spanish language specific header files"
	$(TRANSLATE) translations/es.h.txt translations/es.h es

translations/fi.h: translations/fi.h.txt
	@echo
	@echo "Create Finnish language specific header files"
	$(TRANSLATE) translations/fi.h.txt translations/fi.h fi

translations/fr.h: translations/fr.h.txt
	@echo
	@echo "Create French language specific header files"
	$(TRANSLATE) translations/fr.h.txt translations/fr.h fr

translations/it.h: translations/it.h.txt
	@echo
	@echo "Create Italian language specific header files"
	$(TRANSLATE) translations/it.h.txt translations/it.h it

translations/nl.h: translations/nl.h.txt
	@echo
	@echo "Create Nederlands language specific header files"
	$(TRANSLATE) translations/nl.h.txt translations/nl.h nl

translations/pl.h: translations/pl.h.txt
	@echo
	@echo "Create Polish language specific header files"
	$(TRANSLATE) translations/pl.h.txt translations/pl.h pl

translations/pt.h: translations/pt.h.txt
	@echo
	@echo "Create Portugese language specific header files"
	$(TRANSLATE) translations/pt.h.txt translations/pt.h pt

translations/se.h: translations/se.h.txt
	@echo
	@echo "Create Swedish language specific header files"
	$(TRANSLATE) translations/se.h.txt translations/se.h se

.PHONY: fonts
fonts: $(FONTS)

fonts/%.lbm: fonts/%.png
	$(IMG2LBM) $^ $@ $(LCDSIZE)


.PHONY: bitmaps
bitmaps: $(BITMAPS)

bitmaps/sticks.lbm: bitmaps/sticks.png
	$(IMG2LBM) $^ $@ $(LCDSIZE) 1bit 4

bitmaps/XanyCh.lbm: bitmaps/XanyCh.png
	$(IMG2LBM) $^ $@ $(LCDSIZE) 1bit 8

bitmaps/%.lbm: bitmaps/%.png
	$(IMG2LBM) $^ $@ $(LCDSIZE) 1bit

ifeq ($(OS),Windows_NT)
  PARSER = avr-cpp
else
  PARSER = gcc -E -x c++
endif

begin:
	@echo
	@echo $(MSG_BEGIN)

end:
	@echo $(MSG_END)
	@echo

# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(OBJDUMP) -Pmem-usage $(TARGET).elf


# Display compiler version information.
gccversion:
	@$(CC) --version

eeprom.bin:
	dd if=/dev/zero of=$@ bs=1 count=2048


# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

%.bin: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O binary  $< $@

%.eep: %.elf
	@echo
	@echo $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S $< > $@

# Concatenate all sources files in one big file to optimize size
allsrc.cpp: Makefile $(BOARDSRC) $(CPPSRC) $(EXTRABOARDSRC)
	@rm -f allsrc.cpp
	for f in $(BOARDSRC) $(CPPSRC) $(EXTRABOARDSRC) ; do echo "# 1 \"$$f\"" >> allsrc.cpp; cat "$$f" >> allsrc.cpp; done

# Concatenate all sources files in one big file to optimize size
allasm.S: Makefile $(ASRC)
	@rm -f allasm.S
ifneq ($(ASRC),  )
	for f in $(ASRC) ; do echo "# 1 \"$$f\"" >> allasm.S; cat "$$f" >> allasm.S; done
endif

remallsrc:
	$(REMOVE) allsrc.cpp
	$(REMOVE) allsrc.o
	$(REMOVE) allasm.S
	$(REMOVE) allasm.o


sizeoffirmware:
	@if test -f $(TARGET).elf; then echo; \
	echo ----------------; $(HEXSIZE); \
	echo ----------------; $(ELFSIZE); \
	echo; fi


# Link: create ELF output file from object files.
OpenAVRc.elf: allsrc.cpp
	@echo
ifneq ($(ASRC),  )
	@echo $(MSG_COMPILINGASM) $@
	$(CC) -c $(AVRCPPFLAGS) -I. -x assembler-with-cpp allasm.S
	@echo $(MSG_COMPILING) $@
	$(CC) -c $(AVRCPPFLAGS) $<
	$(CC) -mmcu=$(MCU) allasm.o allsrc.o --output $@ $(LDFLAGS)
else
	@echo $(MSG_COMPILING) $@
	$(CC) $(AVRCPPFLAGS) $< --output $@ $(LDFLAGS)
endif

# Target: clean project.
clean: begin clean_list end
#clean: begin clean_list clean_list_all end

CLEAN_SRC=$(filter %.c,$(SRC))
clean_list :
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) simu
	$(REMOVE) $(TARGET).bin
	$(REMOVE) $(TARGET).hex
	$(REMOVE) $(TARGET).eep
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lss
	$(REMOVE) $(TARGET).lst
	$(REMOVE) $(CLEAN_SRC:.c=.s)
	$(REMOVE) `find . -name "*.o"`
	$(REMOVE) *.d
	$(REMOVE) *.lst
	$(REMOVE) allsrc.cpp
	$(REMOVE) allsrc.o
	$(REMOVE) allasm.S
	$(REMOVE) allasm.o
	$(REMOVE) stamp-OpenAVRc.h

clean_list_all :
	$(REMOVE) *.lbm
	$(REMOVE) bitmaps/*.lbm
	$(REMOVE) bitmaps/*/*.lbm
	$(REMOVE) fonts/*.lbm
	$(REMOVE) fonts/*/*.lbm
	$(REMOVE) *.lbmi
	$(REMOVE) bitmaps/*.lbmi
	$(REMOVE) bitmaps/*/*.lbmi
	$(REMOVE) fonts/*.lbmi
	$(REMOVE) fonts/*/*.lbmi
	$(REMOVE) translations/*.h







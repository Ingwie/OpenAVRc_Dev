# !!!! BETA Makefile !!!!
# !!!! Use at own risk !!!!
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
# Values: MEGA2560
PCB = MEGA2560

# PCB revision
PCBREV =

# LCD
# Values: ST7565P, ST7565R, ERC12864FSF, ST7920, KS108, SSD1306
LCD = ST7565P

# Enable JETI-Telemetry or FrSky Telemetry reception on UART0
# For this option you need to modify your hardware!
# More information at [insertURLhere]
# Values = STD, FRSKY, FRSKY_SPORT, JETI, NMEA, ARDUPILOT, MAVLINK, TELEMETREZ
EXT = STD

# GUI enabled
# Values = YES, NO
GUI = YES

# FAI mode
# Values = YES, NO, CHOICE
FAI = NO

# the latests deliveries for Turnigy 9x transmitters (since 2 years now, 2014) do not stop sending if no pulses are generated.
# This fact totally breaks the nice throttle and switch warning concept, because even we prevent sending pulses the
# transmitter module ignores this, and continues to send the last value or 0 at beginning. And 0 means in the middle of throttle -> 50%.
# To cope with this situation, we need a new concept how to handle throttle and switch warnings.
# if you have a 0 value (50% ) for throttle, while throttle warning is active, you should activate this switch
# Values = YES, NO
TURNIGY_TRANSMITTER_FIX = NO

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

# Stock sticks replaced by FrSky ones
# Values = YES, NO
FRSKY_STICKS = NO

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

# adds additional code to prevent arithmetic overflows. MAX or MIN values stays. Not yet guaranteed!! Just improved
# Values = YES, NO
ARITHMETIC_OVERFLOW_CHECK = NO

# increases resolution for throttle statistics and throttle counting, in very rare cases.
# this will be only true if an output channel is used for throttle calculation, and this channel has changed limits, e.g. -90% to +80%
# if you use standard limits for throttle or directly the throttle stick you do not need to bother about this option
# not valid for ARM based transmitters like Taranis, They do not have this rounding issue at all
# Values = YES, NO
ACCURAT_THROTTLE_STATS = NO

# HAPTIC Mods
# Values = YES, NO
HAPTIC = NO


# Fade Backlight using PWM
# Values = YES, NO
PWM_BACKLIGHT = NO

# SPLASH on START
# Values = YES, NO, FIXED
SPLASH = $(GUI)

# GRAPHICS (sliders / checkboxes / bold / batt graph)
GRAPHICS = YES
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

# Dangerous functions available in Custom Functions (range test, bind, module off)
# Values = YES, NO
DANGEROUS_MODULE_FUNCTIONS = NO

# FrSky Hub
# Values = YES, NO
FRSKY_HUB = YES

# FrSky FAS current sensor offset compensation
# Values = YES, NO
FAS_OFFSET = NO

# WS HowHigh
# Values = YES, NO
WS_HOW_HIGH = YES

# Gauges in Telemetry screen
# Values = YES, NO
GAUGES = YES

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

# Hardware volume
# Values = YES, NO
# default = YES (AR9X board doesn't have a hardware volume control as delivered but it can be fitted optionally)
HARDWARE_VOLUME = YES

# PXX (FrSky PCM) protocol
PXX = NO

# DSM2 (Spektrum) protocol
# Values = NO, SERIAL, PPM
DSM2 = NO

# HUBSAN protocol
# Values = NO, YES
HUBSAN = NO

# TRANSLATIONS
# Values = cz, de, en, fi, fr, it, se, es, pl, pt, nl
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

# Shutdown confirmation (for boards which have soft power)
# Values = NO, YES
SHUTDOWN_CONFIRMATION = NO

# Activates the DEBUG Mode (all boards)
# Values = YES, NO
DEBUG = NO

# Activate writing of SPORT telemetry received data to sport.log file
# Values = YES, NO
SPORT_FILE_LOG = NO

# Timers Count
# Values = 1, 2, 3 (on ARM boards)
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

# Enable another 3 positions switch to replace a pot
# Values = NO, POT1, POT2, POT3,
3POS = NO

# Allow to change buttons names (only Mega2560)
# Values = NO, YES with personames.h edited
PERSONAMES = NO

#Use external eeprom FM24W256
EXTERNALEEPROM = NO

# Menu Lock modes
# Values = NO, READONLY, NOMENUS
MENUS_LOCK = NO

# Mixers monitor view added to Channels monitor one
# Values = NO, YES
# YES - Adds mixers output view to Channels monitor screen,
# pressing [ENT] switches between views.
# NO - classic Channels monitor
MIXERS_MONITOR = NO

# Use newlib-nano
# Values = NO, YES
NANO = NO

# Use custom bin based allocator for Lua
# Values = NO, YES
# YES - use our bin based allocator
# NO - use default Lua allocator
USE_BIN_ALLOCATOR = NO

# Enable trace of events into Trace Buffer for debugging purposes
# Activating any of these options also activates DEBUG and DEBUG_TRACE_BUFFER
# Values = NO, YES
#   TRACE_SD_CARD     SD card operations (read, write, etc)
#   TRACE_FATFS       FatFs file operations
#   TRACE_AUDIO       Audio processing (not yet implemented)
TRACE_SD_CARD = NO
TRACE_FATFS = NO
TRACE_AUDIO = NO

# Support for D16-EU only (no D8, no LR12 which are not EU compatible)
# Value = NO, YES
SUPPORT_D16_EU_ONLY = NO

# Enable Watchdog test (DANGEROUS)
# Values = NO, YES
WATCHDOG_TEST = NO

# Treat compiler warnings as errors
# Values = NO, YES
WARNINGS_AS_ERRORS = NO


#------- END BUILD OPTIONS ---------------------------

# Define programs and commands.
SHELL = sh

IMG2LBM = -python ../util/img2lbm.py
BIN2LBM = -python ../util/bin2lbm.py
FONT2PNG = -python ../util/font2png.py
TRANSLATE = -python ../util/translate.py

VERSION_MAJOR = 2
VERSION_MINOR = 1
VERSION_REVISION = 11

VERSION = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_REVISION)$(OpenAVRc_VERSION_SUFFIX)

THIRDPARTY = thirdparty
FATFSDIR = $(THIRDPARTY)/FatFs

SRC =
CPPSRC =
INCDIRS = . translations
LUADEP =
DEBUG_TRACE_BUFFER = NO

# PCB name
ifneq ($(PCB), $(filter $(PCB), MEGA2560 EVO_V1))
  $(error Sorry, the PCB $(PCB) is not supported yet in OpenAVRc)
endif

GVARS_VARIANT   = +1
FRSKY_VARIANT   = +2
3POS_VARIANT    = +4
MAVLINK_VARIANT = +8
EEPROM_VARIANT  = 0

GUIMODELSRC = gui/menu_model.cpp gui/menu_model_select.cpp gui/menu_model_setup.cpp gui/menu_model_inputs_mixes.cpp gui/menu_model_limits.cpp gui/menu_model_logical_switches.cpp gui/menu_model_custom_functions.cpp gui/menu_model_telemetry.cpp
GUIGENERALSRC = gui/menu_general.cpp gui/menu_general_setup.cpp gui/menu_general_trainer.cpp gui/menu_general_version.cpp gui/menu_general_calib.cpp

BITMAPS = bitmaps/sticks.lbm

#FONTS = $(patsubst %.png,%.lbm,$(wildcard fonts/std/*.png))

ifeq ($(PCB), EVO_V1)
  # Evo V1 PCB with Mega2560

  GUIDIRECTORY = 9X
  FLAVOUR = mega2560
  ARCH = AVR
  LCDSIZE = 128
  TRGT = avr-
  MCU = atmega2560
  OPT = s
  # Processor frequency.
  F_CPU = 16000000
  CPPDEFS = -DF_CPU=$(F_CPU)UL -DPCBMEGA2560 -DCPUM2560 -DREV_EVO_V1 -DEEPROM_RLC -DEEPROM_VARIANT=0
  SDCARD = YES
  BUZZER = NO
  EXTERNALEEPROM = YES
  AUDIO = YES
  VOICE = NO
  HAPTIC = NO
  THR_TRACE = YES
  PERSONAMES = YES
  PWRMANAGE = NO
  
  INCDIRS += targets/evo_v1 targets/stock $(FATFSDIR) $(FATFSDIR)/option
  BOARDSRC = main_avr.cpp i2c_master.cpp spi.c targets/evo_v1/evo_mega2560.cpp targets/evo_v1/evo_lcd_driver.cpp targets/evo_v1/evo_adc_driver.cpp
  EEPROMSRC = eeprom_common.cpp eeprom_rlc.cpp
  PULSESSRC = pulses/pulses_avr.cpp
# PULSESSRC = targets/evo_v1/evo_pulses_avr.cpp
  BITMAPS += bitmaps/splash.lbm bitmaps/asterisk.lbm
  GUIGENERALSRC += gui/menu_general_diagkeys.cpp targets/evo_v1/menu_general_diaganas.cpp
  EXTRABOARDSRC = telemetry_driver.cpp

  ifeq ($(PWRMANAGE), YES)
    CPPDEFS += -DPWRMANAGE
  endif

  ifeq ($(SDCARD), YES)
    EXTRABOARDSRC += $(FATFSDIR)/ff.c $(FATFSDIR)/option/ccsbcs.c diskio.cpp
    CPPDEFS += -DSDCARD -DRTCLOCK
    GUIGENERALSRC += gui/menu_general_sdmanager.cpp
    CPPSRC += rtc.cpp sdcard.cpp logs.cpp rtc_driver.cpp
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
    CPPSRC += somo14d.cpp
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

  # Enable extra rotary encoders
  ROTARY_ENCODERS = 2
  CPPDEFS += -DROTARY_ENCODERS=${ROTARY_ENCODERS}
endif


ifeq ($(PCB), MEGA2560)
  # ARDUINO2560 DIY RADIO

  FLAVOUR = mega2560
  ARCH = AVR
  LCDSIZE = 128
  TRGT = avr-
  MCU = atmega2560
  OPT = s
  # Processor frequency.
  F_CPU = 16000000
  CPPDEFS = -DF_CPU=$(F_CPU)UL -DPCBMEGA2560 -DCPUM2560 -DEEPROM_RLC -DEEPROM_VARIANT=0
  # -DAUDIO Removed by Bracame
  SDCARD = NO
  BUZZER = YES
  THR_TRACE = YES
  PWRMANAGE = NO
  INCDIRS += targets/mega2560 targets/stock $(FATFSDIR) $(FATFSDIR)/option
  BOARDSRC = main_avr.cpp i2c_master.cpp spi.c targets/mega2560/board_mega2560.cpp
  EEPROMSRC = eeprom_common.cpp eeprom_rlc.cpp
  PULSESSRC = pulses/pulses_avr.cpp
  BITMAPS += bitmaps/splash.lbm bitmaps/asterisk.lbm
  GUIGENERALSRC += gui/menu_general_diagkeys.cpp gui/menu_general_diaganas.cpp

  EXTRABOARDSRC = adc_driver.cpp targets/lcd/lcd_driver.cpp telemetry_driver.cpp

  ifeq ($(PWRMANAGE), YES)
    CPPDEFS += -DPWRMANAGE
  endif

  ifeq ($(SDCARD), YES)
    EXTRABOARDSRC += $(FATFSDIR)/ff.c $(FATFSDIR)/option/ccsbcs.c diskio.cpp
    CPPDEFS += -DSDCARD -DRTCLOCK
    GUIGENERALSRC += gui/menu_general_sdmanager.cpp
    CPPSRC += rtc.cpp sdcard.cpp logs.cpp rtc_driver.cpp
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

  ifeq ($(VOICE), WTV20SD)
  CPPDEFS += -DVOICE
  CPPDEFS += -DVOICE_WTV20SD
  CPPSRC += targets/mega2560/voice_wtv20sd.cpp
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

  # Enable extra rotary encoders
  ROTARY_ENCODERS = 2
  CPPDEFS += -DROTARY_ENCODERS=${ROTARY_ENCODERS}
endif

CC      = $(TRGT)gcc
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

ifeq ($(GUI), YES)
  GUISRC = gui/navigation.cpp gui/popups.cpp gui/widgets.cpp gui/menus.cpp $(GUIMODELSRC) $(GUIGENERALSRC) gui/view_main.cpp gui/view_statistics.cpp
  CPPDEFS += -DGUI
endif

CPPSRC += OpenAVRc.cpp functions.cpp strhelpers.cpp $(PULSESSRC) switches.cpp curves.cpp mixer.cpp stamp.cpp $(GUISRC) $(EEPROMSRC) timers.cpp trainer_input.cpp

ifeq ($(GUI), YES)
  GUISRC += gui/lcd.cpp gui/splash.cpp gui/fonts.cpp
endif

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

ifeq ($(TURNIGY_TRANSMITTER_FIX), YES)
    CPPDEFS += -DMODULE_ALWAYS_SEND_PULSES
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
endif

ifeq ($(AUTOSWITCH), YES)
  CPPDEFS += -DAUTOSWITCH
endif

ifeq ($(AUTOSOURCE), YES)
  CPPDEFS += -DAUTOSOURCE
endif

ifeq ($(FRSKY_STICKS), YES)
  CPPDEFS += -DFRSKY_STICKS
endif

ifeq ($(DBLKEYS), YES)
  ifeq ($(PCB), $(filter $(PCB), 9XR 9XR128 9XR2561))
    $(warning Sorry, DBLKEYS doesnt work on 9XR)
  else
    CPPDEFS += -DDBLKEYS
  endif
endif

ifeq ($(SPLASH), YES)
  CPPDEFS += -DSPLASH
endif

ifeq ($(SPLASH), FIXED)
  CPPDEFS += -DSPLASH -DFSPLASH
endif

ifeq ($(GRAPHICS), YES)
  CPPDEFS += -DGRAPHICS
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
  EEPROM_VARIANT += ${GVARS_VARIANT}
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

ifeq ($(ARITHMETIC_OVERFLOW_CHECK), YES)
  CPPDEFS += -DPREVENT_ARITHMETIC_OVERFLOW
endif

ifeq ($(ACCURAT_THROTTLE_STATS), YES)
  CPPDEFS += -DACCURAT_THROTTLE_TIMER
endif

# EXTENSIONS (for Telemetry protocols)
ifneq ($(EXT), $(filter $(EXT), STD FRSKY FRSKY_SPORT ARDUPILOT NMEA JETI MAVLINK TELEMETREZ))
  $(error Sorry, the EXT $(EXT) is not supported yet in OpenAVRc)
endif

ifeq ($(EXT), STD)
  CPPDEFS += -DEXTSTD
endif

ifeq ($(EXT), ARDUPILOT)
  CPPDEFS += -DARDUPILOT
  CPPSRC += telemetry/ardupilot.cpp
endif

ifeq ($(EXT), NMEA)
  CPPDEFS += -DNMEA
  CPPSRC += telemetry/nmea.cpp
endif

ifeq ($(EXT), JETI)
  CPPDEFS += -DJETI
  CPPSRC += telemetry/jeti.cpp
endif

ifeq ($(EXT), MAVLINK)
  CPPDEFS += -DMAVLINK
  INCDIRS += thirdparty
  CPPSRC += telemetry/mavlink.cpp gui/view_mavlink.cpp serial_driver.cpp
  EEPROM_VARIANT += ${MAVLINK_VARIANT}
endif

ifeq ($(EXT), TELEMETREZ)
  CPPDEFS += -DTELEMETREZ
endif

ifeq ($(EXT), $(filter $(EXT), FRSKY FRSKY_SPORT TELEMETREZ))
  CPPDEFS += -DFRSKY
  CPPSRC += telemetry/frsky.cpp telemetry/frsky_d.cpp

  ifeq ($(EXT), FRSKY_SPORT)
    CPPSRC += telemetry/frsky_sport.cpp
    CPPSRC += crc16.cpp
    CPPDEFS += -DFRSKY_SPORT
  endif

  CPPSRC += gui/view_telemetry.cpp

  ifeq ($(FRSKY_HUB), YES)
    CPPDEFS += -DFRSKY_HUB
  endif

  ifeq ($(WS_HOW_HIGH), YES)
    CPPDEFS += -DWS_HOW_HIGH
  endif

  ifeq ($(GAUGES), YES)
    CPPDEFS += -DGAUGES
  endif

  ifeq ($(GPS), YES)
    CPPDEFS += -DGPS
  endif

  ifeq ($(VARIO), YES)
    CPPSRC += vario.cpp
    CPPDEFS += -DVARIO
  endif
  EEPROM_VARIANT += ${FRSKY_VARIANT}
endif

ifeq ($(SIMU), YES)
  CPPDEFS += -DDEBUG
  CPPSRC += dump.cpp
else ifeq ($(CLI), YES)
  CPPDEFS += -DCLI -DDEBUG
  CPPSRC += cli.cpp dump.cpp
else ifeq ($(DEBUG), YES)
  CPPDEFS += -DDEBUG
  CPPSRC += dump.cpp
endif

ifeq ($(EEPROM_PROGRESS_BAR), YES)
  CPPDEFS += -DEEPROM_PROGRESS_BAR
endif

ifeq ($(SHUTDOWN_CONFIRMATION), YES)
  CPPDEFS += -DSHUTDOWN_CONFIRMATION
endif

RUN_FROM_FLASH = 1

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

ifeq ($(DANGEROUS_MODULE_FUNCTIONS), YES)
  CPPDEFS += -DDANGEROUS_MODULE_FUNCTIONS
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

ifeq ($(HELI), YES)
  CPPDEFS += -DHELI
  GUIMODELSRC += gui/menu_model_heli.cpp
endif

ifeq ($(TEMPLATES), YES)
    CPPDEFS += -DTEMPLATES
    CPPSRC += templates.cpp
    GUIMODELSRC += gui/menu_model_templates.cpp
endif

ifeq ($(PXX), YES)
  CPPDEFS += -DPXX
endif

ifeq ($(DSM2), SERIAL)
  CPPDEFS += -DDSM2 -DDSM2_SERIAL
endif

ifeq ($(DSM2), PPM)
  CPPDEFS += -DDSM2 -DDSM2_PPM
endif

ifeq ($(TX_CADDY), YES)
  CPPDEFS += -DTX_CADDY
endif

ifeq ($(IRPROTOS), YES)
  CPPDEFS += -DIRPROTOS
endif

ifeq ($(3POS), POT1)
  CPPDEFS += -DEXTRA_3POS=1
  # EEPROM_VARIANT += ${3POS_VARIANT}
endif
ifeq ($(3POS), POT2)
  CPPDEFS += -DEXTRA_3POS=2
  # EEPROM_VARIANT += ${3POS_VARIANT}
endif
ifeq ($(3POS), POT3)
  CPPDEFS += -DEXTRA_3POS=3
  # EEPROM_VARIANT += ${3POS_VARIANT}
endif

#---------------- Compiler Options C++ ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CPPFLAGS = -g$(DBGFMT)
CPPFLAGS += $(CPPDEFS)
CPPFLAGS += -Wall
CPPFLAGS += -mrelax
CPPFLAGS += -fno-exceptions
CPPFLAGS += -Wno-strict-aliasing
CPPFLAGS += -Wformat -Wreturn-type -Wunused -Wuninitialized -Wunknown-pragmas -Wno-switch -Wtype-limits
INCFLAGS = $(patsubst %,-I%,$(INCDIRS))

ifeq ($(WARNINGS_AS_ERRORS), YES)
  CPPFLAGS += -Werror
endif

MATH_LIB = -lm

#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref,-Map,OpenAVRc.map
LDFLAGS += $(MATH_LIB)

# Define Messages
# English
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before:
MSG_SIZE_AFTER = Size after:
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_COMPILING = Compiling C++:
MSG_CLEANING = Cleaning project:

# Combine all necessary flags and optional flags.
# Add target processor to flags.

CPPFLAGS += -DFLAVOUR=\"$(FLAVOUR)\"
ARMCPPFLAGS = -c -mcpu=$(MCU) -mthumb -fomit-frame-pointer -fverbose-asm -Wa,-ahlms=OpenAVRc.lst -DRUN_FROM_FLASH=1 -O$(OPT) $(CPPFLAGS) -fdata-sections -ffunction-sections $(INCFLAGS) $(GENDEPFLAGS)
AVRCPPFLAGS = -mmcu=$(MCU) -I. -x c++ -O$(OPT) $(CPPFLAGS) $(INCFLAGS) $(GENDEPFLAGS) -fwhole-program

ifeq ($(ARCH), AVR)
  GCCVERSIONGTE462 := $(shell expr 4.6.2 \<= `$(CC) -dumpversion`)
  ifeq ($(GCCVERSIONGTE462),1)
    CPPFLAGS += -flto
  endif

  AVRCPPFLAGS += -fno-inline-small-functions

  GCCVERSIONGTE470 := $(shell expr 4.7.0 \<= `$(CC) -dumpversion`)
  ifeq ($(GCCVERSIONGTE470),1)
    AVRCPPFLAGS += -mstrict-X
  endif
endif

MAJ_VER = ${shell sh -c "grep \"MAJ_VERS\" OpenAVRc.h | cut -d\  -f3 | egrep -o \"[[:digit:]]\""}
MIN_VER = ${shell sh -c "grep \"MIN_VERS\" OpenAVRc.h | cut -d\  -f3"}

.SECONDARY:

# Default target.
all: begin gccversion sizebefore build sizeafter end

# Change the build target to build a HEX file or a library.
ifeq ($(ARCH), AVR)
  build: stamp_header tra lbm allsrc.cpp elf remallsrc hex
  build: stamp_header tra lbm allsrc.cpp elf remallsrc bin
endif

tra: translations/en.h translations/cz.h translations/de.h translations/es.h translations/fi.h translations/fr.h translations/it.h translations/nl.h translations/pl.h translations/pt.h translations/se.h
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

FONTNAME = Roboto

.PHONY: fonts
fonts: $(FONTS)

fonts/%.lbm: fonts/%.png
	$(IMG2LBM) $^ $@ $(LCDSIZE)

fonts/font_tinsize.lbm:
	$(FONT2PNG) $(FONTNAME) 5 fonts/font_tinsize.png
	$(IMG2LBM) fonts/font_tinsize.png $@ $(LCDSIZE) 4bits

fonts/font_stdsize.lbm:
	$(FONT2PNG) $(FONTNAME) 8 fonts/font_stdsize.png
	$(IMG2LBM) fonts/font_stdsize.png $@ $(LCDSIZE) 4bits

fonts/font_midsize.lbm:
	$(FONT2PNG) $(FONTNAME) 10 fonts/font_midsize.png
	$(IMG2LBM) fonts/font_midsize.png $@ $(LCDSIZE) 4bits

fonts/font_dblsize.lbm:
	$(FONT2PNG) $(FONTNAME) 14 fonts/font_dblsize.png
	$(IMG2LBM) fonts/font_dblsize.png $@ $(LCDSIZE) 4bits

.PHONY: bitmaps
bitmaps: $(BITMAPS)

bitmaps/sticks.lbm: bitmaps/sticks.xbm
	$(IMG2LBM) $^ $@ $(LCDSIZE) 1bit 4

bitmaps/%.lbm: bitmaps/%.png
	$(IMG2LBM) $^ $@ $(LCDSIZE) 1bit

ifeq ($(OS),Windows_NT)
  PARSER = avr-cpp
else
  PARSER = gcc -E -x c++
endif

# Eye candy.
# AVR Studio 3.x does not check make's exit code but relies on
# the following magic strings to be generated by the compile job.

begin:
	@echo
	@echo $(MSG_BEGIN)

end:
	@echo $(MSG_END)
	@echo

# Display size of file.
ifeq ($(ARCH), AVR)
  MCU_FLAG = $(shell $(SIZE) --help | grep -- --mcu > /dev/null && echo --mcu=$(MCU) )
  FORMAT_FLAG = $(shell $(SIZE) --help | grep -- --format=.*avr > /dev/null && echo --format=avr )
  ELFSIZE = $(SIZE) $(MCU_FLAG) $(FORMAT_FLAG) $(TARGET).elf
else
  ELFSIZE = $(SIZE) $(TARGET).elf
endif

sizebefore:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); fi

sizeafter:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); fi

# Display compiler version information.
gccversion:
	@$(CC) --version

FOXINC=-I/usr/local/include/fox-1.6 -I/usr/include/fox-1.6 \
       -I$(FOXPATH)/include \
       -I/opt/local/include/fox-1.6
FOXLIB=-L/usr/local/lib \
       -L$(FOXPATH)/src/.libs \
       -L/opt/local/lib \
       -lFOX-1.6 \
       -Wl,-rpath,$(FOXPATH)/src/.libs

SIMUDEFS = -DSIMU -DLUA_USE_APICHECK
ifeq ($(SIMU_AUDIO), YES)
  AUDIOLIB = -L/usr/lib/x86_64-linux-gnu/ -lSDLmain -lSDL
  AUDIOINC = -I/usr/include/SDL
  SIMUDEFS += -DSIMU_AUDIO
endif

ifeq ($(SIMU_DISKIO), YES)
  CPPSRC += $(FATFSDIR)/ff.c $(FATFSDIR)/option/ccsbcs.c
  SIMUDEFS += -DSIMU_DISKIO
endif

simu: $(LUADEP) stamp_header allsimusrc.cpp Makefile simu.cpp targets/simu/simpgmspace.cpp *.h tra lbm eeprom.bin
	g++ $(CPPFLAGS) $(SIMUCPPFLAGS) $(INCFLAGS) simu.cpp allsimusrc.cpp $(LUASRC) targets/simu/simpgmspace.cpp -MD $(SIMUDEFS) -O0 -o simu $(FOXINC) $(FOXLIB) $(AUDIOINC) $(AUDIOLIB) -pthread -fexceptions

eeprom.bin:
	dd if=/dev/zero of=$@ bs=1 count=2048


# Create final output files (.hex, .eep) from ELF output file.
ifeq ($(ARCH), AVR)
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@
else
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O ihex OpenAVRc.elf OpenAVRc.hex
endif

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

allsimusrc.cpp: Makefile $(BOARDSRC) $(CPPSRC)
	@rm -f allsimusrc.cpp
	for f in $(BOARDSRC) $(CPPSRC) ; do echo "# 1 \"$$f\"" >> allsimusrc.cpp; cat "$$f" >> allsimusrc.cpp; done

remallsrc:
	$(REMOVE) allsrc.cpp
	$(REMOVE) allsimusrc.cpp

# Link: create ELF output file from object files.
ifeq ($(ARCH), AVR)
OpenAVRc.elf: allsrc.cpp
	@echo
	@echo $(MSG_COMPILING) $@
	$(CC) $(AVRCPPFLAGS) $< --output $@ $(LDFLAGS)
else
%.o: %.c
	$(CC) $(ARMCPPFLAGS) -Wimplicit $< -o $@
%.o: %.s
	$(AS) $< -o $@
TMP = $(SRC:.c=.o)
OBJS =  $(TMP:.s=.o)
OpenAVRc.elf: allsrc.cpp $(OBJS)
	@echo
	@echo $(MSG_COMPILING) $@
	$(CC) $(ARMCPPFLAGS) $< -o allsrc.o
	$(CC) $(OBJS) allsrc.o -mcpu=$(MCU) -mthumb -nostartfiles -lm -T$(LDSCRIPT) -Wl,-Map=$(TARGET).map,--cref,--no-warn-mismatch,--gc-sections $(NEWLIB_NANO_FLAGS) -o $@
endif

# Target: clean project.
clean: begin clean_list end

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
	$(REMOVE) allsimusrc.cpp
	$(REMOVE) stamp-OpenAVRc.h
#	$(REMOVE) *.lbm
#	$(REMOVE) bitmaps/*.lbm
#	$(REMOVE) bitmaps/*/*.lbm
#	$(REMOVE) fonts/*.lbm
#	$(REMOVE) fonts/*/*.lbm
#	$(REMOVE) translations/*.h

#### Install

DATE = ${shell sh -c "date +%F:%T" }
AVRCOMMAND = $(AVRDUDE) $(AVRDEVICE)

WRITE_COMMAND = $(AVRCOMMAND) -U flash:w:OpenAVRc.hex:i
ifeq ($(RESTORE_EEPROM), YES)
  BACKUP_EEPROM = YES
  ifeq ($(DOUBLE_WRITE_OK), YES)
    WRITE_COMMAND += -U eeprom:w:$(DATA_DIR)/eeprom-$(DATE).hex:i
  else
    SECOND_WRITE_COMMAND = -U eeprom:w:$(DATA_DIR)/eeprom-$(DATE).hex:i
  endif
endif

ifeq ($(BACKUP_FLASH), YES)
  BACKUP_ARGS += -U eeprom:r:$(DATA_DIR)/eeprom-$(DATE).hex:i $(BACKUPFLASH)
endif
ifeq ($(BACKUP_EEPROM), YES)
  BACKUP_ARGS += -U flash:r:$(DATA_DIR)/OpenAVRc-$(DATE).hex:i
endif

install: all
ifdef BACKUP_ARGS
	$(AVRCOMMAND) $(BACKUP_ARGS)
	$(SLEEP) $(DELAY)
endif
	$(WRITE_COMMAND)
ifdef	SECOND_WRITE_COMMAND
	$(SLEEP) $(DELAY)
	$(SECOND_WRITE_COMMAND)
endif

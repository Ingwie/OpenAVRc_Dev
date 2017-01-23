# ----------------------------------------------------------------------------
# Makefile for project stk500boot
# Author: Peter Fleury
# File:   $Id: Makefile,v 1.7 2015/08/15 09:10:38 peter Exp $
#
# Adjust MCU, F_CPU and BOOTLOADER_ADDRESS below to your AVR target
#----------------------------------------------------------------------------
# usage:
#
# make          = build software
# make clean    = Clean out built project files
# make program  = Download the hex file to the device, using avrdude.
#                 Please customize the avrdude settings below first!
#
# make filename.s = Just compile filename.c into the assembler code only.
# make filename.i = Create a preprocessed source file
#
# To rebuild project do "make clean" then "make"
#----------------------------------------------------------------------------


# MCU name
MCU = atmega2560


# Processor frequency.
#     This will define a symbol, F_CPU, in all source code files equal to the
#     processor frequency. You can then use this symbol in your source code to
#     calculate timings. Do NOT tack on a 'UL' at the end, this will be done
#     automatically to create a 32-bit value in your source code.
F_CPU = 16000000


# Target file name (without extension).
TARGET = BootFramM2560


# Bootloader
# Please adjust if using a different AVR using following formula:
# BOOTLOADER_ADDRESS =  2 * ( (AVR-Flash size in words) - (boot-size in words) )
# e.g. for ATmega8 and 512 words boot size:  2 * ( 4096 - 512 )   or  2 * ( 0x1000 - 0x200 ) = 0x1C00
#      for ATmega32 and 512 words boot size: 2 * ( 16384 - 512 )  or  2 * ( 0x4000 - 0x200 ) = 0x7C00
BOOTLOADER_ADDRESS = 0x3E000


# List C source files here. (C dependencies are automatically generated.)
SRC = $(TARGET).c


# List Assembler source files here.
#     Make them always end in a capital .S.  Files ending in a lowercase .s
#     will not be considered source files but generated files (assembler
#     output from the compiler), and will be deleted upon "make clean"!
#     Even though the DOS/Win* filesystem matches both .s and .S the same,
#     it will preserve the spelling of the filenames, and gcc itself does
#     care about how the name is spelled on its command-line.
#ASRC =


# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
#EXTRAINCDIRS =


#specify alternate location of sources files
#     (unlike VPATH= which is a search path for all prerequisites, not just source files)


# Optimization level, can be [0, 1, 2, 3, s].
#     0 = turn off optimization. s = optimize for size.
#     (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT = s


# Place -D or -U options here
CDEFS = -DBOOTLOADER_ADDRESS=$(BOOTLOADER_ADDRESS)


# Place -I options here
#CINCS =


#---------------- Compiler Options ----------------
#  -gdwarf-2:    generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CFLAGS  = -DF_CPU=$(F_CPU)UL
CFLAGS += $(CDEFS) $(CINCS)
CFLAGS += -I. $(patsubst %,-I%,$(EXTRAINCDIRS))
CFLAGS += -std=gnu99
CFLAGS += -gdwarf-2
CFLAGS += -O$(OPT)
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -Wa,-adhlns=$(<:.c=.lst)
#CFLAGS += -save-temps
CFLAGS += -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions -fno-jump-tables


#---------------- Assembler Options ----------------
#  -Wa,...:   tell GCC to pass this to the assembler.
#   -adhlns=$(<:.S=.lst)  create listing
#   -gdwarf-2:            generate debugging information
ASFLAGS = -DF_CPU=$(F_CPU)UL -x assembler-with-cpp -Wa,-gdwarf2

#---------------- Library Options ----------------
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

# If this is left blank, then it will use the Standard printf version.
PRINTF_LIB =
#PRINTF_LIB = $(PRINTF_LIB_MIN)
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)


# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB =
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)


MATH_LIB = -lm



#---------------- External Memory Options ----------------

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--defsym=__heap_start=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =



#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)


#--------------- bootloader linker Options -------
# BOOTLOADER_ADDRESS (=Start of Boot Loader section
# in bytes - not words) is defined above.
LDFLAGS += -Wl,--section-start=.text=$(BOOTLOADER_ADDRESS) -nostartfiles -nodefaultlibs


#---------------- Programming Options (avrdude) ----------------

# Programming hardware:   Type: avrdude -c ? to get a full listing.
AVRDUDE_PROGRAMMER = usbasp

# usb, com1 = serial port, lpt1 = parallel port
AVRDUDE_PORT = USB

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep

# Uncomment the following if you do /not/ wish a verification to be performed after programming the device.
#AVRDUDE_NO_VERIFY = -V

# Disable save mode for fuses
AVRDUDE_NO_SAFEMODE = -u

# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
#AVRDUDE_ERASE_COUNTER = -y

# Increase verbosity level.
#AVRDUDE_VERBOSE = -v -v

# Adjust programming speed of USBasp
#  no -B switch 375khz (default)
#  -B 2000    	500 hz
#  -B 1000 			1khz
#  -B 500 			2khz
#  -B 250 			4khz
#  -B 125 			8 khz
#  -B 62 			  16khz
#  -B 31 				32khz * the cutoff for bit banged isp
#  -B 10 				93.75 khz
#  -B 5 			  187.5 khz
#  -B 2 				375 khz
#  -B 1 				750 khz
#  -B .5 			  1.5mhz
AVRDUDE_SPEED = -B .5

AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_NO_SAFEMODE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)
AVRDUDE_FLAGS += $(AVRDUDE_SPEED)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)



#============================================================================
# Output format. (can be srec, ihex, binary)
FORMAT = ihex

# Define programs and commands.
#SHELL = win-bash
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -rf
COPY = cp
WINSHELL = cmd


# Define Messages
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:



# Define all object files.
OBJ = $(SRC:.c=.o) $(ASRC:.S=.o)

# Define all listing files.
LST = $(SRC:.c=.lst) $(ASRC:.S=.lst)


# Compiler flags to generate dependency files.
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d


# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) $(CFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. $(ASFLAGS)


# Default target.
all: gccversion $(TARGET).elf $(TARGET).hex $(TARGET).eep $(TARGET).lss $(TARGET).sym size


# Display compiler version information.
gccversion :
	@echo $(OBJ1)
	@$(CC) --version


# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

%.eep: %.elf
	@echo $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@

# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)
%.elf: $(OBJ)
	@echo $(MSG_LINKING) $@
	$(CC) -mmcu=$(MCU) $(LDFLAGS) $^ --output $(@F)

# Compile: create object files from C source files.
%.o : %.c
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(ALL_CFLAGS) $< -o $(@F)

# Compile: create assembler files from C source files.
%.s : %.c
	$(CC) -S $(ALL_CFLAGS) $< -o $(@F)

# Assemble: create object files from assembler source files.
%.o : %.S
	@echo $(MSG_ASSEMBLING) $<
	$(CC) -c $(ALL_ASFLAGS) $< -o $(@F)

# Create preprocessed source for use in sending a bug report.
%.i : %.c
	$(CC) -E -mmcu=$(MCU) -I. $(CFLAGS) $< -o $(@F)


# Display size of file.
size: ${TARGET}.elf
	@avr-size -C --mcu=${MCU} ${TARGET}.elf


# Program the device.
program: $(TARGET).hex $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)


# Delete all generated files.
clean:
	@echo $(MSG_CLEANING)
	$(REMOVE) $(TARGET).hex $(TARGET).eep $(TARGET).cof $(TARGET).elf $(TARGET).map $(TARGET).sym $(TARGET).lss $(OBJ) $(LST) $(SRC:.c=.s) $(SRC:.c=.d) $(SRC:.c=.i) .dep/


# Include the dependency files.
#-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
-include $(shell mkdir .dep 2>NUL) $(wildcard .dep/*)


# Listing of phony targets.
.PHONY : all size gccversion clean program


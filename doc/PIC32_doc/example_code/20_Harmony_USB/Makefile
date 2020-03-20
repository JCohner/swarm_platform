XC32PATH=
HARMONYPATH=
NU32PATH=
PORT=
TERMEMU=
CONFIG=pic32_NU32

PROC = 32MX795F512H
#This file is used to:
#	1. compile .c files into .o files
#	2. link the .o files into a .elf binary
#	3. convert the .elf into a .hex
#	4. write the .hex file to the pic using the bootloader utility
# The following must be edited by you so that
# make knows where to find the compiler tools (xc32)
# and the harmony library.
#
# XC32PATH is the path to the xc32 bin directory. This directory contains
# tools such as the compiler (xc32-gcc) and hex converter 
# (xc32-bin2hex)
# Windows example (note the use of slashes not backslashes):
# XC32PATH=C:/Program Files (x86)/Microchip/xc32/v1.40/bin
# Mac example:
# XC32PATH =/Applications/microchip/xc32/v1.40/bin
#
# HARMONYPATH is the path to the harmony framework directory
# Windows example:
# HARMONYPATH=C:/Program Files (x86)/Microchip/harmony/v1_06
# Mac example:
# HARMONYPATH=/Users/kevin/harmony/v1_06
#
# NU32PATH is the path to the nu32utility.
# You compiled this and named it nu32utility
# Example:
# if your nu32utility is in
# /home/student/PIC32/nu32utilty, then
# NU32PATH=/home/student/PIC32
#
# PORT is the COM port used by the PIC32
# Windows Example:
# PORT=\\.\COM22
# Mac Example:
# PORT=/dev/tty.usbserial=00001024A
#
# TERMEMU is your terminal emulator
# Windows example:
# TERMEMU=C:/Program Files (x86)/PuTTY/putty.exe
# Mac example (screen is pre-installed and already on your execution path 
# so you can safely omit the full path)
# TERMEMU=screen
#
# CONFIG - the config directory inside system_config/ that has the desired system configuration
#
#the c compiler
CC="$(XC32PATH)/xc32-gcc"

#the hexfile maker
HX="$(XC32PATH)/xc32-bin2hex"

#object dumper
OBJDMP="$(XC32PATH)/xc32-objdump"

#the bootloader utility
WRITE="$(NU32PATH)/nu32utility"

SYSTEM_CONFIG=system_config/$(CONFIG)

#the linker script. Try to use a linker script from the system configuration directory,
#if it is not there, fallback on NU32bootloaded.ld
LINKSCRIPT=$(wildcard $(SYSTEM_CONFIG)/*.ld)
ifeq "$(LINKSCRIPT)" ""
	LINKSCRIPT="NU32bootloaded.ld"
endif

#get all the c files in the system_config directory.  nest 3 levels deep to allow people to use the full harmony structure
SYSFILES= $(wildcard $(SYSTEM_CONFIG)/framework/*.c) $(wildcard $(SYSTEM_CONFIG)/framework/*/*.c) $(wildcard $(SYSTEM_CONFIG)/framework/*/*/*.c) $(wildcard $(SYSTEM_CONFIG)/*.c)

#only need the headers in the base system_config directory since the others are not going to change since they are part of harmony
#this is used for build dependency purposes
SYSHDRS=$(wildcard $(SYSTEM_CONFIG)/*.h)

#strip filenames from the files used so we add those subdirs to the include path
#also add -I before each filename
INCPATH=$(patsubst %,-I"%",$(dir $(SYSFILES)))

OBJS := $(patsubst %.c, %.o,$(wildcard *.c)) $(patsubst %.c, %.o, $(SYSFILES))
HDRS := $(wildcard *.h) $(SYSHDRS)
CFLAGS=-g -O1 -x c
TARGET = out

#if on windows use a different RM
ifdef ComSpec
	RM = del /Q
endif

#what to do when make all
.PHONY : all
all : $(TARGET).hex $(TARGET).dis

# Turn the elf file into a hex file.
$(TARGET).hex : $(TARGET).elf
	@echo Creating hex file
	$(HX) $(TARGET).elf

$(TARGET).dis : $(TARGET).elf
	$(OBJDMP) -S $< > $@

# Link all the object files into an elf file.
$(TARGET).elf : $(OBJS)
	@echo Linking elf file
	$(CC) -mprocessor=$(PROC) -o $(TARGET).elf $(OBJS) \
		-l:"$(HARMONYPATH)/bin/framework/peripheral/PIC$(PROC)_peripherals.a" \
	-Wl,--script=$(LINKSCRIPT),-Map=$(TARGET).map

# Create an object file for each C file.
$(OBJS) : %.o : %.c $(HDRS) $(SYSHDRS)
	@echo Creating object file $@
	$(CC) $(CFLAGS) -c -mprocessor=$(PROC) -I"$(HARMONYPATH)/framework" -I. $(INCPATH) -o $@ $<
#path to harmony framework, harmony peripherals, harmony dsp library, harmony libq
.PHONY: clean
# Erase all hex, map, object, and elf files.
clean :
	$(RM) *.hex *.map *.o *.elf *.dep *.dis       
	$(RM) $(OBJS) 
.PHONY: write
# After making, call the NU32utility to program via bootloader.
write : $(TARGET).hex $(TARGET).dis
	$(WRITE) "$(PORT)" $(TARGET).hex 

.PHONY: putty
putty :
	"$(TERMEMU)" -serial -sercfg 230400,R "$(PORT)"

.PHONY: screen
screen :
	"$(TERMEMU)" "$(PORT)" 230400,crtscts

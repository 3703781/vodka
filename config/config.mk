ABSPROJDIR := /home/miaow/m7proj-h7/

PREFIX := /home/miaow/software/arm-none-eabi-13.2.rel1/bin/arm-none-eabi-

DEBUGGER := JLINK
# DEBUGGER := STLINK

CUBEPROGDIR := /opt/st/stm32cubeclt/STM32CubeProgrammer/bin/
JLINKDIR := /home/miaow/software/jlink_v794e/

TGTIMG := intflash
# TGTIMG := sdcard
# TGTIMG := qspiflash

LDFILE := $(ABSPROJDIR)config/stm32h743xihx-$(TGTIMG).ld

CC := $(PREFIX)gcc
LD := $(PREFIX)ld
CXX := $(PREFIX)g++
AS := $(PREFIX)gcc -x assembler-with-cpp
AR := $(PREFIX)ar
OBJCOPY := $(PREFIX)objcopy
SIZE := $(PREFIX)size
PR := $(CC) -E
OBJDUMP := $(PREFIX)objdump
READELF := $(PREFIX)readelf

DEFINES := \
-DUSE_HAL_DRIVER \
-DSTM32H743xx

INCS := \
-I$(ABSPROJDIR)cmsis/include \
-I$(ABSPROJDIR)hal/include

ARCHS := -mcpu=cortex-m7 \
-mthumb \
-mlittle-endian \
-mfloat-abi=hard \
-mfpu=fpv5-d16

DEBUGS := -g -ggdb3
OPTIMS := -O0
FUNCS := \
-fmacro-prefix-map=./= \
-fno-common \
-fno-strict-aliasing \
-fno-dwarf2-cfi-asm \
-fno-allow-store-data-races \
-ftrivial-auto-var-init=zero \
-fno-stack-clash-protection \
-fstrict-flex-arrays=3
WARNS := \
-Wall \
-Wundef \
-Werror=implicit-function-declaration \
-Werror=implicit-int \
-Werror=return-type \
-Werror=strict-prototypes \
-Wframe-larger-than=1024 \
-Wvla \
-Wno-pointer-sign \
-Wcast-function-type \
-Wimplicit-fallthrough=5 \
-Werror=date-time \
-Werror=incompatible-pointer-types \
-Werror=designated-init \
-Wenum-conversion

# SYSROOT := --sysroot=/home/miaow/software/arm-none-eabi-13.2.rel1/arm-none-eabi
SYSROOT :=

LIBS := -lc -lm -lgcc

CFLAGS := \
-c \
-std=gnu17 \
$(ARCHS) \
$(DEFINES) \
$(INCS) \
$(SYSROOT) \
$(DEBUGS) \
$(OPTIMS) \
$(WARNS) \
$(FUNCS)

LDFLAGS := \
-nostartfiles \
$(ARCHS) \
$(LIBS) \
$(SYSROOT) \
$(DEBUGS) \
$(OPTIMS) \
$(WARNS) \
-T$(LDFILE) \
-Wl,--cref

# -Wl,--cref

ASFLAGS := \
-c \
$(ARCHS) \
$(DEFINES) \
-D_ASMLANGUAGE \
$(INCS) \
$(SYSROOT) \
$(DEBUGS) \
$(OPTIMS) \
$(WARNS) \
$(FUNCS)

OBJCOPYFLAGS := -O binary
SIZEFLAGS := --format=berkeley 
PRFLAGS :=
OBJDUMPFLAGS := -d -S -s -x
READELFFLAGS := -a

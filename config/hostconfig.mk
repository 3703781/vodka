include $(config_mk)

HOSTCC := gcc
HOSTLD := ld
HOSTCXX := g++
HOSTAS := as
HOSTAR := ar
HOSTOBJCOPY := objcopy
HOSTSIZE := size
HOSTPR := $(HOSTCC) -E
HOSTOBJDUMP := objdump

HOSTDEFINES :=

HOSTINCS :=

HOSTLIBS := -lc -lm -lgcc

HOSTWARN := \
-Wall \
-Wextra \
-Wunused \
-Wmissing-prototypes \
-Wsign-compare \
-Werror=implicit-function-declaration

HOSTCSTD := -std=gnu17

HOSTOPT := -O2

HOSTLDFLAGS := \
$(HOSTDEFINES) \
$(HOSTINCS) \
$(HOSTLIBS) \
$(HOSTWARN) \
$(HOSTCSTD) \
$(HOSTOPT)

HOSTCFLAGS := \
$(HOSTDEFINES) \
$(HOSTINCS) \
$(HOSTWARN) \
$(HOSTCSTD) \
$(HOSTOPT) \
-c

HOSTASFLAGS := -c \
$(HOSTINCS)

HOSTSIZEFLAGS := --format=berkeley 

HOSTPRFLAGS := -x assembler-with-cpp

HOSTOBJDUMPFLAGS := -d -S -x
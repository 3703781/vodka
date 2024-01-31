this_file := $(lastword $(MAKEFILE_LIST))

target := mkimage
objs := mkimage.o main.o
dirs :=
confs :=
softlink_froms :=

EXTRA_HOSTCFLAGS_DOWN += -Wstrict-prototypes
EXTRA_HOSTCFLAGS_SELF := -I$(this_dir)

include $(hosttarget_mk)

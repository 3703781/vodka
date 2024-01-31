this_file := $(lastword $(MAKEFILE_LIST))

target := genconfig
objs := genconfig.o dict.o
dirs :=
confs :=
softlink_froms :=

EXTRA_HOSTCFLAGS_DOWN += -Wstrict-prototypes
EXTRA_HOSTCFLAGS_SELF :=

include $(hosttarget_mk)

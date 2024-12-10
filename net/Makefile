this_file := $(lastword $(MAKEFILE_LIST))

target := libnet.a

objs := $(patsubst %.c,%.o,$(wildcard *.c))
confs :=
dirs := portable/
softlink_froms :=
EXTRA_CFLAGS_SELF :=
EXTRA_CFLAGS_DOWN +=

include $(target_mk)

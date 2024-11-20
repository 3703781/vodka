this_file := $(lastword $(MAKEFILE_LIST))

objs := $(patsubst %.c,%.o,$(wildcard *.c))
confs :=
dirs :=
softlink_froms :=
EXTRA_CFLAGS_SELF :=
EXTRA_CFLAGS_DOWN +=

include $(obj_mk)

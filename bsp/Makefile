this_file := $(lastword $(MAKEFILE_LIST))

target := libbsp.a

objs := $(patsubst %.c,%.o,$(wildcard *.c))
confs :=
dirs :=
softlink_froms :=
EXTRA_CFLAGS_SELF :=
EXTRA_CFLAGS_DOWN += \
-DCONFIG_ARM \
-I$(ABSPROJDIR)bsp/include \
-I$(ABSPROJDIR)include \

include $(target_mk)

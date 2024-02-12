this_file := $(lastword $(MAKEFILE_LIST))

objs := $(patsubst %.c,%.o,$(wildcard *.c))
confs :=
dirs :=
softlink_froms :=
EXTRA_CFLAGS_SELF :=
EXTRA_CFLAGS_DOWN += \
-DCONFIG_ARM \
-I$(ABSPROJDIR)bsp/include \
-I$(ABSPROJDIR)src/include \
-I$(ABSPROJDIR)include

include $(obj_mk)

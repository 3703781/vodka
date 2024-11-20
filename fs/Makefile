this_file := $(lastword $(MAKEFILE_LIST))

target := libfs.a

objs := $(patsubst %.c,%.o,$(wildcard *.c))
confs :=
dirs := portable/
softlink_froms :=
EXTRA_CFLAGS_SELF :=
EXTRA_CFLAGS_DOWN += \
-I$(ABSPROJDIR)fs/include \
-I$(ABSPROJDIR)freertos/include \
-I$(ABSPROJDIR)freertos/portable


include $(target_mk)

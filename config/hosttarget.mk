include $(helper_mk)

target_compile_related := $(call remove,.reserved,$(target))
abs_target := $(this_dir)$(call remove,.module .reserved,$(target))

include $(hostobj_mk)

.PHONY: cleantarget

# below are objs / static libs / shared libs / module libs FROM SUB DIRS, NOT the target to build
# modules libs are plugins that are not linked into other targets but may be loaded dynamically at runtime using dlopen-like functionality. 
raw_list = $(shell cat $<)
all_list = $(subst .reserved,,$(raw_list))
obj_list = $(filter %.o, $(all_list))
reserved_list = $(filter %.reserved, $(raw_list))

static_list = $(filter %.a, $(all_list)) 
static_notdir = $(notdir $(static_list))
static_dir = $(patsubst %/,%,$(dir $(static_list)))
static_link_param = $(static_list)

# modules libs do not need to be linked to target
# however, rpath will be set to the symbolic link (libxxx.so -> libxxx.so.module) when target linking
module_list_with_ext = $(filter %.so.module, $(all_list))
module_list = $(patsubst %.module, %, $(module_list_with_ext))
module_notdir = $(notdir $(module_list))
module_dir = $(patsubst %/,%,$(dir $(module_list)))
module_dir_comma_sep = $(subst $(SPACE),:,$(module_dir))
module_link_param = $(addprefix -Wl$(COMMA)-rpath$(COMMA),$(module_dir_comma_sep))

shared_list = $(filter %.so, $(all_list))
shared_notdir = $(notdir $(shared_list))
shared_dir = $(patsubst %/,%,$(dir $(shared_list)))
shared_dir_comma_sep = $(subst $(SPACE),:,$(shared_dir))
shared_link_param = $(addprefix -L,$(shared_dir)) \
$(addprefix -Wl$(COMMA)-rpath$(COMMA),$(shared_dir_comma_sep)) $(shared_list)

all_link_param =  $(obj_list) $(static_link_param) $(module_link_param) $(shared_link_param)

EXTRA_HOSTLDFLAGS := $(EXTRA_HOSTCFLAGS_DOWN) $(EXTRA_HOSTCFLAGS_SELF)

reserved_ext := $(if $(findstring .reserved,$(target)),.reserved,)

ifneq ($(filter %.so, $(target_compile_related)),)
# to build shared lib target
$(abs_target): $(tmp_make)
	echo "HOSTLN      $@"; \
	cmd="$(HOSTCC) -shared -fPIC -Wl,-soname,$(notdir $@) -o $@ $(EXTRA_HOSTLDFLAGS) $(all_link_param) $(HOSTLDFLAGS)"; \
	eval $$cmd || ( echo $$cmd; exit 1 ); \
	echo "UPD [SHD]   $<"; \
	echo $@$(reserved_ext) $(reserved_list) > $<; \
	touch -c $@

else ifneq ($(filter %.a, $(target_compile_related)),)
# to build static lib target
$(abs_target): $(tmp_make)
	echo "HOSTAR      $@"; \
	cmd="$(HOSTAR) rcs $@ $(all_list)"; \
	eval $$cmd || ( echo $$cmd; exit 1 ); \
	echo "UPD [ARC]   $<"; \
	echo $@$(reserved_ext) $(reserved_list) > $<; \
	touch -c $@

else ifneq ($(filter %.so.module, $(target_compile_related)),)
# to build module lib target, and create a symbolic link libxxx.so -> libxxx.so.module
$(abs_target): $(tmp_make)
	echo "HOSTLN      $@"; \
	cmd="$(HOSTCC) -shared -fPIC -o $(abs_target) $(EXTRA_HOSTLDFLAGS) $(all_link_param) $(HOSTLDFLAGS)"; \
	eval $$cmd || ( echo $$cmd; exit 1 ); \
	echo "UPD [MOD]   $<"; \
	echo $@$(reserved_ext) $(reserved_list) > $<; \
	touch -c $@

else
# to build executable target
$(abs_target): $(tmp_make)
	echo "HOSTLN      $@"; \
	cmd="$(HOSTCC) -o $@ $(EXTRA_HOSTLDFLAGS) $(all_link_param) $(HOSTLDFLAGS)"; \
	eval $$cmd || ( echo $$cmd; exit 1);

endif

cleantarget:
	echo "CLEAN       $(abs_target)"; \
	rm -f $(abs_target) $(tmp_make)
	
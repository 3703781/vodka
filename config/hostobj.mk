export hostobj_mk hostconfig_mk hosttarget_mk helper_mk
export abs_mkgimage abs_genconfig
export EXTRA_HOSTCFLAGS_DOWN ABSPROJDIR
config_down_variable_names := $(filter CONFIG_DOWN_%, $(.VARIABLES))
config_self_variable_names := $(filter CONFIG_SELF_%, $(.VARIABLES))
export $(config_down_variable_names)

include $(hostconfig_mk)
include $(helper_mk)

EXTRA_HOSTCFLAGS := $(EXTRA_HOSTCFLAGS_SELF) $(EXTRA_HOSTCFLAGS_DOWN)

abs_softlink_froms := $(wildcard $(addprefix $(ABSPROJDIR), $(softlink_froms)))
abs_softlink_c_froms := $(filter %.c, $(abs_softlink_froms))
abs_softlink_c_tos := $(addprefix $(this_dir), $(notdir $(abs_softlink_c_froms)))
abs_softlink_s_froms := $(filter %.s %.S, $(abs_softlink_froms))
abs_softlink_s_tos := $(addprefix $(this_dir), $(notdir $(abs_softlink_s_froms)))
abs_softlink_h_froms := $(filter %.h %.H, $(abs_softlink_froms))
abs_softlink_h_tos := $(addprefix $(this_dir), $(notdir $(abs_softlink_h_froms)))
abs_softlink_obj_froms := $(patsubst %.c, %.o, $(abs_softlink_c_froms)) \
 $(patsubst %.S, %.o, $(patsubst %.s, %.o, $(abs_softlink_s_froms)))
abs_softlink_obj_tos := $(patsubst %.c, %.o, $(abs_softlink_c_tos)) \
 $(patsubst %.S, %.o, $(patsubst %.s, %.o, $(abs_softlink_s_tos)))
abs_softlink_dir_froms := $(filter %/, $(abs_softlink_froms))
abs_softlink_dir_tos := $(addprefix $(this_dir), $(addsuffix /, $(notdir $(patsubst %/,%, $(abs_softlink_dir_froms)))))
abs_softlink_conf_froms := $(filter %.in, $(abs_softlink_froms))
abs_softlink_conf_tos := $(addprefix $(this_dir), $(notdir $(abs_softlink_conf_froms)))
abs_softlink_idl_froms := $(filter %.idl, $(abs_softlink_froms))
abs_softlink_idl_tos := $(addprefix $(this_dir), $(notdir $(abs_softlink_idl_froms)))

abs_softlink_notdir_tos := $(abs_softlink_c_tos) $(abs_softlink_s_tos) $(abs_softlink_h_tos) $(abs_softlink_conf_tos) $(abs_softlink_idl_tos)
abs_softlink_tos := $(abs_softlink_dir_tos) $(abs_softlink_notdir_tos)

abs_conf_froms := $(addprefix $(this_dir), $(confs)) $(abs_softlink_conf_tos)
abs_conf_tos := $(patsubst %.in, %, $(abs_conf_froms))
abs_conf_obj_tos := $(patsubst %.c, %.o, $(filter %.c, $(abs_conf_tos)))

abs_idl_froms := $(addprefix $(this_dir), $(idls)) $(abs_softlink_idl_tos)
abs_idl_tos := $(patsubst %.idl, %.c, $(abs_idl_froms)) $(patsubst %.idl, %.h, $(abs_idl_froms))
abs_idl_obj_tos := $(patsubst %.c, %.o, $(filter %.c, $(abs_idl_tos)))

dir_orders := $(patsubst %.order, %, $(filter %.order, $(dirs)))
dir_no_orders := $(filter-out %.order, $(dirs))
abs_dir_orders := $(addprefix $(this_dir), $(dir_orders))
abs_dir_no_orders := $(addprefix $(this_dir), $(dir_no_orders))
abs_dirs := $(abs_dir_orders) $(abs_dir_no_orders)
abs_dir_order_tmp_makes := $(addsuffix tmp.make, $(abs_dir_orders))
abs_dir_no_order_tmp_makes := $(addsuffix tmp.make, $(abs_dir_no_orders))
abs_dir_tmp_makes := $(abs_dir_order_tmp_makes) $(abs_dir_no_order_tmp_makes)
clean_abs_dirs := $(addprefix clean$(this_dir), $(dir_orders) $(dir_no_orders))

abs_objs := $(addprefix $(this_dir), $(objs)) $(abs_softlink_obj_tos) $(abs_conf_obj_tos) $(abs_idl_obj_tos)
abs_deps := $(patsubst %.o, %.d, $(abs_objs))
tmp_make := $(this_dir)tmp.make

abs_inc_dirs := $(patsubst -I%, %, $(filter -I%, $(EXTRA_CFLAGS) $(CFLAGS)))
recurse_follow_link_this_dir_files := $(shell \
find -L $(this_dir) -type f -regextype posix-extended -regex '.*\.h$$|.*\.c$$|.*\.H$$|.*\.C$$|.*\.s$$|.*\.S$$|.*\.idl$$|.*\.in$$')
recurse_follow_link_abs_inc_dirs_files := $(sort $(shell \
find -L $(abs_inc_dirs) -type f -regextype posix-extended -regex '.*\.h$$|.*\.H$$|.*\.in$$'))
recurse_follow_link_files := $(sort $(recurse_follow_link_this_dir_files) $(recurse_follow_link_abs_inc_dirs_files))

define LINK_RULE
$(this_dir)$(1): $(filter %$(1),$(abs_softlink_froms))
	echo "SHIPPED     $$@"; ln -sf $$(patsubst %/,%,$$<) -t $$(this_dir)
endef

define ABS_DIR_ORDER_TMP_MAKES_RULE
$(1): $(2) $(recurse_follow_link_files)
	$$(MAKE) -C $$(dir $$@) this_dir=$$(dir $$@) $$(MAKECMDGOALS)
endef

.PHONY: _all clean cleantarget softlinks $(abs_softlink_dir_tos) $(clean_abs_dirs)

_all: $(abs_target) $(tmp_make)

ifneq ($(strip $(abs_softlink_froms)),)
__tmp := $(addsuffix /, $(notdir $(patsubst %/, %, $(abs_softlink_dir_froms))))
__tmp += $(notdir $(abs_softlink_notdir_tos))
$(foreach link, $(__tmp), $(eval $(call LINK_RULE,$(link))))
endif

ifneq ($(strip $(abs_dir_order_tmp_makes)),)
__prev_item :=
$(foreach item, $(abs_dir_order_tmp_makes),\
$(eval $(call ABS_DIR_ORDER_TMP_MAKES_RULE,$(item),$(__prev_item))\
$(eval __prev_item := $(item))\
))
endif

clean: $(clean_abs_dirs) cleantarget
	echo "CLEAN       $(dir $(tmp_make))"; \
	objs_deps_to_remove=$$(find $(dir $(tmp_make)) -maxdepth 1 \( -type f -o -type l \) -a \( -name "*.o" -o -name "*.d" \)); \
	rm -f $(tmp_make) $$objs_deps_to_remove $(abs_softlink_notdir_tos) $(abs_conf_tos) $(abs_idl_tos); \
	rm -rf $(patsubst %/, %, $(abs_softlink_dir_tos));

$(clean_abs_dirs):
	$(MAKE) -C $(patsubst clean%,%,$@) this_dir=$(patsubst clean%,%,$@) clean

$(abs_dir_no_order_tmp_makes): $(recurse_follow_link_files)
	$(MAKE) -C $(dir $@) this_dir=$(dir $@) $(MAKECMDGOALS)

$(tmp_make): $(abs_objs) $(abs_conf_tos) $(abs_idl_tos) $(abs_dir_tmp_makes)
	echo "UPD         $@"; \
	echo -n "$(abs_objs) " > $@; \
	for i in $(abs_dir_tmp_makes); do \
	  if [ -f "$${i}" ]; then \
		  cat $${i} >> $@; \
	  fi; \
	done;

%.o: %.c
	echo "HOSTCC      $@"; \
	cmd="$(HOSTCC) -fPIC $(EXTRA_HOSTCFLAGS) $(HOSTCFLAGS) $< -o $@"; \
	eval $$cmd || ( echo $$cmd; exit 1 )

%.o: %.s
	echo "HOSTAS      $@"; \
	cmd="$(HOSTCC) -fPIC $(EXTRA_HOSTCFLAGS) $(HOSTPRFLAGS) $(HOSTASFLAGS) $< -o $@"; \
	eval $$cmd || ( echo $$cmd; exit 1 )

%.d: %.c $(abs_conf_tos) $(abs_idl_tos) $(abs_dir_tmp_makes)
	echo "HOSTDEP     $@"; \
	rm -f $@; \
	cmd='$(HOSTCC) -fPIC $(EXTRA_HOSTCFLAGS) $(HOSTCFLAGS) -MM $<'; res=$$($$cmd); exit_status=$$?; \
	if [ $$exit_status -eq 0 ]; then \
	  if [ -z "$$res" ]; then \
	    echo "$@: " > $@; \
	  else \
		echo "$@ $(this_dir)$$res" > $@; \
	  fi; \
	else \
	  echo $$cmd; exit 1; \
	fi;

%.d: %.s $(abs_conf_tos) $(abs_idl_tos) $(abs_dir_tmp_makes)
	echo "DEP         $@"; \
	touch $@;

genconfig_cmd = $(abs_genconfig) $< $@ $(foreach var,$(config_down_variable_names) \
$(config_self_variable_names),$(call remove, CONFIG_DOWN_ CONFIG_SELF_,$(var))=$(call escape,$($(var))))

$(abs_conf_tos): %: %.in
	echo "WRAP        $@"; \
	$(genconfig_cmd) || ( echo $(genconfig_cmd); exit 1 )

%.c %.h &: %.idl
	echo "IDLC        $@"; \
	cmd="$(abs_idlc) $<"; \
	eval $$cmd || ( echo $$cmd; exit 1 ); \
	if [ ! -e "$(basename $@).c" ]; then \
	  touch $(basename $@).c; \
	fi;

ifeq (,$(findstring clean, $(MAKECMDGOALS)))
include $(abs_deps)
endif

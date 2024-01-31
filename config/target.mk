include $(helper_mk)

target_compile_related := $(call remove,.reserved,$(target))
abs_target := $(this_dir)$(call remove,.module .reserved,$(target))

include $(obj_mk)

.PHONY: cleantarget

# below are static libs  FROM SUB DIRS, NOT the target to build
raw_list = $(shell cat $<)
all_list = $(subst .reserved,,$(raw_list))
obj_list = $(filter %.o, $(all_list))
reserved_list = $(filter %.reserved, $(raw_list))

static_list = $(filter %.a, $(all_list)) 
static_notdir = $(notdir $(static_list))
static_dir = $(patsubst %/,%,$(dir $(static_list)))
static_link_param = $(static_list)

all_link_param = $(obj_list) $(static_link_param)

EXTRA_LDFLAGS := $(EXTRA_CFLAGS_DOWN) $(EXTRA_CFLAGS_SELF)

reserved_ext := $(if $(findstring .reserved,$(target)),.reserved,)

abs_map := $(this_dir)$(target).map
abs_cfgout := $(this_dir)config/$(TGTIMG).cfgout

ifneq ($(filter %.a, $(target_compile_related)),)
# to build static lib target
$(abs_target): $(tmp_make)
	@echo "AR          $@"; \
	cmd="$(AR) rcs $@ $(all_list)"; \
	eval $$cmd || ( echo $$cmd; exit 1 ); \
	echo $@$(reserved_ext) $(reserved_list) > $<; \
	touch -c $@;

cleantarget:
	@echo "CLEAN       $(abs_target)"; \
	rm -f $(abs_target) $(tmp_make);

else
# to build executable target
$(abs_target): $(abs_target).dis $(abs_target).sym # $(abs_target)-$(TGTIMG).bin

$(abs_target)-$(TGTIMG).bin: $(abs_target).bin
	@echo "MKIMG       $@"; \
	$(abs_mkimage) -a `sed -n 's/^\s*\(0x\S*\)\s*__INT_SRAM_START\s*.*$$/\1/p' $(abs_map)` \
	-n $(abs_cfgout) -e `sed -n 's/^\s*\(0x\S*\)\s*__INIT_INTERRUPT_START\s*.*$$/\1/p' $(abs_map)` \
	-d $< $@; 

$(abs_target).sym: $(abs_target).elf
	@echo "READELF     $@"; \
	$(READELF) $(READELFFLAGS) $< > $@

$(abs_target).dis: $(abs_target).elf
	@echo "OBJDUMP     $@"; \
	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@

$(abs_target).bin: $(abs_target).elf
	@echo "OBJCOPY     $@"; \
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

$(abs_target).elf: $(tmp_make) $(LDFILE)
	@echo "LN          $@"; \
	cmd="$(CC) -o $@ $(EXTRA_LDFLAGS) -Wl,--start-group $(all_link_param) -Wl,--end-group $(LDFLAGS) -Wl,-Map,$(abs_map)"; \
	eval $$cmd || ( echo $$cmd; exit 1); \
	set -e; \
	echo "SYSMAP      $(abs_map)"; \
	echo "SIZE        $@"; \
	$(SIZE) $(SIZEFLAGS) $@;

cleantarget:
	@echo "CLEAN       $(abs_target)[.bin .map .sym .elf .dis -$(TGTIMG).bin ]"; \
	rm -f $(addprefix $(abs_target), .bin .map .sym .elf .dis -$(TGTIMG).bin) $(tmp_make)
endif
	
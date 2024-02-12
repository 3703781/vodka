MAKE_VERSION := $(shell make --version | awk '/Make/ {print $$NF}')
MAKE_VERSION_MAJOR := $(shell echo $(MAKE_VERSION) | cut -f1 -d.)
MAKE_VERSION_MINOR := $(shell echo $(MAKE_VERSION) | cut -f2 -d.)
version_ge = $(shell expr $(1) \>= $(2))
ifneq ($(call version_ge,$(MAKE_VERSION_MAJOR),4)$(call version_ge,$(MAKE_VERSION_MINOR),3),11)
    $(error Unsupported Make version. The build system does not work properly with GNU Make $(MAKE_VERSION), please use GNU Make 4.3 or above.)
endif

unexport LC_ALL
LC_COLLATE=C
LC_NUMERIC=C
export LC_COLLATE LC_NUMERIC

unexport GREP_OPTIONS

MAKEFLAGS += --no-builtin-rules --no-print-directory -s
.SUFFIXES:

target := m7proj-h7
dirs := bsp/ cmsis/ hal/ src/

this_file := $(abspath $(lastword $(MAKEFILE_LIST)))
this_dir := $(dir $(this_file))
obj_mk := $(this_dir)config/obj.mk
target_mk := $(this_dir)config/target.mk
config_mk := $(this_dir)config/config.mk
hostobj_mk := $(this_dir)config/hostobj.mk
hosttarget_mk := $(this_dir)config/hosttarget.mk
hostconfig_mk := $(this_dir)config/hostconfig.mk
helper_mk := $(this_dir)config/helper.mk

abs_mkimage := $(this_dir)tools/mkimage/mkimage
abs_genconfig := $(this_dir)tools/genconfig/genconfig

export abs_mkimage abs_genconfig 

.PHONY: bear defconfig cleanbear hosttools cleanhosttools all distclean

all:
	@set -e; \
	$(MAKE) hosttools; \
	$(MAKE) _all

hosttools:
	@set -e; \
	$(MAKE) -C $(dir $(abs_mkimage)) this_dir=$(dir $(abs_mkimage)) _all; \
	$(MAKE) -C $(dir $(abs_genconfig)) this_dir=$(dir $(abs_genconfig)) _all;

cleanhosttools:
	$(MAKE) -C $(dir $(abs_mkimage)) this_dir=$(dir $(abs_mkimage)) clean; \
	$(MAKE) -C $(dir $(abs_genconfig)) this_dir=$(dir $(abs_genconfig)) clean;

include $(target_mk)

bear: clean cleanbear
	bear -- $(MAKE) all; \
	echo "\nIf vscode not updated,\n\trun (ctrl + shift + p) \"Developer: Reload Window\""

cleanbear:
	@echo "CLEAN       $(this_dir)compile_commands.json"; \
	rm -f $(this_dir)compile_commands.json
	
defconfig: 
	@mkdir -p $(this_dir).vscode; \
	files=`ls $(this_dir)config/vsc*`; \
	for file in $$files; do \
	  filename=`basename "$$file"`; \
	  cp -f $(this_dir)config/$$filename $(this_dir).vscode/`echo "$$filename" | sed 's/vsc//'`; \
	done
		
distclean: clean cleanbear cleanhosttools
	@echo "CLEAN       $(this_dir).vscode"; \
	rm -rf $(this_dir).vscode

ifeq ($(DEBUGGER),STLINK)
install:
	@set -e; \
	$(MAKE) -j`nproc`; \
	echo "PROG[STLNK] $(abs_target).elf"; \
	if [ "$(TGTIMG)" = "intflash" ]; then \
	  $(CUBEPROGDIR)STM32_Programmer_CLI -c port=SWD freq=4000 -w $(abs_target).elf -g > /dev/null; \
	elif [ "$(TGTIMG)" = "sdcard" ]; then \
	  echo not impelemented; \
	elif [ "$(TGTIMG)" = "qspiflash" ]; then \
	  echo not impelemented; \
	fi
else ifeq ($(DEBUGGER),JLINK)
install:
	@set -e; \
	$(MAKE) -j`nproc`; \
	echo "PROG[JLNK]  $(abs_target).elf"; \
	if [ "$(TGTIMG)" = "intflash" ]; then \
	  $(JLINKDIR)JFlashExe -hide -openprj$(this_dir)config/stm32h743xi.jflash -open$(abs_target).elf -auto -exit; \
	elif [ "$(TGTIMG)" = "sdcard" ]; then \
	  echo not impelemented; \
	elif [ "$(TGTIMG)" = "qspiflash" ]; then \
	  echo not impelemented; \
	fi

endif

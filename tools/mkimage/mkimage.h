/* SPDX-License-Identifier: GPL-2.0+ */
/* Copyright 2019-2022 NXP */

#ifndef S32CC_IMAGE_PARAMS_H
#define S32CC_IMAGE_PARAMS_H

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#include <asm/types.h>
#include <ctype.h>
#include <image.h>
#include <imagetool.h>
#include <inttypes.h>
#include <stddef.h>
#include <unistd.h>
#include <sizes.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <compiler.h>

#ifndef __packed
#define __packed __attribute__((packed))
#endif

struct flash_write {
	union {
		struct {
			__u32 opcode : 8;
			__u32 reserved : 8;
			__u32 pad : 2;
			__u32 addr_size : 6;
			__u32 cdata_size : 7;

			__u32 valid_addr : 1;
		} config;
		__u32 _config;
	};
	__u32 addr;
	__u32 data;
} __packed;

struct qspi_params {
	__u32 header;
	__u32 mcr;
	__u32 flshcr;
	__u32 bufgencr;
	__u32 dllcr;
	__u32 paritycr;
	__u32 sfacr;
	__u32 smpr;
	__u32 dlcr;
	__u32 sflash_1_size;
	__u32 sflash_2_size;
	__u32 dlpr;
	__u32 sfar;
	__u32 ipcr;
	__u32 tbdr;
	__u8 dll_bypass_en;
	__u8 dll_slv_upd_en;
	__u8 dll_auto_upd_en;
	__u8 ipcr_trigger_en;
	__u8 sflash_clk_freq;
	__u8 reserved[3];
	__u32 command_seq[80];
	struct flash_write writes[10];
};

struct qspi_params *get_macronix_qspi_conf(void);
struct qspi_params *get_micron_qspi_conf(void);

extern struct image_type_params s32cc_image_type_params;
extern struct image_type_params *s32cc_image_type_params_ptr;

int s32cc_check_params(struct image_tool_params *params);
int s32cc_verify_header(unsigned char *ptr, int image_size, struct image_tool_params *params);
void s32cc_print_header(const void *data);
void s32cc_set_header(void *header, struct stat *sbuf, int unused, struct image_tool_params *tool_params);
int s32cc_check_image_type(uint8_t type);
int s32cc_vrec_header(struct image_tool_params *tool_params, struct image_type_params *type_params);

#endif /* IMAGE_QSPI_PARAMS_H */

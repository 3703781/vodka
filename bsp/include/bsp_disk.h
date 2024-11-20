#ifndef __BSP_DISK_H
#define __BSP_DISK_H

#include <stm32h7xx.h>

struct bsp_disk_des;

struct bsp_disk_ops {
	size_t (*write)(struct bsp_disk_des *des, const char *buf, uint32_t blk_addr, uint32_t blk_count);
	size_t (*read)(struct bsp_disk_des *des, const char *buf, uint32_t blk_addr, uint32_t blk_count);
	uint32_t read_timeout;
	uint32_t write_timeout;
};

/**
 * @brief Structure representing the disk descriptor
 *
 * This structure contains various parameters and configurations related to the disk.
 * Members starting with an underscore (_) are meant for internal use only.
 */
struct bsp_disk_des {
	SDMMC_TypeDef *sdmmc;

	uint16_t detect_pin; //!< insert detection pin of SD Card
	GPIO_TypeDef *detect_port; //!< insert detection of SD Card

	uint16_t d0_pin; //!< DATA0 pin of SD Card
	GPIO_TypeDef *d0_port; //!< DATA0 port of SD Card

	uint16_t d1_pin; //!< DATA1 pin of SD Card
	GPIO_TypeDef *d1_port; //!< DATA1 port of SD Card

	uint16_t d2_pin; //!< DATA2 pin of SD Card
	GPIO_TypeDef *d2_port; //!< DATA2 port of SD Card

	uint16_t d3_pin; //!< DATA3 pin of SD Card
	GPIO_TypeDef *d3_port; //!< DATA3 port of SD Card

	uint16_t cmd_pin; //!< CMD pin of SD Card
	GPIO_TypeDef *cmd_port; //!< CMD port of SD Card

	uint16_t clk_pin; //!< CLK pin of SD Card
	GPIO_TypeDef *clk_port; //!< CLK port of SD Card

	struct bsp_disk_ops ops;

	SD_HandleTypeDef _hsd;
};

extern struct bsp_module bsp_disk_mod;

#endif

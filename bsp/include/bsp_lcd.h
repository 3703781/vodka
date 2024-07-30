#ifndef __BSP_LCD_H
#define __BSP_LCD_H

#include <stm32h7xx.h>

struct bsp_lcd_des;

struct bsp_lcd_dev {
	uint32_t hbp; /**< Horizontal back porch */
	uint32_t vbp; /**< Vertical back porch */
	uint32_t hsw; /**< Horizontal sync width */
	uint32_t vsw; /**< Vertical sync width */
	uint32_t hfp; /**< Horizontal front porch */
	uint32_t vfp; /**< Vertical front porch */
	uint32_t width; /**< Width of the LCD display */
	uint32_t height; /**< Height of the LCD display */
};

struct bsp_lcd_backlight_ops {
	void (*turn_on)(struct bsp_lcd_des *des);
	void (*set_brightness)(struct bsp_lcd_des *des, uint8_t brightness);
	uint8_t (*get_brightness)(struct bsp_lcd_des *des);
	void (*turn_off)(struct bsp_lcd_des *des);
};

/**
 * @brief Structure representing the LCD descriptor
 * 
 * This structure contains various parameters and configurations related to the LCD display.
 * The backlight operations (bklt_ops) member is only accessible after calling the init_ltdc function.
 * Members starting with an underscore (_) are meant for internal use only.
 */
struct bsp_lcd_des {
	struct bsp_lcd_dev dev;
	struct bsp_lcd_backlight_ops bklt_ops;
	uint16_t bklt_pin;
	GPIO_TypeDef *bklt_port;
	TIM_TypeDef *bklt_timer;
	uint16_t bklt_timer_ch;
	uint32_t layer_num;
	void *layer_gram_addr[2];
	LTDC_HandleTypeDef _hltdc;
	DMA2D_HandleTypeDef _hdma2d;
	LTDC_LayerCfgTypeDef _lyr_cfg[2];
};

extern struct bsp_module bsp_lcd_mod;

void bsp_lcd_cpybuf(struct bsp_lcd_des *lcd_des, uint16_t x, uint16_t y, uint16_t width, uint16_t height,
		    uint32_t *color);

#endif

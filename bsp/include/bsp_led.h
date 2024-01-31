#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "stm32h7xx_hal.h"

enum bsp_led_inst {
    BSP_LED_BLUE = 0,
    BSP_LED_ERROR
};

#define BSP_LED_BLUE_PIN GPIO_PIN_13
#define BSP_LED_BLUE_PORT GPIOC
#define BSP_LED_ERROR_PIN GPIO_PIN_7
#define BSP_LED_ERROR_PORT GPIOH

void bsp_led_init(void);
void bsp_led_toggle(enum bsp_led_inst led);
void bsp_led_on(enum bsp_led_inst led);
void bsp_led_off(enum bsp_led_inst led);

#endif /* __LED_H__ */


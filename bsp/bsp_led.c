#include "bsp_led.h"
#include <zephyr/sys/util.h>

struct led_info {
	GPIO_TypeDef *port;
	uint16_t pin;
};

#define LED_LIST_ITEM(which) [which] = { .port = which##_PORT, .pin = which##_PIN }

static struct led_info led_info_list[] = { LED_LIST_ITEM(BSP_LED_BLUE), LED_LIST_ITEM(BSP_LED_RED),
					   LED_LIST_ITEM(BSP_LED_YELLOW) };

void bsp_led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_TypeDef *port;
	uint16_t pin;

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	ARRAY_FOR_EACH_PTR(led_info_list, ptr) {
		port = ptr->port;
		pin = ptr->pin;
		HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
		GPIO_InitStruct.Pin = pin;
		HAL_GPIO_Init(port, &GPIO_InitStruct);
	}
}

void bsp_led_toggle(enum bsp_led_inst led)
{
	HAL_GPIO_TogglePin(led_info_list[(size_t)led].port, led_info_list[(size_t)led].pin);
}

void bsp_led_on(enum bsp_led_inst led)
{
	HAL_GPIO_WritePin(led_info_list[(size_t)led].port, led_info_list[(size_t)led].pin, GPIO_PIN_RESET);
}

void bsp_led_off(enum bsp_led_inst led)
{
	HAL_GPIO_WritePin(led_info_list[(size_t)led].port, led_info_list[(size_t)led].pin, GPIO_PIN_SET);
}

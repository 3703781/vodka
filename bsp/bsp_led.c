#include "bsp_led.h"
#include <zephyr/sys/util.h>

struct led_info {
	GPIO_TypeDef *port;
	uint16_t pin;
};

#define LED_LIST_ITEM(which) \
	[which] = { .port = which##_PORT, .pin = which##_PIN }

static struct led_info led_info_list[] = { LED_LIST_ITEM(BSP_LED_BLUE),
					 LED_LIST_ITEM(BSP_LED_ERROR) };

void bsp_led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	for (size_t i = 0; i < ARRAY_SIZE(led_info_list); i++) {
		HAL_GPIO_WritePin(led_info_list[i].port, led_info_list[i].pin,
				  GPIO_PIN_SET);
	}

	GPIO_InitStruct.Pin = BSP_LED_BLUE_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BSP_LED_BLUE_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = BSP_LED_ERROR_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BSP_LED_ERROR_PORT, &GPIO_InitStruct);
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

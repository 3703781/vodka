#ifndef __BSP_TTY_H
#define __BSP_TTY_H

#include <stdint.h>
#include <stm32h7xx.h>

struct bsp_tty_des;

struct bsp_tty_ops {
	size_t (*write)(struct bsp_tty_des *des, const char *buf, size_t count);
	size_t (*read)(struct bsp_tty_des *des, char *buf, size_t count);
};

/**
 * @brief Structure representing the TTY descriptor
 * 
 * This structure contains various parameters and configurations related to the TTY display.
 */
struct bsp_tty_des {
	USART_TypeDef *uart; // compatible with USART and UART
	uint32_t baud; // baudrate
	GPIO_TypeDef *uart_port_tx;
	uint16_t uart_pin_tx;
	GPIO_TypeDef *uart_port_rx;
	uint16_t uart_pin_rx;
	DMA_Stream_TypeDef *dma_tx;
	DMA_Stream_TypeDef *dma_rx;
	struct bsp_tty_ops ops;
	UART_HandleTypeDef _huart;
	DMA_HandleTypeDef _hdma_uart_tx;
	DMA_HandleTypeDef _hdma_uart_rx;
	uint8_t *_tx_rd_ptr;
	uint8_t *_tx_wr_ptr;
};

extern struct bsp_module bsp_tty_mod;

#endif
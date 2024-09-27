#include <bsp_utils.h>
#include <bsp_module.h>
#include <bsp_tty.h>

static void *set_default_des(void *des);
static int bsp_tty_init(void *des);
static size_t write(struct bsp_tty_des *des, const char *buf, size_t count);
static size_t read(struct bsp_tty_des *des, char *buf, size_t count);
static void start_tx(DMA_HandleTypeDef *hdma);

extern uint8_t _stty_int_sram_d1, _etty_int_sram_d1, TTY_SIZE;

struct bsp_module bsp_tty_mod = { .name = "TTY1",
				  .state = BSP_MODULE_STATE_COMING,
				  .prepare = set_default_des,
				  .setup = bsp_tty_init,
				  .descriptor = NULL,
				  .version = "0.1" };

static USART_TypeDef *uart_tb[8] = { USART1, USART2, USART3, UART4, UART5, USART6, UART7, UART8 };

static DMA_Stream_TypeDef *dma_tb[2][8] = { { DMA1_Stream0, DMA1_Stream1, DMA1_Stream2, DMA1_Stream3, DMA1_Stream4,
					      DMA1_Stream5, DMA1_Stream6, DMA1_Stream7 },
					    { DMA2_Stream0, DMA2_Stream1, DMA2_Stream2, DMA2_Stream3, DMA2_Stream4,
					      DMA2_Stream5, DMA2_Stream6, DMA2_Stream7 } };

static uint32_t dma_req_tb[8][2] = {
	{ DMA_REQUEST_USART1_TX, DMA_REQUEST_USART1_RX }, { DMA_REQUEST_USART2_TX, DMA_REQUEST_USART2_RX },
	{ DMA_REQUEST_USART3_TX, DMA_REQUEST_USART3_RX }, { DMA_REQUEST_UART4_TX, DMA_REQUEST_UART4_RX },
	{ DMA_REQUEST_UART5_TX, DMA_REQUEST_UART5_RX },	  { DMA_REQUEST_USART6_TX, DMA_REQUEST_USART6_RX },
	{ DMA_REQUEST_UART7_TX, DMA_REQUEST_UART7_RX },	  { DMA_REQUEST_UART8_TX, DMA_REQUEST_UART8_RX },
};

static IRQn_Type dma_irq_tb[2][8] = { { DMA1_Stream0_IRQn, DMA1_Stream1_IRQn, DMA1_Stream2_IRQn, DMA1_Stream3_IRQn,
					DMA1_Stream4_IRQn, DMA1_Stream5_IRQn, DMA1_Stream6_IRQn, DMA1_Stream7_IRQn },
				      { DMA2_Stream0_IRQn, DMA2_Stream1_IRQn, DMA2_Stream2_IRQn, DMA2_Stream3_IRQn,
					DMA2_Stream4_IRQn, DMA2_Stream5_IRQn, DMA2_Stream6_IRQn, DMA2_Stream7_IRQn } };

/**
 * @brief Retrieve the index of the specified UART in the UART table.
 * 
 * This function searches for the UART instance in the uart_tb array and returns 
 * its index. If the UART instance is not found in the array, it returns -ENODEV.
 * 
 * @param des Pointer to the TTY descriptor.
 * @return The index of the UART instance in the uart_tb array, or -ENODEV if not found.
 */
static int get_uart_idx(struct bsp_tty_des *des)
{
	size_t i = 0;
	while (i < ARRAY_SIZE(uart_tb) && uart_tb[i] != des->uart) i++;
	if (i == ARRAY_SIZE(uart_tb))
		return -ENODEV;
	return i;
}

/**
 * @brief Get the alternate function for the specified UART and pin configuration.
 * 
 * This function returns the appropriate GPIO alternate function (AF) value 
 * based on the UART instance and the associated TX/RX pin configuration. 
 * Different UARTs and pin mappings may have different AF values.
 * 
 * @param des Pointer to TTY descriptor containing initialization parameters.
 * @param  tx_rx Flag indicating whether the function is for TX (non-zero) or RX (zero).
 * @return The GPIO alternate function value for the specified configuration.
 */
static uint8_t get_uart_af(struct bsp_tty_des *des, uint8_t tx_rx)
{
	if (des->uart == USART1) {
		if (tx_rx && des->uart_port_tx == GPIOB && des->uart_pin_tx == GPIO_PIN_14)
			return GPIO_AF4_USART1;
		else if (!tx_rx && des->uart_port_rx == GPIOB && des->uart_pin_rx == GPIO_PIN_15)
			return GPIO_AF4_USART1;
		return GPIO_AF7_USART1;
	} else if (des->uart == USART2) {
		return GPIO_AF7_USART2;
	} else if (des->uart == USART3) {
		return GPIO_AF7_USART3;
	} else if (des->uart == UART4) {
		if (tx_rx && des->uart_port_tx == GPIOA && des->uart_pin_tx == GPIO_PIN_12)
			return GPIO_AF6_UART4;
		else if (!tx_rx && des->uart_port_rx == GPIOA && des->uart_pin_rx == GPIO_PIN_11)
			return GPIO_AF6_UART4;
		return GPIO_AF8_UART4;
	} else if (des->uart == UART5) {
		if (tx_rx && des->uart_port_tx == GPIOC && des->uart_pin_tx == GPIO_PIN_12)
			return GPIO_AF8_UART5;
		else if (!tx_rx && des->uart_port_rx == GPIOD && des->uart_pin_rx == GPIO_PIN_2)
			return GPIO_AF8_UART5;
		return GPIO_AF14_UART5;
	} else if (des->uart == USART6) {
		return GPIO_AF7_USART6;
	} else if (des->uart == UART7) {
		if (tx_rx && des->uart_port_tx == GPIOE && des->uart_pin_tx == GPIO_PIN_8)
			return GPIO_AF7_UART7;
		else if (!tx_rx && des->uart_port_rx == GPIOE && des->uart_pin_rx == GPIO_PIN_7)
			return GPIO_AF7_UART7;
		else if (tx_rx && des->uart_port_tx == GPIOF && des->uart_pin_tx == GPIO_PIN_7)
			return GPIO_AF7_UART7;
		else if (!tx_rx && des->uart_port_rx == GPIOF && des->uart_pin_rx == GPIO_PIN_6)
			return GPIO_AF7_UART7;
		return GPIO_AF11_UART7;
	} else {
		return GPIO_AF8_UART8;
	}
}

/**
 * @brief Get the DMA request for the specified UART and direction.
 * 
 * This function retrieves the DMA request setting for the specified UART instance 
 * and direction (TX or RX).
 * 
 * @param des Pointer to the TTY descriptor.
 * @param tx_rx: Flag indicating whether the request is for TX (1) or RX (0).
 * @param request Pointer to store the resulting DMA request.
 * @return 0 on success, or -ENODEV if the UART is not found.
 */
static int get_dma_request(struct bsp_tty_des *des, uint8_t tx_rx, uint32_t *request)
{
	int uart_idx = get_uart_idx(des);
	if (uart_idx)
		return -ENODEV;
	*request = dma_req_tb[uart_idx][!tx_rx];
	return 0;
}

/**
 * @brief Retrieve the DMA IRQ number for the specified UART and direction.
 * 
 * The DMA IRQ number is cached each time this func is called to accelerate
 * the subsequent callings. To avoid data corruption, the tx and rx are cached seprately.
 * 
 * @param des Pointer to the TTY descriptor containing the DMA stream information.
 * @param tx_rx Flag indicating whether the request is for TX (non-zero) or RX (zero).
 * @param dma_irq Pointer to store the resulting IRQ number.
 * @param clear_cache If set to 1, the cache for the param tx_rx is cleared before retrieving 
 *                    the irq number. Note that the cache is always updated regardless of the
 *                    setting of this param.
 * @return 0 on success, or -ENODEV if the DMA stream is not found.
 */
static int get_dma_irq(struct bsp_tty_des *des, uint8_t tx_rx, IRQn_Type *dma_irq, uint8_t clear_cache)
{
	size_t i, j;
	DMA_Stream_TypeDef *dma = tx_rx ? des->dma_tx : des->dma_rx;
	static IRQn_Type cache_dma_irq[2] = { NonMaskableInt_IRQn, NonMaskableInt_IRQn };
	if (!clear_cache && cache_dma_irq[tx_rx] != NonMaskableInt_IRQn) {
		*dma_irq = cache_dma_irq[tx_rx];
		return 0;
	}

	*dma_irq = NonMaskableInt_IRQn;
	for (i = 0; i < ARRAY_SIZE(dma_tb); i++) {
		for (j = 0; j < ARRAY_SIZE(dma_tb[0]); j++) {
			if (dma == dma_tb[i][j])
				goto found;
		}
	}
	if (i == ARRAY_SIZE(dma_tb) || j == ARRAY_SIZE(dma_tb[0])) {
		cache_dma_irq[tx_rx] = NonMaskableInt_IRQn;
		return -ENODEV;
	}
found:
	cache_dma_irq[tx_rx] = *dma_irq = dma_irq_tb[i][j];
	return 0;
}

/**
 * @brief Initialize uninitialized members in the TTY descriptor with default values.
 * 
 * @param des TTY descriptor, set to NULL to alloc the memory of the descriptor automatically.
 * @return struct bsp_tty_des * Pointer to descriptor on success, otherwise negative error code.
 */
static void *set_default_des(void *des)
{
	struct bsp_tty_des *int_des = (struct bsp_tty_des *)des;

	if (int_des == NULL) {
		int_des = (struct bsp_tty_des *)calloc(sizeof(struct bsp_tty_des), 1);
		if (IS_ERR_OR_NULL(int_des))
			return ERR_PTR(-ENOMEM);
	}

	/* set to default value */
	SET_VAL_IF_NULL(int_des->uart, USART1);
	SET_VAL_IF_ZERO(int_des->baud, 115200);
	SET_VAL_IF_ZERO(int_des->uart_pin_tx, GPIO_PIN_6);
	SET_VAL_IF_NULL(int_des->uart_port_tx, GPIOB);
	SET_VAL_IF_ZERO(int_des->uart_pin_rx, GPIO_PIN_7);
	SET_VAL_IF_NULL(int_des->uart_port_rx, GPIOB);
	SET_VAL_IF_NULL(int_des->dma_tx, DMA1_Stream0);
	SET_VAL_IF_NULL(int_des->dma_rx, DMA1_Stream1);
	SET_VAL_IF_NULL(int_des->ops.write, write);
	SET_VAL_IF_NULL(int_des->ops.read, read);
	SET_VAL_IF_NULL(int_des->_tx_rd_ptr, (uint8_t *)&_stty_int_sram_d1);
	SET_VAL_IF_NULL(int_des->_tx_wr_ptr, (uint8_t *)&_stty_int_sram_d1);
	SET_VAL_IF_NOT_ZERO(int_des->_tx_rd_total_bytes, 0);
	SET_VAL_IF_NOT_ZERO(int_des->_tx_wr_total_bytes, 0);

	return int_des;
}

/**
 * @brief Initialize the UART/USART hardware based on the descriptor.
 *        This function configures the peripheral with the parameters defined 
 *        in the `bsp_tty_des` structure, and initializes the hardware.
 * 
 * @param des Pointer to TTY descriptor containing initialization parameters.
 * 
 * @return 0 on success, or -EAGAIN or -ENODEV if initialization fails.
 */
static int init_uart(struct bsp_tty_des *des)
{
	struct bsp_utils_periph_des periph_des = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };
	if (des->uart == USART1 || des->uart == USART6) {
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART16;
		PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
			return -EAGAIN;

	} else {
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART234578;
		PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
			return -EAGAIN;
	}
	periph_des.addr_lo = (uint32_t)des->uart;
	if (bsp_utils_periph_clk(&periph_des, ENABLE))
		return -ENODEV;

	des->_huart.Instance = des->uart;
	des->_huart.Init.BaudRate = des->baud;
	des->_huart.Init.WordLength = UART_WORDLENGTH_8B;
	des->_huart.Init.StopBits = UART_STOPBITS_1;
	des->_huart.Init.Parity = UART_PARITY_NONE;
	des->_huart.Init.Mode = UART_MODE_TX_RX;
	des->_huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	des->_huart.Init.OverSampling = UART_OVERSAMPLING_16;
	des->_huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	des->_huart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	des->_huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&des->_huart) != HAL_OK)
		return -EAGAIN;
	if (HAL_UARTEx_DisableFifoMode(&des->_huart) != HAL_OK)
		return -EAGAIN;

	ATOMIC_SET_BIT(des->_huart.Instance->CR3, USART_CR3_DMAT);
	return 0;
}

/**
 * @brief Initialize GPIO pins for UART communication.
 * 
 * @param des Pointer to TTY descriptor containing initialization parameters.
 * @return 0 on success, or -ENODEV if enabling the peripheral clock fails.
 */
static int init_gpio(struct bsp_tty_des *des)
{
	struct bsp_utils_periph_des periph_des = { 0 };
	GPIO_InitTypeDef gpio_init = { 0 };

	periph_des.addr_lo = (uint32_t)des->uart_port_tx;
	if (bsp_utils_periph_clk(&periph_des, ENABLE))
		return -ENODEV;

	periph_des.addr_lo = (uint32_t)des->uart_port_rx;
	if (bsp_utils_periph_clk(&periph_des, ENABLE))
		return -ENODEV;

	gpio_init.Pin = des->uart_pin_tx;
	gpio_init.Mode = GPIO_MODE_AF_PP;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
	gpio_init.Alternate = get_uart_af(des, 1);
	HAL_GPIO_Init(des->uart_port_tx, &gpio_init);

	gpio_init.Pin = des->uart_pin_rx;
	gpio_init.Alternate = get_uart_af(des, 0);
	HAL_GPIO_Init(des->uart_port_rx, &gpio_init);
	return 0;
}

/**
 * @brief Initialize DMA for UART.
 * @param des Pointer to the TTY descriptor.
 * @return 0 on success, -ENODEV or -EAGAIN on failure.
 */
static int init_dma(struct bsp_tty_des *des)
{
	struct bsp_utils_periph_des periph_des = { .addr_lo = (uint32_t)des->dma_tx };
	IRQn_Type dma_irq;
	uint32_t request;
	int res;

	res = bsp_utils_periph_clk(&periph_des, ENABLE);
	if (res)
		return -ENODEV;
	periph_des.addr_lo = (uint32_t)des->dma_rx;
	res = bsp_utils_periph_clk(&periph_des, ENABLE);
	if (res)
		return res;

	// DMA init
	des->_hdma_uart_tx.Instance = des->dma_tx;
	res = get_dma_request(des, 1, &request);
	if (res)
		return res;
	des->_hdma_uart_tx.Init.Request = request;
	des->_hdma_uart_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
	des->_hdma_uart_tx.Init.PeriphInc = DMA_PINC_DISABLE;
	des->_hdma_uart_tx.Init.MemInc = DMA_MINC_ENABLE;
	des->_hdma_uart_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	des->_hdma_uart_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	des->_hdma_uart_tx.Init.Mode = DMA_NORMAL;
	des->_hdma_uart_tx.Init.Priority = DMA_PRIORITY_LOW;
	des->_hdma_uart_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	des->_hdma_uart_tx.Init.MemBurst = DMA_MBURST_SINGLE;
	des->_hdma_uart_tx.Init.PeriphBurst = DMA_MBURST_SINGLE;
	des->_hdma_uart_tx.XferCpltCallback = start_tx;
	if (HAL_DMA_Init(&des->_hdma_uart_tx) != HAL_OK)
		return -EAGAIN;

	__HAL_LINKDMA(&des->_huart, hdmatx, des->_hdma_uart_tx);

	des->_hdma_uart_rx.Instance = des->dma_rx;
	res = get_dma_request(des, 0, &request);
	if (res)
		return res;
	des->_hdma_uart_rx.Init.Request = request;
	des->_hdma_uart_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	des->_hdma_uart_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	des->_hdma_uart_rx.Init.MemInc = DMA_MINC_ENABLE;
	des->_hdma_uart_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	des->_hdma_uart_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	des->_hdma_uart_rx.Init.Mode = DMA_NORMAL;
	des->_hdma_uart_rx.Init.Priority = DMA_PRIORITY_LOW;
	des->_hdma_uart_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	if (HAL_DMA_Init(&des->_hdma_uart_rx) != HAL_OK)
		return -EAGAIN;

	__HAL_LINKDMA(&des->_huart, hdmarx, des->_hdma_uart_rx);

	// DMA interrupt
	res = get_dma_irq(des, 1, &dma_irq, 1);
	if (res)
		return res;
	HAL_NVIC_SetPriority(dma_irq, 0, 0);
	HAL_NVIC_EnableIRQ(dma_irq);

	res = get_dma_irq(des, 0, &dma_irq, 1);
	if (res)
		return res;
	HAL_NVIC_SetPriority(dma_irq, 0, 0);
	HAL_NVIC_EnableIRQ(dma_irq);

	MODIFY_REG(((DMA_Stream_TypeDef *)des->_hdma_uart_tx.Instance)->CR,
		   (DMA_IT_TC | DMA_IT_TE | DMA_IT_DME | DMA_IT_HT), (DMA_IT_TC));

	// Configure DMA Stream destination address
	((DMA_Stream_TypeDef *)des->_hdma_uart_tx.Instance)->PAR = (uint32_t)&des->_huart.Instance->TDR;

	return 0;
}

/**
 * @brief  Initialize the TTY interface.
 * 
 * @param des Pointer to the TTY descriptor.
 * @return 0 on success, or an error code on failure.
 * @note Ensure the descriptor is properly configured before calling.
 */
static int bsp_tty_init(void *des)
{
	int res;
	struct bsp_tty_des *int_des = (struct bsp_tty_des *)des;

	res = init_gpio(int_des);
	if (res)
		return res;

	res = init_uart(int_des);
	if (res)
		return res;

	res = init_dma(int_des);
	if (res)
		return res;

	return 0;
}

static uint8_t *get_real_tx_rd_ptr(struct bsp_tty_des *des)
{
	DMA_Stream_TypeDef *dma = (DMA_Stream_TypeDef *)des->dma_tx;
	uint8_t *real_ptr = NULL;

	if (READ_BIT(dma->CR, DMA_SxCR_EN))
		return ERR_PTR(-EBUSY);

	real_ptr = des->_tx_rd_ptr - dma->NDTR;
	des->_tx_rd_total_bytes -= dma->NDTR;
	if (real_ptr < &_stty_int_sram_d1)
		real_ptr += (uint32_t)&TTY_SIZE;
	return real_ptr;
}

#define BSP_TTY_INC_PTR(ptr, num)                 \
	do {                                      \
		ptr += num;                       \
		if (ptr >= &_etty_int_sram_d1)    \
			ptr = &_stty_int_sram_d1; \
	} while (0)

static size_t write(struct bsp_tty_des *des, const char *buf, size_t count)
{
	IRQn_Type dma_irq;
	DMA_Stream_TypeDef *dma = (DMA_Stream_TypeDef *)des->dma_tx;
	uint8_t *tx_rd_ptr;
	ssize_t new_count = count;

	if (get_dma_irq(des, 1, &dma_irq, 0))
		return 0;

	// disable and wait for the current isr to finish
	NVIC_DisableIRQ(dma_irq);
	while (NVIC_GetActive(dma_irq));

	// disable DMA and wait for the current trans
	// We are to sync and set all necessary fields.
	// So, no further dma isr is needed and pending status should be cleared immediately.
	CLEAR_BIT(dma->CR, DMA_SxCR_EN);
	while (READ_BIT(dma->CR, DMA_SxCR_EN));
	NVIC_ClearPendingIRQ(dma_irq);

	// We assumed that all the data will be sent. However, there is chance that not all
	// the bytes are sent. So get the read point to the next byte of the position
	// that dma had just read from.
	tx_rd_ptr = get_real_tx_rd_ptr(des);
	if (IS_ERR(tx_rd_ptr))
		return 0;	

	// if ring buffer overflow, wait
	if (des->_tx_wr_total_bytes + count - des->_tx_rd_total_bytes > (uint32_t)&TTY_SIZE) {
		new_count = tx_rd_ptr - des->_tx_wr_ptr;
		if (new_count < 0)
			new_count += (ssize_t)&TTY_SIZE;
	}
	des->_tx_wr_total_bytes += new_count;
	count -= new_count;
	while (new_count--) {
		*des->_tx_wr_ptr = *buf++;
		BSP_TTY_INC_PTR(des->_tx_wr_ptr, 1);
	}
	NVIC_EnableIRQ(dma_irq);
	start_tx(&des->_hdma_uart_tx);

	// Enable the DMA transfer for transmit request by setting the DMAT bit in the UART CR3 register
	return new_count;
}

typedef struct {
	__IO uint32_t ISR; /*!< DMA interrupt status register */
	__IO uint32_t Reserved0;
	__IO uint32_t IFCR; /*!< DMA interrupt flag clear register */
} DMA_Base_Registers;

static void start_tx(DMA_HandleTypeDef *hdma)
{
	uint32_t count;
	struct bsp_tty_des *des =
		CONTAINER_OF(CONTAINER_OF(&hdma->Init, DMA_HandleTypeDef, Init), struct bsp_tty_des, _hdma_uart_tx);
	DMA_Stream_TypeDef *dma = (DMA_Stream_TypeDef *)des->dma_tx;
	uint8_t *real_ptr;

	// disable DMA
	CLEAR_BIT(dma->CR, DMA_SxCR_EN);
	while (READ_BIT(dma->CR, DMA_SxCR_EN)); // wait for the current trans

	// Clear all interrupt flags at correct offset within the register
	((DMA_Base_Registers *)hdma->StreamBaseAddress)->IFCR = 0x3FUL << (hdma->StreamIndex & 0x1FU);

	// We assumed that all the data will be sent. However, there is chance that not all
	// the bytes are sent. So revert the read point to the next byte of the position
	// that dma had just read from.
	real_ptr = get_real_tx_rd_ptr(des);

	// we do nothing if read position equals to write position
	if (des->_tx_wr_total_bytes == des->_tx_rd_total_bytes)
		return;

	if (IS_ERR(real_ptr))
		return;
	des->_tx_rd_ptr = real_ptr;
	count = des->_tx_rd_ptr < des->_tx_wr_ptr ? des->_tx_wr_ptr - des->_tx_rd_ptr :
						    &_etty_int_sram_d1 - des->_tx_rd_ptr;

	// set dma parameters
	dma->NDTR = count;
	dma->M0AR = (uint32_t)des->_tx_rd_ptr; // only source address here. destination address is set when init

	// assuming that all the data will be sent
	BSP_TTY_INC_PTR(des->_tx_rd_ptr, count);
	des->_tx_rd_total_bytes += count;

	// SCB_CleanDCache();
	SCB_CleanDCache_by_Addr((uint32_t *)dma->M0AR, count);
	SET_BIT(dma->CR, DMA_SxCR_EN); // start to send
}

static size_t read(struct bsp_tty_des *des, char *buf, size_t count)
{
	return 0;
}
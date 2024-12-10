#include <bsp.h>
#include <stm32h7xx.h>

static int bsp_disk_init(void *des);
static void *set_default_des(void *des);

struct bsp_module bsp_disk_mod = { .name = "DISK",
				   .state = BSP_MODULE_STATE_COMING,
				   .prepare = set_default_des,
				   .setup = bsp_disk_init,
				   .descriptor = NULL,
				   .version = "0.1" };

BSP_MODULE_DECLARE(bsp_disk_mod);

static size_t write(struct bsp_disk_des *des, const char *buf, uint32_t blk_addr, uint32_t blk_count)
{
	HAL_StatusTypeDef res;
	if (BSP_MODULE_THIS->state != BSP_MODULE_STATE_LIVE)
		return 0;
	// res = HAL_SD_GetCardState(&des->_hsd);
	// if (res != HAL_SD_CARD_STANDBY)
	// 	return 0;
	res = HAL_SD_Erase(&des->_hsd, blk_addr, blk_addr + 512);
	if (res != HAL_OK)
		return 0;
	res = HAL_SD_WriteBlocks(&des->_hsd, (uint8_t *)buf, blk_addr, blk_count, des->ops.read_timeout);
	if (res != HAL_OK)
		return 0;
	return blk_count;
}

static size_t read(struct bsp_disk_des *des, const char *buf, uint32_t blk_addr, uint32_t blk_count)
{
	HAL_StatusTypeDef res;
	if (BSP_MODULE_THIS->state != BSP_MODULE_STATE_LIVE)
		return 0;
	// res = HAL_SD_GetCardState(&des->_hsd);
	// if (res != HAL_SD_CARD_STANDBY)
	// 	return 0;
	res = HAL_SD_ReadBlocks(&des->_hsd, (uint8_t *)buf, blk_addr, blk_count, des->ops.write_timeout);
	if (res != HAL_OK)
		return 0;
	return blk_count;
}

/**
 * @brief Initialize uninitialized members in the DISK descriptor with default values.
 *
 * @param des DISK descriptor, set to NULL to alloc the memory of the descriptor automatically.
 * @return struct bsp_disk_des * Pointer to descriptor on success, otherwise negative error code.
 */
static void *set_default_des(void *des)
{
	struct bsp_disk_des *int_des = (struct bsp_disk_des *)des;
	const uint32_t sys_clk_freq = HAL_RCC_GetSysClockFreq();

	if (int_des == NULL) {
		int_des = (struct bsp_disk_des *)calloc(sizeof(struct bsp_disk_des), 1);
		if (IS_ERR_OR_NULL(int_des))
			return ERR_PTR(-ENOMEM);
	}

	/* set to default value */
	// +--------+-------------+
	// | signal |  sdmmc1     |
	// +--------+-------------+
	// | detect |  PG14       |
	// | D0     |  PC8  AF12  |
	// | D1     |  PC9  AF12  |
	// | D2     |  PC10 AF12  |
	// | D3     |  PC11 AF12  |
	// | CLK    |  PC12 AF12  |
	// | CMD    |  PD2  AF12  |
	// +--------+-------------+
	SET_VAL_IF_ZERO(int_des->detect_pin, GPIO_PIN_14);
	SET_VAL_IF_NULL(int_des->detect_port, GPIOG);
	SET_VAL_IF_ZERO(int_des->d0_pin, GPIO_PIN_8);
	SET_VAL_IF_NULL(int_des->d0_port, GPIOC);
	SET_VAL_IF_ZERO(int_des->d1_pin, GPIO_PIN_9);
	SET_VAL_IF_NULL(int_des->d1_port, GPIOC);
	SET_VAL_IF_ZERO(int_des->d2_pin, GPIO_PIN_10);
	SET_VAL_IF_NULL(int_des->d2_port, GPIOC);
	SET_VAL_IF_ZERO(int_des->d3_pin, GPIO_PIN_11);
	SET_VAL_IF_NULL(int_des->d3_port, GPIOC);
	SET_VAL_IF_ZERO(int_des->clk_pin, GPIO_PIN_12);
	SET_VAL_IF_NULL(int_des->clk_port, GPIOC);
	SET_VAL_IF_ZERO(int_des->cmd_pin, GPIO_PIN_2);
	SET_VAL_IF_NULL(int_des->cmd_port, GPIOD);
	SET_VAL_IF_NULL(int_des->sdmmc, SDMMC1);
	SET_VAL_IF_NULL(int_des->ops.write, write);
	SET_VAL_IF_NULL(int_des->ops.read, read);
	SET_VAL_IF_ZERO(int_des->ops.read_timeout, 100 * sys_clk_freq / 1000); // 100ms
	SET_VAL_IF_ZERO(int_des->ops.write_timeout, 100 * sys_clk_freq / 1000); // 100ms

	return int_des;
}

/**
 * @brief Initialize the MMC hardware based on the descriptor.
 *        This function configures the peripheral with the parameters defined
 *        in the `bsp_disk_des` structure, and initializes the hardware.
 *
 * @param des Pointer to DISK descriptor containing initialization parameters.
 *
 * @return 0 on success, or -EAGAIN or -ENODEV if initialization fails.
 */
static int init_mmc(struct bsp_disk_des *des)
{
	struct bsp_utils_periph_des periph_des = { 0 };

	// sdmmc clocksource is initialized in bsp_earlyinit_clk()

	HAL_NVIC_SetPriority(SDMMC1_IRQn, 14, 0);
	HAL_NVIC_EnableIRQ(SDMMC1_IRQn);

	periph_des.addr_lo = (uint32_t)des->sdmmc;
	if (bsp_utils_periph_clk(&periph_des, ENABLE))
		return -ENODEV;

	des->_hsd.Instance = (SD_TypeDef *)des->sdmmc;
	des->_hsd.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
	des->_hsd.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
	des->_hsd.Init.BusWide = SDMMC_BUS_WIDE_4B;
	des->_hsd.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
	des->_hsd.Init.ClockDiv = 2;
	if (HAL_SD_Init(&des->_hsd) != HAL_OK)
		return -EAGAIN;
	return 0;
}

/**
 * @brief Initialize GPIO pins for MMC communication.
 *
 * @param des Pointer to DISK descriptor containing initialization parameters.
 * @return 0 on success, or -ENODEV if enabling the peripheral gpios fails.
 */
static int init_gpio(struct bsp_disk_des *des)
{
	struct bsp_utils_periph_des periph_des = { 0 };
	GPIO_InitTypeDef gpio_init = { 0 };

	periph_des.addr_lo = (uint32_t)des->detect_port;
	if (bsp_utils_periph_clk(&periph_des, ENABLE))
		return -ENODEV;

	gpio_init.Mode = GPIO_MODE_INPUT;
	gpio_init.Pull = GPIO_PULLUP;
	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
	gpio_init.Pin = des->detect_pin;
	HAL_GPIO_Init(des->detect_port, &gpio_init);

	// +--------+-------------+------------------------+
	// | signal |  sdmmc1     |  sdmmc2                |
	// +--------+-------------+------------------------+
	// | D0     |  PC8  AF12  |  PB14 AF9              |
	// | D1     |  PC9  AF12  |  PB15 AF9              |
	// | D2     |  PC10 AF12  |  PB3  AF9 or PC11 AF10 |
	// | D3     |  PC11 AF12  |  PB4  AF9              |
	// | CLK    |  PC12 AF12  |  PC1  AF9 or PD6  AF11 |
	// | CMD    |  PD2  AF12  |  PA0  AF9 or PD7  AF11 |
	// +--------+-------------+------------------------+

	gpio_init.Mode = GPIO_MODE_AF_PP;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	if (des->sdmmc == SDMMC1) {
		if (des->d0_pin != GPIO_PIN_8 || des->d1_pin != GPIO_PIN_9 || des->d2_pin != GPIO_PIN_10 ||
		    des->d3_pin != GPIO_PIN_11 || des->clk_pin != GPIO_PIN_12 || des->cmd_pin != GPIO_PIN_2 ||
		    des->d0_port != GPIOC || des->d1_port != GPIOC || des->d2_port != GPIOC || des->d3_port != GPIOC ||
		    des->clk_port != GPIOC || des->cmd_port != GPIOD)
			return -ENODEV;

		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();

		gpio_init.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_8 | GPIO_PIN_9;
		gpio_init.Alternate = GPIO_AF12_SDMMC1;
		HAL_GPIO_Init(GPIOC, &gpio_init);

		gpio_init.Pin = GPIO_PIN_2;
		HAL_GPIO_Init(GPIOD, &gpio_init);
	} else if (des->sdmmc == SDMMC2) {
		if (!(des->d0_pin == GPIO_PIN_14 && des->d0_port == GPIOB && des->d1_pin == GPIO_PIN_15 &&
		      des->d1_port == GPIOB &&
		      ((des->d2_pin == GPIO_PIN_3 && des->d2_port == GPIOB) ||
		       (des->d2_pin == GPIO_PIN_11 && des->d2_port == GPIOC)) &&
		      des->d3_pin == GPIO_PIN_4 && des->d3_port == GPIOB &&
		      ((des->clk_pin == GPIO_PIN_1 && des->clk_port == GPIOC) ||
		       (des->clk_pin == GPIO_PIN_6 && des->clk_port == GPIOD)) &&
		      ((des->cmd_pin == GPIO_PIN_0 && des->cmd_port == GPIOA) ||
		       (des->cmd_pin == GPIO_PIN_7 && des->cmd_port == GPIOD))))
			return -ENODEV;

		__HAL_RCC_GPIOB_CLK_ENABLE();
		gpio_init.Pin = GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_4;
		gpio_init.Alternate = GPIO_AF9_SDMMC2;
		HAL_GPIO_Init(GPIOB, &gpio_init);
		if (des->d2_port == GPIOB) {
			gpio_init.Pin = GPIO_PIN_3;
			gpio_init.Alternate = GPIO_AF9_SDMMC2;
			HAL_GPIO_Init(GPIOB, &gpio_init);
		} else {
			__HAL_RCC_GPIOC_CLK_ENABLE();
			gpio_init.Pin = GPIO_PIN_11;
			gpio_init.Alternate = GPIO_AF10_SDMMC2;
			HAL_GPIO_Init(GPIOC, &gpio_init);
		}
		if (des->clk_port == GPIOC) {
			__HAL_RCC_GPIOC_CLK_ENABLE();
			gpio_init.Pin = GPIO_PIN_1;
			gpio_init.Alternate = GPIO_AF9_SDMMC2;
			HAL_GPIO_Init(GPIOC, &gpio_init);
		} else {
			__HAL_RCC_GPIOD_CLK_ENABLE();
			gpio_init.Pin = GPIO_PIN_6;
			gpio_init.Alternate = GPIO_AF11_SDMMC2;
			HAL_GPIO_Init(GPIOD, &gpio_init);
		}
		if (des->clk_port == GPIOA) {
			__HAL_RCC_GPIOA_CLK_ENABLE();
			gpio_init.Pin = GPIO_PIN_0;
			gpio_init.Alternate = GPIO_AF9_SDMMC2;
			HAL_GPIO_Init(GPIOA, &gpio_init);
		} else {
			__HAL_RCC_GPIOD_CLK_ENABLE();
			gpio_init.Pin = GPIO_PIN_7;
			gpio_init.Alternate = GPIO_AF11_SDMMC2;
			HAL_GPIO_Init(GPIOD, &gpio_init);
		}
	}

	return 0;
}

/**
 * @brief Initialize DMA for MMC.
 * @param des Pointer to the DISK descriptor.
 * @return 0 on success, -ENODEV or -EAGAIN on failure.
 */
static int init_dma(struct bsp_disk_des *des)
{

	return 0;
}

/**
 * @brief  Initialize the DISK module.
 *
 * @param des Pointer to the DISK descriptor.
 * @return 0 on success, or an error code on failure.
 * @note Ensure the descriptor is properly configured before calling.
 */
static int bsp_disk_init(void *des)
{
	int res;
	struct bsp_disk_des *int_des = (struct bsp_disk_des *)des;
	BSP_LOG_DECLARE();

	res = init_gpio(int_des);
	if (res) {
		BSP_LOG_ERR("failed to init GPIO");
		return res;
	}

	res = init_mmc(int_des);
	if (res) {
		BSP_LOG_ERR("failed to init MMC");
		return res;
	}

	res = init_dma(int_des);
	if (res) {
		BSP_LOG_ERR("failed to init DMA");
		return res;
	}

	return 0;
}

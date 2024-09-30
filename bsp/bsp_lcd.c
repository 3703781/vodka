#include <bsp.h>

static int bsp_lcd_init(void *des);
static void *set_default_des(void *des);

struct bsp_module bsp_lcd_mod = { .name = "LCD",
				  .state = BSP_MODULE_STATE_COMING,
				  .prepare = set_default_des,
				  .setup = bsp_lcd_init,
				  .descriptor = NULL,
				  .version = "0.1" };
BSP_MODULE_DECLARE(bsp_lcd_mod);

/**
 * @brief Set the brightness of the backlight
 * @param des LCD descriptor
 * @param brightness 0..255 corresponds to the darkest to the brightest
 * @return Return 0 on success, otherwise return an error code.
 * @note If brightness is set to 0, the backlight should be turned off.
 *       Otherwise 255 means a total on state.
 */
static int bklt_set_brightness(uint8_t brightness)
{
	struct bsp_lcd_des *des = (struct bsp_lcd_des *)bsp_lcd_mod.descriptor;
	// if (unlikely(bsp_lcd_mod.state != BSP_MODULE_STATE_LIVE))
	// 	return -EBUSY;

	__HAL_TIM_SetCompare(&des->_htim_bklt, des->bklt_timer_ch, (uint32_t)brightness);
	return 0;
}

/**
 * @brief Get the brightness of the backlight
 * @param des LCD descriptor
 * @return Return the brightness (0..255) on success, otherwise return a negative error code.
 * @note If brightness is set to 0, the backlight should be turned off.
 *       Otherwise 255 means a total on state.
 */
static int bklt_get_brightness(void)
{
	struct bsp_lcd_des *des = (struct bsp_lcd_des *)bsp_lcd_mod.descriptor;
	// if (unlikely(bsp_lcd_mod.state != BSP_MODULE_STATE_LIVE))
	// 	return -EBUSY;

	return __HAL_TIM_GetCompare(&des->_htim_bklt, des->bklt_timer_ch);
}

void HAL_LTDC_MspInit(LTDC_HandleTypeDef *hltdc)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	if (hltdc->Instance != LTDC)
		return;
	__HAL_RCC_LTDC_CLK_ENABLE();
	__HAL_RCC_GPIOK_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOJ_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOI_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE(); /* backlight */

	/*
    PI15 ------> LTDC_R0   PJ7  ------> LTDC_G0  PJ12 ------> LTDC_B0  PI12 ------> LTDC_HSYNC
    PJ0  ------> LTDC_R1   PJ8  ------> LTDC_G1  PJ13 ------> LTDC_B1  PI13 ------> LTDC_VSYNC
    PJ1  ------> LTDC_R2   PJ9  ------> LTDC_G2  PJ14 ------> LTDC_B2  PK7  ------> LTDC_DE
    PJ2  ------> LTDC_R3   PG10 ------> LTDC_G3  PJ15 ------> LTDC_B3  PI14 ------> LTDC_CLK  
    PJ3  ------> LTDC_R4   PH15 ------> LTDC_G4  PK3  ------> LTDC_B4
    PJ4  ------> LTDC_R5   PH4  ------> LTDC_G5  PK4  ------> LTDC_B5
    PJ5  ------> LTDC_R6   PK1  ------> LTDC_G6  PK5  ------> LTDC_B6
    PJ6  ------> LTDC_R7   PK2  ------> LTDC_G7  PK6  ------> LTDC_B7
    */

	GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_3 | GPIO_PIN_7 | GPIO_PIN_2 | GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_9 | GPIO_PIN_0 |
			      GPIO_PIN_8 | GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_3 | GPIO_PIN_2 |
			      GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

	/**TIM12 GPIO Configuration
	 PH6     ------> TIM12_CH1
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM12;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(LTDC_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(LTDC_IRQn);
}

/**
 * @brief Initialize uninitialized members in the LCD descriptor with default values.
 * 
 * @param des LCD descriptor
 * @return struct bsp_lcd_des * Pointer to descriptor on success, otherwise negative error code.
 */
static void *set_default_des(void *des)
{
	extern char _sgram0_ext_sdram, _sgram1_ext_sdram;
	char *sgram0 = &_sgram0_ext_sdram;
	char *sgram1 = &_sgram1_ext_sdram;

	struct bsp_lcd_des *int_des = (struct bsp_lcd_des *)des;

	if (int_des == NULL) {
		int_des = (struct bsp_lcd_des *)calloc(sizeof(struct bsp_lcd_des), 1);
		if (IS_ERR_OR_NULL(int_des))
			return ERR_PTR(-ENOMEM);
	}
	struct bsp_lcd_dev *dev = &int_des->dev;

	/* set to default value */
	SET_VAL_IF_ZERO(dev->hsw, 1);
	SET_VAL_IF_ZERO(dev->vsw, 1);
	SET_VAL_IF_ZERO(dev->hbp, 80);
	SET_VAL_IF_ZERO(dev->vbp, 40);
	SET_VAL_IF_ZERO(dev->hfp, 200);
	SET_VAL_IF_ZERO(dev->vfp, 22);
	SET_VAL_IF_ZERO(dev->width, 800);
	SET_VAL_IF_ZERO(dev->height, 480);
	SET_VAL_IF_NULL(int_des->bklt_ops.set_brightness, bklt_set_brightness);
	SET_VAL_IF_NULL(int_des->bklt_ops.get_brightness, bklt_get_brightness);
	// SET_VAL_IF_NULL(int_des->layer_gram_addr[0], calloc(3 * dev->width * dev->height, 1));
	// if (IS_ERR_OR_NULL(int_des->layer_gram_addr[0]))
	// 	return ERR_PTR(-ENOMEM);
	SET_VAL_IF_NULL(int_des->layer_gram_addr[0], sgram0);
	SET_VAL_IF_NULL(int_des->layer_gram_addr[1], sgram1);

	SET_VAL_IF_ZERO(int_des->layer_num, 1);
	SET_VAL_IF_ZERO(int_des->bklt_pin, GPIO_PIN_6);
	SET_VAL_IF_NULL(int_des->bklt_port, GPIOH);
	SET_VAL_IF_NULL(int_des->bklt_timer, TIM12);
	SET_VAL_IF_ZERO(int_des->bklt_timer_ch, TIM_CHANNEL_1);
	return int_des;
}

/**
 * @brief Initialize the LTDC
 * 
 * This function initializes the LTDC peripheral based on the provided configuration parameters.
 * 
 * @param init Pointer to the LTDC initialization structure
 * @return int Returns 0 on success, otherwise returns a negative error code
 */
static inline int init_ltdc(LTDC_InitTypeDef *init)
{
	struct bsp_lcd_des *des =
		CONTAINER_OF(CONTAINER_OF(init, LTDC_HandleTypeDef, Init), struct bsp_lcd_des, _hltdc);
	struct bsp_lcd_dev *dev = &des->dev;
	BSP_LOG_DECLARE();
	BSP_LOG_TRC("");

	des->_hltdc.Instance = LTDC;

	/* electrical params */
	init->HSPolarity = LTDC_HSPOLARITY_AL;
	init->VSPolarity = LTDC_VSPOLARITY_AL;
	init->DEPolarity = LTDC_DEPOLARITY_AL;
	init->PCPolarity = LTDC_PCPOLARITY_IPC;
	init->HorizontalSync = dev->hsw - 1;
	init->VerticalSync = dev->vsw - 1;
	init->AccumulatedHBP = dev->hbp + init->HorizontalSync;
	init->AccumulatedVBP = dev->vbp + init->VerticalSync;
	init->AccumulatedActiveW = dev->width + init->AccumulatedHBP;
	init->AccumulatedActiveH = dev->height + init->AccumulatedVBP;
	init->TotalWidth = init->AccumulatedActiveW + dev->hfp;
	init->TotalHeigh = init->AccumulatedActiveH + dev->vfp;

	/* bakcground initial color */
	init->Backcolor.Red = 0;
	init->Backcolor.Green = 0;
	init->Backcolor.Blue = 0;

	if (HAL_LTDC_Init(&des->_hltdc) != HAL_OK)
		return -ENODEV;

	if (HAL_LTDC_DisableDither(&des->_hltdc) != HAL_OK)
		return -ENODEV;

	return 0;
}

/**
 * @brief Initialize LTDC layer configurations
 * 
 * This function initializes the LTDC layer configurations based on the provided structure.
 * 
 * @param cfg Pointer to the LTDC layer configuration structure
 * @return int Returns 0 on success, otherwise returns a negative error code
 */
static inline int init_layer(LTDC_LayerCfgTypeDef *cfg)
{
	struct bsp_lcd_des *des = CONTAINER_OF((LTDC_LayerCfgTypeDef(*)[2])cfg, struct bsp_lcd_des, _lyr_cfg);
	struct bsp_lcd_dev *dev = &des->dev;

	/* layers config */
	for (size_t i = 0; i < des->layer_num; i++) {
		cfg[i].WindowX0 = 0;
		cfg[i].WindowX1 = dev->width;
		cfg[i].WindowY0 = 0;
		cfg[i].WindowY1 = dev->height;
		cfg[i].ImageWidth = dev->width;
		cfg[i].ImageHeight = dev->height;
		cfg[i].PixelFormat = LTDC_PIXEL_FORMAT_RGB888;

		/* color =  BF1 * layer0 + BF2 * background */
		cfg[i].Alpha = 255;
		cfg[i].BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
		cfg[i].BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
		cfg[i].FBStartAdress = (uint32_t)(des->layer_gram_addr[i]);

		/* layer0 initial color */
		cfg[i].Alpha0 = 0;
		cfg[i].Backcolor.Blue = 0;
		cfg[i].Backcolor.Green = 0;
		cfg[i].Backcolor.Red = 0;

		if (HAL_LTDC_ConfigLayer(&des->_hltdc, &(cfg[i]), i) != HAL_OK)
			return -ENODEV;
	}
	return 0;
}

/**
 * @brief Initialize backlight PWM
 * 
 * This function initializes the PWM configurations based on the provided configuration parameters.
 * 
 * @param htim Pointer to the timer handle
 * @return int Returns 0 on success, otherwise returns a negative error code
 */
static inline int init_bklt(TIM_HandleTypeDef *htim)
{
	TIM_MasterConfigTypeDef master_config = { 0 };
	TIM_OC_InitTypeDef oc_config = { 0 };
	struct bsp_lcd_des *des = CONTAINER_OF(htim, struct bsp_lcd_des, _htim_bklt);
	struct bsp_utils_periph_des ph_des = { .addr_lo = (uint32_t)des->bklt_timer };

	if (bsp_utils_periph_clk(&ph_des, ENABLE)) {
		return -ENODEV;
	}

	htim->Instance = des->bklt_timer;
	htim->Init.Prescaler = 1199; // 240_000_000 Hz / 1_2000 = 2000 kHz
	htim->Init.CounterMode = TIM_COUNTERMODE_UP;
	htim->Init.Period = 255; // 2_000_000 Hz / 256 = 7812.5 Hz
	htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_PWM_Init(htim) != HAL_OK) {
		return -ENODEV;
	}
	master_config.MasterOutputTrigger = TIM_TRGO_RESET;
	master_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(htim, &master_config) != HAL_OK) {
		return -ENODEV;
	}
	oc_config.OCMode = TIM_OCMODE_PWM1;
	oc_config.Pulse = 5;
	oc_config.OCPolarity = TIM_OCPOLARITY_HIGH;
	oc_config.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(htim, &oc_config, des->bklt_timer_ch) != HAL_OK) {
		return -ENODEV;
	}
	__HAL_TIM_SetCompare(htim, des->bklt_timer_ch, 0); // turn off
	HAL_TIM_PWM_Start(htim, des->bklt_timer_ch);

	return 0;
}

/**
 * @brief Initialize LCD as the descriptor instructed.
 * 
 * @param des The LCD descriptor to provide configration info for the LCD. 
 *            If it is NULL, a descriptor with default values will be created for initialization, 
 *            and this descriptor will be returned with parameters and return values
 * @return int Returns 0 on success, otherwise returns a negative error code
 */
static int bsp_lcd_init(void *des)
{
	int res;
	struct bsp_lcd_des *int_des = (struct bsp_lcd_des *)des;

	res = init_bklt(&int_des->_htim_bklt);
	CALL_IF_NOT_NULL(int_des->bklt_ops.set_brightness, 128); // set bklt
	if (res)
		return -EBUSY;

	res = init_ltdc(&int_des->_hltdc.Init);
	if (res)
		return -EBUSY;

	res = init_layer(int_des->_lyr_cfg);
	if (res)
		return -EBUSY;

	res = HAL_LTDC_ProgramLineEvent(&int_des->_hltdc, int_des->_hltdc.Init.AccumulatedActiveH + 1);
	if (res != HAL_OK)
		return -EBUSY;

	return 0;
}

/**
 * @param x [0,479]
 * @param y [0,271]
 * @param width less than 480
 * @param height less than 272
 * @param color where the data from
 */
void bsp_lcd_cpybuf(struct bsp_lcd_des *des, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t *color)
{
	DMA2D->CR &= ~(DMA2D_CR_START); /* stop transffering */
	DMA2D->CR = DMA2D_M2M;
	DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_RGB888;
	DMA2D->FGOR = 0;
	DMA2D->OOR = des->dev.width - width;
	DMA2D->FGMAR = (uint32_t)color;
	DMA2D->OMAR = (uint32_t)des->layer_gram_addr[0] + 3 * (des->dev.width * y + x);
	DMA2D->NLR = (width << 16) | (height);
	DMA2D->CR |= DMA2D_CR_START;

	while (DMA2D->CR & DMA2D_CR_START);
}

void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef *hltdc)
{
	struct bsp_lcd_des *des = CONTAINER_OF(hltdc, struct bsp_lcd_des, _hltdc);
	des->bklt_ops.set_brightness(60);

	uint32_t color = (uint32_t)rand();
	uint8_t *tmp = des->layer_gram_addr[0];

	for (size_t i = 0; i < des->dev.width * des->dev.height; i++) {
		*(uint32_t *)tmp = color;
		tmp += 3;
	}
	__DSB();
	SCB_CleanInvalidateDCache();
	HAL_LTDC_ProgramLineEvent(&des->_hltdc, des->_hltdc.Init.AccumulatedActiveH + 1);
	__HAL_LTDC_CLEAR_FLAG(hltdc, LTDC_FLAG_LI);
}
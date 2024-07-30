#include <bsp_utils.h>
#include <bsp_lcd.h>
#include <bsp_module.h>
#include <stdlib.h>
#include <string.h>

static void *bsp_lcd_init(void *desc);

struct bsp_module bsp_lcd_mod = { .name = "LCD",
				  .state = BSP_MODULE_STATE_COMING,
				  .setup = bsp_lcd_init,
				  .descriptor = NULL,
				  .version = "0.1" };

static void bklt_off_default(struct bsp_lcd_des *des)
{
	HAL_GPIO_WritePin(des->bklt_port, des->bklt_pin, GPIO_PIN_RESET);
}

static void bklt_on_default(struct bsp_lcd_des *des)
{
	HAL_GPIO_WritePin(des->bklt_port, des->bklt_pin, GPIO_PIN_SET);
}

static void bklt_set_brightness(struct bsp_lcd_des *des)
{
	
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

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
}

/**
 * @brief Initialize uninitialized members in the LCD descriptor with default values.
 * 
 * @param des LCD descriptor
 * @return Return 0 on success, otherwise return an error code.
 */
static inline int set_default_des(struct bsp_lcd_des *des)
{
	struct bsp_lcd_dev *dev = &des->dev;

	/* set to default value */
	SET_VAL_IF_ZERO(dev->hsw, 1);
	SET_VAL_IF_ZERO(dev->vsw, 1);
	SET_VAL_IF_ZERO(dev->hsw, 1);
	SET_VAL_IF_ZERO(dev->hbp, 80);
	SET_VAL_IF_ZERO(dev->vbp, 40);
	SET_VAL_IF_ZERO(dev->hfp, 200);
	SET_VAL_IF_ZERO(dev->vfp, 22);
	SET_VAL_IF_ZERO(dev->width, 800);
	SET_VAL_IF_ZERO(dev->height, 480);
	SET_VAL_IF_NULL(des->bklt_ops.turn_on, bklt_on_default);
	SET_VAL_IF_NULL(des->bklt_ops.turn_off, bklt_off_default);
	SET_VAL_IF_NULL(des->layer_gram_addr[0], calloc(3 * dev->width * dev->height, 1));
	if (IS_ERR_OR_NULL(des->layer_gram_addr[0]))
		return -ENOMEM;
	SET_VAL_IF_ZERO(des->layer_num, 1);
	SET_VAL_IF_ZERO(des->bklt_pin, GPIO_PIN_6);
	SET_VAL_IF_NULL(des->bklt_port, GPIOH);
	SET_VAL_IF_NULL(des->bklt_timer, TIM12);
	SET_VAL_IF_ZERO(des->bklt_timer_ch, 1);
	return 0;
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
	if (HAL_LTDC_EnableDither(&des->_hltdc) != HAL_OK)
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

static inline int init_bklt(struct bsp_lcd_des * des)
{
	// TIM_HandleTypeDef tim_handle;
	// __HAL_RCC_TIM12_CLK_ENABLE();
	// tim_handle.Channel = HAL_TIM_ACTIVE_CHANNEL_1;
	// tim_handle.Init.AutoReloadPreload = ();
	// HAL_TIM_PWM_Init()
	return 0;
}

/**
 * @brief Initialize LCD as the descriptor instructed.
 * 
 * @param des The LCD descriptor to provide configration info for the LCD. 
 *            If it is NULL, a descriptor with default values will be created for initialization, 
 *            and this descriptor will be returned with parameters and return values
 * @return struct bsp_lcd_des * Pointer to descriptor on success, otherwise negative error code.
 */
static void *bsp_lcd_init(void *des)
{
	int res;
	struct bsp_lcd_des *int_des = (struct bsp_lcd_des *)des;
	if (des == NULL) {
		int_des = (struct bsp_lcd_des *)calloc(sizeof(struct bsp_lcd_des), 1);
		if (IS_ERR_OR_NULL(int_des))
			goto init_des_err;
	}
	res = set_default_des(int_des);
	if (res)
		goto set_des_err;

	res = init_ltdc(&int_des->_hltdc.Init);
	CALL_IF_NOT_NULL(int_des->bklt_ops.turn_off, int_des);
	if (res)
		goto ltdc_err;

	CALL_IF_NOT_NULL(int_des->bklt_ops.turn_on, int_des);
	res = init_layer(int_des->_lyr_cfg);
	if (res)
		goto layer_err;

	while (1) {
		uint32_t color = (uint32_t)rand();
		uint8_t *tmp = int_des->layer_gram_addr[0];
		for (size_t i = 0; i < int_des->dev.width * int_des->dev.height; i++) {
			*(uint32_t *)tmp = color;
			tmp += 3;
		}
		SCB_CleanDCache();
		HAL_Delay(200);
	}
	return int_des;

ltdc_err:
layer_err:
	free(int_des);
	return ERR_PTR(res);
init_des_err:
set_des_err:
	return ERR_PTR(-ENOMEM);
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

	while (DMA2D->CR & DMA2D_CR_START)
		;
}

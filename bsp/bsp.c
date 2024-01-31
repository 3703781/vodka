#include "bsp.h"

static void bsp_clk_init(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	__HAL_RCC_SYSCFG_CLK_ENABLE();

	/* Supply configuration update enable */
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

	/* Configure the main internal regulator output voltage */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
	}

	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
	}

	/* Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 5;
	RCC_OscInitStruct.PLL.PLLN = 192;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	assert(HAL_RCC_OscConfig(&RCC_OscInitStruct) == HAL_OK);
	/* Initializes the CPU, AHB and APB buses clocks */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
				      RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

	assert(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) == HAL_OK);
}

static void bsp_mpu_init(void)
{
	MPU_Region_InitTypeDef MPU_InitStruct = { 0 };

	/* Disables the MPU */
	HAL_MPU_Disable();

	/* Initializes and configures the Region and the memory to be protected */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	MPU_InitStruct.BaseAddress = 0x24000000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
	MPU_InitStruct.SubRegionDisable = 0;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	MPU_InitStruct.Number = MPU_REGION_NUMBER1;
	MPU_InitStruct.BaseAddress = 0xc0000000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	/* Enables the MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

static void bsp_cache_init(void)
{
	SCB_EnableICache();
	SCB_EnableDCache();
}

static void bsp_crt0(void)
{
	extern char _sdata_ext_sdram, _edata_ext_sdram, _sidata_ext_sdram;
	extern char _sbss_ext_sdram, _ebss_ext_sdram;
	volatile char *src = &_sidata_ext_sdram;
	volatile char *dst = &_sdata_ext_sdram;

	while (dst < &_edata_ext_sdram)
		*dst++ = *src++;

	dst = &_sbss_ext_sdram;
	while (dst < &_ebss_ext_sdram)
		*dst++ = 0;

	SCB_CleanDCache();
	SCB_InvalidateDCache();
}

static void bsp_debug_init(void)
{
	GPIO_InitTypeDef gpio_init;
	gpio_init.Pin = GPIO_PIN_3;
	gpio_init.Mode = GPIO_MODE_AF_PP;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_init.Alternate = GPIO_AF0_TRACE;

	HAL_GPIO_Init(GPIOB, &gpio_init);
	// CoreDebug->DEMCR |= CoreDebug_DHCSR_S_RETIRE_ST_Msk;
	//  ITM->TCR |= ITM_TCR_ITMENA_Msk;
	// ITM->TER |= 1UL;
}

static void bsp_io_common_init(void)
{
	/* PA14 (JTCK/SWCLK)   ------> DEBUG_JTCK-SWCLK
	PC15-OSC32_OUT (OSC32_OUT)   ------> RCC_OSC32_OUT
	PC14-OSC32_IN (OSC32_IN)   ------> RCC_OSC32_IN
	PB3 (JTDO/TRACESWO)   ------> DEBUG_JTDO-SWO
	PA13 (JTMS/SWDIO)   ------> DEBUG_JTMS-SWDIO
	PH1-OSC_OUT (PH1)   ------> RCC_OSC_OUT
	PH0-OSC_IN (PH0)   ------> RCC_OSC_IN */
	__HAL_RCC_GPIOI_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	HAL_SYSCFG_CompensationCodeSelect(SYSCFG_CELL_CODE);
	HAL_EnableCompensationCell();
}

void bsp_init(void)
{
	bsp_mpu_init();
	bsp_cache_init();
	HAL_Init();
	bsp_clk_init();
	bsp_io_common_init();
	bsp_debug_init();
	bsp_sdram_init();
	bsp_crt0();
	bsp_led_init();
}

#include "bsp_sdram.h"

#define SDRAM_MODEREG_BURST_LENGTH_1 ((uint16_t)0)
#define SDRAM_MODEREG_BURST_LENGTH_2 ((uint16_t)1)
#define SDRAM_MODEREG_BURST_LENGTH_4 ((uint16_t)2)
#define SDRAM_MODEREG_BURST_LENGTH_8 ((uint16_t)4)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL ((uint16_t)0)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED ((uint16_t)8)
#define SDRAM_MODEREG_CAS_LATENCY_2 ((uint16_t)0x20)
#define SDRAM_MODEREG_CAS_LATENCY_3 ((uint16_t)0x30)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD ((uint16_t)0)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE ((uint16_t)0x200)

static SDRAM_HandleTypeDef hsdram1;

/* FMC initialization function */
void bsp_sdram_init(void)
{
	FMC_SDRAM_TimingTypeDef SdramTiming = { 0 };
	FMC_SDRAM_CommandTypeDef cmd;

	/** Perform the SDRAM1 memory initialization sequence */
	hsdram1.Instance = FMC_SDRAM_DEVICE;
	/* hsdram1.Init */
	hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
	hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
	hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
	hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
	hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
	hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
	hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
	hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
	hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
	hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;
	/* SdramTiming */
	SdramTiming.LoadToActiveDelay = 2;
	SdramTiming.ExitSelfRefreshDelay = 7;
	SdramTiming.SelfRefreshTime = 4;
	SdramTiming.RowCycleDelay = 7;
	SdramTiming.WriteRecoveryTime = 3;
	SdramTiming.RPDelay = 2;
	SdramTiming.RCDDelay = 2;

	// SdramTiming.LoadToActiveDelay = 2;
	// SdramTiming.ExitSelfRefreshDelay = 11;
	// SdramTiming.SelfRefreshTime = 7;
	// SdramTiming.RowCycleDelay = 9;
	// SdramTiming.WriteRecoveryTime = 2;
	// SdramTiming.RPDelay = 3;
	// SdramTiming.RCDDelay = 3;

	assert(HAL_SDRAM_Init(&hsdram1, &SdramTiming) == HAL_OK);

	cmd.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
	cmd.AutoRefreshNumber = 1;
	cmd.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
	cmd.ModeRegisterDefinition = 0;
	assert(HAL_SDRAM_SendCommand(&hsdram1, &cmd, 0x1000) == HAL_OK);

	HAL_Delay(1);

	cmd.CommandMode = FMC_SDRAM_CMD_PALL;
	assert(HAL_SDRAM_SendCommand(&hsdram1, &cmd, 0x1000) == HAL_OK);

	cmd.AutoRefreshNumber = 8;
	cmd.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	assert(HAL_SDRAM_SendCommand(&hsdram1, &cmd, 0x1000) == HAL_OK);

	cmd.ModeRegisterDefinition = SDRAM_MODEREG_BURST_LENGTH_2 | SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
				     SDRAM_MODEREG_CAS_LATENCY_3 | SDRAM_MODEREG_OPERATING_MODE_STANDARD |
				     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
	cmd.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
	cmd.AutoRefreshNumber = 1;
	assert(HAL_SDRAM_SendCommand(&hsdram1, &cmd, 0x1000) == HAL_OK);

	HAL_SDRAM_ProgramRefreshRate(&hsdram1, 918);
}

static uint32_t fmc_initialized = 0;

void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *sdramHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	if (fmc_initialized) {
		return;
	}
	fmc_initialized = 1;
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

	UNUSED(sdramHandle);

	/** Initializes the peripherals clock */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FMC;
	PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_D1HCLK;
	assert(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) == HAL_OK);
	/* Peripheral clock enable */
	__HAL_RCC_FMC_CLK_ENABLE();

	/** FMC GPIO Configuration
  PE1   ------> FMC_NBL1
  PE0   ------> FMC_NBL0
  PG15   ------> FMC_SDNCAS
  PD0   ------> FMC_D2
  PD1   ------> FMC_D3
  PG8   ------> FMC_SDCLK
  PF2   ------> FMC_A2
  PF1   ------> FMC_A1
  PF0   ------> FMC_A0
  PG5   ------> FMC_BA1
  PF3   ------> FMC_A3
  PG4   ------> FMC_BA0
  PG2   ------> FMC_A12
  PF5   ------> FMC_A5
  PF4   ------> FMC_A4
  PC0   ------> FMC_SDNWE
  PH2   ------> FMC_SDCKE0
  PE10   ------> FMC_D7
  PH3   ------> FMC_SDNE0
  PF13   ------> FMC_A7
  PF14   ------> FMC_A8
  PE9   ------> FMC_D6
  PE11   ------> FMC_D8
  PD15   ------> FMC_D1
  PD14   ------> FMC_D0
  PF12   ------> FMC_A6
  PF15   ------> FMC_A9
  PE12   ------> FMC_D9
  PE15   ------> FMC_D12
  PF11   ------> FMC_SDNRAS
  PG0   ------> FMC_A10
  PE8   ------> FMC_D5
  PE13   ------> FMC_D10
  PD10   ------> FMC_D15
  PD9   ------> FMC_D14
  PG1   ------> FMC_A11
  PE7   ------> FMC_D4
  PE14   ------> FMC_D11
  PD8   ------> FMC_D13
  */
	/* GPIO_InitStruct */
	GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_0 | GPIO_PIN_10 | GPIO_PIN_9 | GPIO_PIN_11 | GPIO_PIN_12 |
			      GPIO_PIN_15 | GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_7 | GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/* GPIO_InitStruct */
	GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_8 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_2 | GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/* GPIO_InitStruct */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_10 | GPIO_PIN_9 |
			      GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* GPIO_InitStruct */
	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_4 |
			      GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_12 | GPIO_PIN_15 | GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	/* GPIO_InitStruct */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* GPIO_InitStruct */
	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

	HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
}

static uint32_t fmc_deinitialized = 0;

void HAL_SDRAM_MspDeInit(SDRAM_HandleTypeDef *sdramHandle)
{
	UNUSED(sdramHandle);
	if (fmc_deinitialized) {
		return;
	}
	fmc_deinitialized = 1;
	/* Peripheral clock enable */
	__HAL_RCC_FMC_CLK_DISABLE();

	/** FMC GPIO Configuration
  PE1   ------> FMC_NBL1
  PE0   ------> FMC_NBL0
  PG15   ------> FMC_SDNCAS
  PD0   ------> FMC_D2
  PD1   ------> FMC_D3
  PG8   ------> FMC_SDCLK
  PF2   ------> FMC_A2
  PF1   ------> FMC_A1
  PF0   ------> FMC_A0
  PG5   ------> FMC_BA1
  PF3   ------> FMC_A3
  PG4   ------> FMC_BA0
  PG2   ------> FMC_A12
  PF5   ------> FMC_A5
  PF4   ------> FMC_A4
  PC0   ------> FMC_SDNWE
  PH2   ------> FMC_SDCKE0
  PE10   ------> FMC_D7
  PH3   ------> FMC_SDNE0
  PF13   ------> FMC_A7
  PF14   ------> FMC_A8
  PE9   ------> FMC_D6
  PE11   ------> FMC_D8
  PD15   ------> FMC_D1
  PD14   ------> FMC_D0
  PF12   ------> FMC_A6
  PF15   ------> FMC_A9
  PE12   ------> FMC_D9
  PE15   ------> FMC_D12
  PF11   ------> FMC_SDNRAS
  PG0   ------> FMC_A10
  PE8   ------> FMC_D5
  PE13   ------> FMC_D10
  PD10   ------> FMC_D15
  PD9   ------> FMC_D14
  PG1   ------> FMC_A11
  PE7   ------> FMC_D4
  PE14   ------> FMC_D11
  PD8   ------> FMC_D13
  */

	HAL_GPIO_DeInit(GPIOE, GPIO_PIN_1 | GPIO_PIN_0 | GPIO_PIN_10 | GPIO_PIN_9 | GPIO_PIN_11 | GPIO_PIN_12 |
				       GPIO_PIN_15 | GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_7 | GPIO_PIN_14);
	HAL_GPIO_DeInit(GPIOG,
			GPIO_PIN_15 | GPIO_PIN_8 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_2 | GPIO_PIN_0 | GPIO_PIN_1);
	HAL_GPIO_DeInit(GPIOD,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_10 | GPIO_PIN_9 | GPIO_PIN_8);
	HAL_GPIO_DeInit(GPIOF, GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_4 |
				       GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_12 | GPIO_PIN_15 | GPIO_PIN_11);
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);
	HAL_GPIO_DeInit(GPIOH, GPIO_PIN_2 | GPIO_PIN_3);
}

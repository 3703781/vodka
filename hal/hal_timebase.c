#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_tim.h"

TIM_HandleTypeDef htim7;

/**
 * @brief This function configures the source of the time base.
 *        The time source is configured  to have 1ms time base with a dedicated
 *        Tick interrupt priority.
 * @note This function is called  automatically at the beginning of program after
 *       reset by HAL_Init() or at any time when clock is reconfigured  by HAL_RCC_ClockConfig().
 * @param TickPriority: Tick interrupt priority.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
	RCC_ClkInitTypeDef clkconfig;
	uint32_t uwTimclock, uwAPB1Prescaler;

	uint32_t uwPrescalerValue;
	uint32_t pFLatency;
	/*Configure the TIM7 IRQ priority */
	if (TickPriority < (1UL << __NVIC_PRIO_BITS)) {
		HAL_NVIC_SetPriority(TIM7_IRQn, TickPriority, 0);

		/* Enable the TIM7 global Interrupt */
		HAL_NVIC_EnableIRQ(TIM7_IRQn);
		uwTickPrio = TickPriority;
	} else {
		return HAL_ERROR;
	}

	/* Enable TIM7 clock */
	__HAL_RCC_TIM7_CLK_ENABLE();
	/* Get clock configuration */
	HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
	/* Get APB1 prescaler */
	uwAPB1Prescaler = clkconfig.APB1CLKDivider;
	/* Compute TIM7 clock */
	if (uwAPB1Prescaler == RCC_HCLK_DIV1)
		uwTimclock = HAL_RCC_GetPCLK1Freq();
	else
		uwTimclock = 2UL * HAL_RCC_GetPCLK1Freq();

	/* Compute the prescaler value to have TIM7 counter clock equal to 1MHz */
	uwPrescalerValue = (uint32_t)((uwTimclock / 1000000U) - 1U);

	/* Initialize TIM7 */
	htim7.Instance = TIM7;

	/* Initialize TIMx peripheral as follow:

	+ Period = [(TIM7CLK/1000) - 1]. to have a (1/1000) s time base.
	+ Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
	+ ClockDivision = 0
	+ Counter direction = Up
	*/
	htim7.Init.Period = (1000000U / 1000U) - 1U;
	htim7.Init.Prescaler = uwPrescalerValue;
	htim7.Init.ClockDivision = 0;
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;

	if (HAL_TIM_Base_Init(&htim7) == HAL_OK) {
		/* Start the TIM time Base generation in interrupt mode */
		return HAL_TIM_Base_Start_IT(&htim7);
	}

	/* Return function status */
	return HAL_ERROR;
}

/**
 * @brief  Suspend Tick increment.
 * @note   Disable the tick increment by disabling TIM7 update interrupt.
 * @param  None
 * @retval None
 */
void HAL_SuspendTick(void)
{
	/* Disable TIM7 update Interrupt */
	__HAL_TIM_DISABLE_IT(&htim7, TIM_IT_UPDATE);
}

/**
 * @brief  Resume Tick increment.
 * @note   Enable the tick increment by Enabling TIM7 update interrupt.
 * @param  None
 * @retval None
 */
void HAL_ResumeTick(void)
{
	/* Enable TIM7 Update interrupt */
	__HAL_TIM_ENABLE_IT(&htim7, TIM_IT_UPDATE);
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM7 interrupt took place,
 * It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  None
 * @retval None
 */
void TIM7_IRQHandler(void)
{
	if (__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE)) {
		__HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE);
		HAL_IncTick();
	}
}
#include "bsp_utils.h"

/**
 * @brief table of descriptors of all the buses
 */
static const struct bsp_utils_bus_des bsp_utils_bus_des[] = {
	{ .addr_lo = 0x40000000, .addr_hi = 0x4000D400, .name = "apb1", .get_freq = HAL_RCC_GetPCLK1Freq },
	{ .addr_lo = 0x40010000, .addr_hi = 0x40017800, .name = "apb2", .get_freq = HAL_RCC_GetPCLK2Freq },
	{ .addr_lo = 0x40020000, .addr_hi = 0x400C0000, .name = "ahb1", .get_freq = HAL_RCC_GetHCLKFreq },
	{ .addr_lo = 0x48020000, .addr_hi = 0x48022C00, .name = "ahb2", .get_freq = HAL_RCC_GetHCLKFreq },
	{ .addr_lo = 0x50000000, .addr_hi = 0x50004000, .name = "apb3", .get_freq = HAL_RCCEx_GetD1PCLK1Freq },
	{ .addr_lo = 0x51000000, .addr_hi = 0x52009000, .name = "ahb3", .get_freq = HAL_RCC_GetHCLKFreq },
	{ .addr_lo = 0x58000000, .addr_hi = 0x58006C00, .name = "apb4", .get_freq = HAL_RCCEx_GetD3PCLK1Freq },
	{ .addr_lo = 0x58020000, .addr_hi = 0x580267FF, .name = "ahb4", .get_freq = HAL_RCC_GetHCLKFreq }
};

#define BSP_UTILS_APB1_DES (&bsp_utils_bus_des[0])
#define BSP_UTILS_APB2_DES (&bsp_utils_bus_des[1])
#define BSP_UTILS_AHB1_DES (&bsp_utils_bus_des[2])
#define BSP_UTILS_AHB2_DES (&bsp_utils_bus_des[3])
#define BSP_UTILS_APB3_DES (&bsp_utils_bus_des[4])
#define BSP_UTILS_AHB3_DES (&bsp_utils_bus_des[5])
#define BSP_UTILS_APB4_DES (&bsp_utils_bus_des[6])
#define BSP_UTILS_AHB4_DES (&bsp_utils_bus_des[7])

/**
 * @brief table of descriptors of all the peripherals
 */
static const struct bsp_utils_periph_des bsp_utils_periph_des[] = {
	{ .addr_lo = 0x40000000, .addr_hi = 0x400003FF, .name = "TIM2", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40000400, .addr_hi = 0x400007FF, .name = "TIM3", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40000800, .addr_hi = 0x40000BFF, .name = "TIM4", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40000C00, .addr_hi = 0x40000FFF, .name = "TIM5", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40001000, .addr_hi = 0x400013FF, .name = "TIM6", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40001400, .addr_hi = 0x400017FF, .name = "TIM7", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40001800, .addr_hi = 0x40001BFF, .name = "TIM12", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40001C00, .addr_hi = 0x40001FFF, .name = "TIM13", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40002000, .addr_hi = 0x400023FF, .name = "TIM14", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40002400, .addr_hi = 0x400027FF, .name = "LPTIM1", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40002C00, .addr_hi = 0x40002FFF, .name = "RESERVED", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40003800, .addr_hi = 0x40003BFF, .name = "SPI2", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40003C00, .addr_hi = 0x40003FFF, .name = "SPI3", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40004000, .addr_hi = 0x400043FF, .name = "SPDIFRX", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40004400, .addr_hi = 0x400047FF, .name = "USART2", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40004800, .addr_hi = 0x40004BFF, .name = "USART3", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40004C00, .addr_hi = 0x40004FFF, .name = "UART4", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40005000, .addr_hi = 0x400053FF, .name = "UART5", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40005400, .addr_hi = 0x400057FF, .name = "I2C1", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40005800, .addr_hi = 0x40005BFF, .name = "I2C2", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40005C00, .addr_hi = 0x40005FFF, .name = "I2C3", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40006C00, .addr_hi = 0x40006FFF, .name = "HDMI_CEC", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40007400, .addr_hi = 0x400077FF, .name = "DAC1", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40007800, .addr_hi = 0x40007BFF, .name = "UART7", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40007C00, .addr_hi = 0x40007FFF, .name = "UART8", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40008400, .addr_hi = 0x400087FF, .name = "CRS", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40008800, .addr_hi = 0x40008BFF, .name = "SWPMI", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40009000, .addr_hi = 0x400093FF, .name = "OPAMP", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40009400, .addr_hi = 0x400097FF, .name = "MDIOS", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x4000A000, .addr_hi = 0x4000A3FF, .name = "FDCAN1", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x4000A400, .addr_hi = 0x4000A7FF, .name = "FDCAN2", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x4000A800, .addr_hi = 0x4000D3FF, .name = "FDCAN", .bus_des = BSP_UTILS_APB1_DES },
	{ .addr_lo = 0x40010000, .addr_hi = 0x400103FF, .name = "TIM1", .bus_des = BSP_UTILS_APB2_DES },
	{ .addr_lo = 0x40010400, .addr_hi = 0x400107FF, .name = "TIM8", .bus_des = BSP_UTILS_APB2_DES },
	{ .addr_lo = 0x40011000, .addr_hi = 0x400113FF, .name = "USART1", .bus_des = BSP_UTILS_APB2_DES },
	{ .addr_lo = 0x40011400, .addr_hi = 0x400117FF, .name = "USART6", .bus_des = BSP_UTILS_APB2_DES },
	{ .addr_lo = 0x40013000, .addr_hi = 0x400133FF, .name = "SPI1", .bus_des = BSP_UTILS_APB2_DES },
	{ .addr_lo = 0x40013400, .addr_hi = 0x400137FF, .name = "SPI4", .bus_des = BSP_UTILS_APB2_DES },
	{ .addr_lo = 0x40014000, .addr_hi = 0x400143FF, .name = "TIM15", .bus_des = BSP_UTILS_APB2_DES },
	{ .addr_lo = 0x40014400, .addr_hi = 0x400147FF, .name = "TIM16", .bus_des = BSP_UTILS_APB2_DES },
	{ .addr_lo = 0x40014800, .addr_hi = 0x40014BFF, .name = "TIM17", .bus_des = BSP_UTILS_APB2_DES },
	{ .addr_lo = 0x40015000, .addr_hi = 0x400153FF, .name = "SPI5", .bus_des = BSP_UTILS_APB2_DES },
	{ .addr_lo = 0x40015800, .addr_hi = 0x40015BFF, .name = "SAI1", .bus_des = BSP_UTILS_APB2_DES },
	{ .addr_lo = 0x40015C00, .addr_hi = 0x40015FFF, .name = "SAI2", .bus_des = BSP_UTILS_APB2_DES },
	{ .addr_lo = 0x40016000, .addr_hi = 0x400163FF, .name = "SAI3", .bus_des = BSP_UTILS_APB2_DES },
	{ .addr_lo = 0x40017000, .addr_hi = 0x400173FF, .name = "DFSDM1", .bus_des = BSP_UTILS_APB2_DES },
	{ .addr_lo = 0x40017400, .addr_hi = 0x400177FF, .name = "HRT1M", .bus_des = BSP_UTILS_APB2_DES },
	{ .addr_lo = 0x40020000, .addr_hi = 0x400203FF, .name = "DMA1", .bus_des = BSP_UTILS_AHB1_DES },
	{ .addr_lo = 0x40020400, .addr_hi = 0x400207FF, .name = "DMA2", .bus_des = BSP_UTILS_AHB1_DES },
	{ .addr_lo = 0x40020800, .addr_hi = 0x40020BFF, .name = "DMAMUX1", .bus_des = BSP_UTILS_AHB1_DES },
	{ .addr_lo = 0x40022000, .addr_hi = 0x400223FF, .name = "ADC", .bus_des = BSP_UTILS_AHB1_DES },
	{ .addr_lo = 0x40024400, .addr_hi = 0x400247FF, .name = "RESERVED", .bus_des = BSP_UTILS_AHB1_DES },
	{ .addr_lo = 0x40028000, .addr_hi = 0x400293FF, .name = "MAC", .bus_des = BSP_UTILS_AHB1_DES },
	{ .addr_lo = 0x40040000, .addr_hi = 0x4007FFFF, .name = "OTG_HS", .bus_des = BSP_UTILS_AHB1_DES },
	{ .addr_lo = 0x40080000, .addr_hi = 0x400BFFFF, .name = "OTG_FS", .bus_des = BSP_UTILS_AHB1_DES },
	{ .addr_lo = 0x48020000, .addr_hi = 0x480203FF, .name = "DCMI", .bus_des = BSP_UTILS_AHB2_DES },
	{ .addr_lo = 0x48021000, .addr_hi = 0x480213FF, .name = "CRYPTO", .bus_des = BSP_UTILS_AHB2_DES },
	{ .addr_lo = 0x48021400, .addr_hi = 0x480217FF, .name = "HASH", .bus_des = BSP_UTILS_AHB2_DES },
	{ .addr_lo = 0x48021800, .addr_hi = 0x48021BFF, .name = "RNG", .bus_des = BSP_UTILS_AHB2_DES },
	{ .addr_lo = 0x48022400, .addr_hi = 0x480227FF, .name = "SDMMC2", .bus_des = BSP_UTILS_AHB2_DES },
	{ .addr_lo = 0x48022800, .addr_hi = 0x48022BFF, .name = "DLYB_SDMMC2", .bus_des = BSP_UTILS_AHB2_DES },
	{ .addr_lo = 0x50000000, .addr_hi = 0x50000FFF, .name = "RESERVED", .bus_des = BSP_UTILS_APB3_DES },
	{ .addr_lo = 0x50001000, .addr_hi = 0x50001FFF, .name = "LTDC", .bus_des = BSP_UTILS_APB3_DES },
	{ .addr_lo = 0x50003000, .addr_hi = 0x50003FFF, .name = "WWDG1", .bus_des = BSP_UTILS_APB3_DES },
	{ .addr_lo = 0x51000000, .addr_hi = 0x510FFFFF, .name = "GPV", .bus_des = BSP_UTILS_AHB3_DES },
	{ .addr_lo = 0x52000000, .addr_hi = 0x52000FFF, .name = "MDMA", .bus_des = BSP_UTILS_AHB3_DES },
	{ .addr_lo = 0x52001000, .addr_hi = 0x52001FFF, .name = "DMA2D", .bus_des = BSP_UTILS_AHB3_DES },
	{ .addr_lo = 0x52002000, .addr_hi = 0x52002FFF, .name = "Flash", .bus_des = BSP_UTILS_AHB3_DES },
	{ .addr_lo = 0x52003000, .addr_hi = 0x52003FFF, .name = "JPEG", .bus_des = BSP_UTILS_AHB3_DES },
	{ .addr_lo = 0x52004000, .addr_hi = 0x52004FFF, .name = "FMC", .bus_des = BSP_UTILS_AHB3_DES },
	{ .addr_lo = 0x52005000, .addr_hi = 0x52005FFF, .name = "QSPI", .bus_des = BSP_UTILS_AHB3_DES },
	{ .addr_lo = 0x52006000, .addr_hi = 0x52006FFF, .name = "DLYB_QSPI", .bus_des = BSP_UTILS_AHB3_DES },
	{ .addr_lo = 0x52007000, .addr_hi = 0x52007FFF, .name = "SDMMC1", .bus_des = BSP_UTILS_AHB3_DES },
	{ .addr_lo = 0x52008000, .addr_hi = 0x52008FFF, .name = "DLYB_SDMMC1", .bus_des = BSP_UTILS_AHB3_DES },
	{ .addr_lo = 0x58000000, .addr_hi = 0x580003FF, .name = "EXTI", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58000400, .addr_hi = 0x580007FF, .name = "SYSCFG", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58000C00, .addr_hi = 0x58000FFF, .name = "LPUART1", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58001400, .addr_hi = 0x580017FF, .name = "SPI6", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58001C00, .addr_hi = 0x58001FFF, .name = "I2C4", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58002400, .addr_hi = 0x580027FF, .name = "LPTIM2", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58002800, .addr_hi = 0x58002BFF, .name = "LPTIM3", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58002C00, .addr_hi = 0x58002FFF, .name = "LPTIM4", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58003000, .addr_hi = 0x580033FF, .name = "LPTIM5", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58003800, .addr_hi = 0x58003BFF, .name = "COMP1_COMP2", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58003C00, .addr_hi = 0x58003FFF, .name = "VREF", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58003C00, .addr_hi = 0x58003FFF, .name = "VREFBUF", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58004000, .addr_hi = 0x580043FF, .name = "RTC", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58004800, .addr_hi = 0x58004BFF, .name = "IWDG1", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58004C00, .addr_hi = 0x58004FFF, .name = "RESERVED", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58005400, .addr_hi = 0x580057FF, .name = "SAI4", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58006400, .addr_hi = 0x58006BFF, .name = "RESERVED", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58020000, .addr_hi = 0x580203FF, .name = "GPIOA", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58020400, .addr_hi = 0x580207FF, .name = "GPIOB", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58020800, .addr_hi = 0x58020BFF, .name = "GPIOC", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58020C00, .addr_hi = 0x58020FFF, .name = "GPIOD", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58021000, .addr_hi = 0x580213FF, .name = "GPIOE", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58021400, .addr_hi = 0x580217FF, .name = "GPIOF", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58021800, .addr_hi = 0x58021BFF, .name = "GPIOG", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58021C00, .addr_hi = 0x58021FFF, .name = "GPIOH", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58022000, .addr_hi = 0x580223FF, .name = "GPIOI", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58022400, .addr_hi = 0x580227FF, .name = "GPIOJ", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58022800, .addr_hi = 0x58022BFF, .name = "GPIOK", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58024400, .addr_hi = 0x580247FF, .name = "RCC", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58024800, .addr_hi = 0x58024BFF, .name = "PWR", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58024C00, .addr_hi = 0x58024FFF, .name = "CRC", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58025400, .addr_hi = 0x580257FF, .name = "BDMA", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58025800, .addr_hi = 0x58025BFF, .name = "DMAMUX2", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58026000, .addr_hi = 0x580263FF, .name = "ADC3", .bus_des = BSP_UTILS_APB4_DES },
	{ .addr_lo = 0x58026400, .addr_hi = 0x580267FF, .name = "HSEM", .bus_des = BSP_UTILS_APB4_DES },
};

/**
 * @brief Get the descriptor of the peripheral that specified by name or address.
 * @param des .addr_lo or .name is used to match the specified periphral.
 *            The address is prior to the name if both of them are set.
 *            The mechanism for matching peripheral descriptors is as follows: If 
 *            .addr_lo is set, it represents an address within the peripheral address
 *            range (inclusive) to be matched; If .name is set, it matches byperipheral name.
 *            If the desired peripheral is matched, the peripheral descriptor is returned 
 *            from this parameter. therefore, this pointer parameter must be pre-allocated.
 *            If no peripheral is matched, this parameter will leave untouched.
 * @return errno (-ENODEV or -EINVAL) or pointer to des. error - no match found; pointer - matched
 */
struct bsp_utils_periph_des *bsp_utils_get_periph(struct bsp_utils_periph_des *des)
{
	size_t i = 0;
	size_t array_size = ARRAY_SIZE(bsp_utils_periph_des);
	if (des->addr_lo) {
		while (bsp_utils_periph_des[i++].addr_lo <= des->addr_lo && i < array_size);
		i -= 2;
		if (bsp_utils_periph_des[i].addr_hi < des->addr_lo)
			return ERR_PTR(-ENODEV);
		memcpy(des, &(bsp_utils_periph_des[i]), sizeof(*des));
	} else if (des->name) {
		for (i = 0; i < array_size; i++) {
			if (!strncmp(bsp_utils_periph_des[i].name, des->name, 10))
				break;
		}
		if (i == ARRAY_SIZE(bsp_utils_periph_des))
			return ERR_PTR(-ENODEV);
		memcpy(des, &(bsp_utils_periph_des[i]), sizeof(*des));
	} else {
		return ERR_PTR(-EINVAL);
	}
	return des;
}

#define __BSP_UTILS_CLK_CASE(pheriph, state)                 \
	case pheriph##_BASE:                                 \
		if (state)                                   \
			__HAL_RCC_##pheriph##_CLK_ENABLE();  \
		else                                         \
			__HAL_RCC_##pheriph##_CLK_DISABLE(); \
		break
#define __BSP_UTILS_CLK_CASE2(base_name, pheriph, state)     \
	case base_name##_BASE:                               \
		if (state)                                   \
			__HAL_RCC_##pheriph##_CLK_ENABLE();  \
		else                                         \
			__HAL_RCC_##pheriph##_CLK_DISABLE(); \
		break
/**
 * @brief Enable or disable clock of the peripheral.
 * @param des .addr_lo or .name is used to match the specified periphral. see @ref bsp_utils_get_periph
 * @param state ENABLE or DISABLE
 * @return Returns 0 on success, otherwise returns a negative error code
 */
int bsp_utils_periph_clk(struct bsp_utils_periph_des *des, FunctionalState state)
{
	struct bsp_utils_periph_des *res = bsp_utils_get_periph(des);
	if (!IS_FUNCTIONAL_STATE(state))
		return -EINVAL;
	if (IS_ERR(res))
		return -ENODEV;

	switch (res->addr_lo) {
		__BSP_UTILS_CLK_CASE(MDMA, state);
		__BSP_UTILS_CLK_CASE(TIM2, state);
		__BSP_UTILS_CLK_CASE(TIM4, state);
		__BSP_UTILS_CLK_CASE(TIM5, state);
		__BSP_UTILS_CLK_CASE(TIM6, state);
		__BSP_UTILS_CLK_CASE(TIM7, state);
		__BSP_UTILS_CLK_CASE(TIM12, state);
		__BSP_UTILS_CLK_CASE(TIM13, state);
		__BSP_UTILS_CLK_CASE(TIM14, state);
		__BSP_UTILS_CLK_CASE(LPTIM1, state);
		__BSP_UTILS_CLK_CASE(SPI2, state);
		__BSP_UTILS_CLK_CASE(SPI3, state);
		__BSP_UTILS_CLK_CASE(SPDIFRX, state);
		__BSP_UTILS_CLK_CASE(USART2, state);
		__BSP_UTILS_CLK_CASE(USART3, state);
		__BSP_UTILS_CLK_CASE(UART4, state);
		__BSP_UTILS_CLK_CASE(UART5, state);
		__BSP_UTILS_CLK_CASE(I2C1, state);
		__BSP_UTILS_CLK_CASE(I2C2, state);
		__BSP_UTILS_CLK_CASE(I2C3, state);
		__BSP_UTILS_CLK_CASE(CEC, state);
		__BSP_UTILS_CLK_CASE2(DAC1, DAC12, state);
		__BSP_UTILS_CLK_CASE(UART7, state);
		__BSP_UTILS_CLK_CASE(UART8, state);
		__BSP_UTILS_CLK_CASE(CRS, state);
		__BSP_UTILS_CLK_CASE(SWPMI1, state);
		__BSP_UTILS_CLK_CASE(OPAMP, state);
		__BSP_UTILS_CLK_CASE(MDIOS, state);
		__BSP_UTILS_CLK_CASE2(FDCAN1, FDCAN, state);
		__BSP_UTILS_CLK_CASE2(FDCAN2, FDCAN, state);
		__BSP_UTILS_CLK_CASE(TIM1, state);
		__BSP_UTILS_CLK_CASE(TIM8, state);
		__BSP_UTILS_CLK_CASE(USART1, state);
		__BSP_UTILS_CLK_CASE(USART6, state);
		__BSP_UTILS_CLK_CASE(SPI1, state);
		__BSP_UTILS_CLK_CASE(SPI4, state);
		__BSP_UTILS_CLK_CASE(TIM15, state);
		__BSP_UTILS_CLK_CASE(TIM16, state);
		__BSP_UTILS_CLK_CASE(TIM17, state);
		__BSP_UTILS_CLK_CASE(SPI5, state);
		__BSP_UTILS_CLK_CASE(SAI1, state);
		__BSP_UTILS_CLK_CASE(SAI2, state);
		__BSP_UTILS_CLK_CASE(SAI3, state);
		__BSP_UTILS_CLK_CASE(DFSDM1, state);
		__BSP_UTILS_CLK_CASE(HRTIM1, state);
		__BSP_UTILS_CLK_CASE(DMA1, state);
		__BSP_UTILS_CLK_CASE(DMA2, state);
		__BSP_UTILS_CLK_CASE2(ADC1, ADC12, state);
		__BSP_UTILS_CLK_CASE2(ADC2, ADC12, state);
	case ETH_BASE:
		if (state) {
			__HAL_RCC_ETH1MAC_CLK_ENABLE();
			__HAL_RCC_ETH1RX_CLK_ENABLE();
			__HAL_RCC_ETH1TX_CLK_ENABLE();
		} else {
			__HAL_RCC_ETH1MAC_CLK_DISABLE();
			__HAL_RCC_ETH1RX_CLK_DISABLE();
			__HAL_RCC_ETH1TX_CLK_DISABLE();
		}
		break;
		__BSP_UTILS_CLK_CASE2(USB1_OTG_HS_PERIPH, USB1_OTG_HS, state);
		__BSP_UTILS_CLK_CASE2(USB2_OTG_FS_PERIPH, USB2_OTG_FS, state);
		__BSP_UTILS_CLK_CASE(DCMI, state);
		__BSP_UTILS_CLK_CASE(RNG, state);
		__BSP_UTILS_CLK_CASE(SDMMC2, state);
		__BSP_UTILS_CLK_CASE2(DLYB_SDMMC2, SDMMC2, state);
		__BSP_UTILS_CLK_CASE(LTDC, state);
		__BSP_UTILS_CLK_CASE(WWDG1, state);
		__BSP_UTILS_CLK_CASE(DMA2D, state);
		__BSP_UTILS_CLK_CASE2(JPGDEC, JPGDECEN, state);
		__BSP_UTILS_CLK_CASE2(FMC_R, FMC, state);
		__BSP_UTILS_CLK_CASE2(QSPI_R, QSPI, state);
		__BSP_UTILS_CLK_CASE2(DLYB_QSPI, QSPI, state);
		__BSP_UTILS_CLK_CASE(SDMMC1, state);
		__BSP_UTILS_CLK_CASE2(DLYB_SDMMC1, SDMMC1, state);
		__BSP_UTILS_CLK_CASE(SYSCFG, state);
		__BSP_UTILS_CLK_CASE(LPUART1, state);
		__BSP_UTILS_CLK_CASE(SPI6, state);
		__BSP_UTILS_CLK_CASE(I2C4, state);
		__BSP_UTILS_CLK_CASE(LPTIM2, state);
		__BSP_UTILS_CLK_CASE(LPTIM3, state);
		__BSP_UTILS_CLK_CASE(LPTIM4, state);
		__BSP_UTILS_CLK_CASE(LPTIM5, state);
		__BSP_UTILS_CLK_CASE(COMP12, state);
		__BSP_UTILS_CLK_CASE2(VREFBUF, VREF, state);
		__BSP_UTILS_CLK_CASE(RTC, state);
		__BSP_UTILS_CLK_CASE(SAI4, state);
		__BSP_UTILS_CLK_CASE(GPIOA, state);
		__BSP_UTILS_CLK_CASE(GPIOB, state);
		__BSP_UTILS_CLK_CASE(GPIOC, state);
		__BSP_UTILS_CLK_CASE(GPIOD, state);
		__BSP_UTILS_CLK_CASE(GPIOE, state);
		__BSP_UTILS_CLK_CASE(GPIOF, state);
		__BSP_UTILS_CLK_CASE(GPIOG, state);
		__BSP_UTILS_CLK_CASE(GPIOH, state);
		__BSP_UTILS_CLK_CASE(GPIOI, state);
		__BSP_UTILS_CLK_CASE(GPIOJ, state);
		__BSP_UTILS_CLK_CASE(GPIOK, state);
		__BSP_UTILS_CLK_CASE(CRC, state);
		__BSP_UTILS_CLK_CASE(BDMA, state);
		__BSP_UTILS_CLK_CASE(ADC3, state);
		__BSP_UTILS_CLK_CASE(HSEM, state);
	default:
		return -EINVAL;
	}

	return 0;
}

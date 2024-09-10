#include <bsp_utils.h>
#include <bsp_debug.h>
#include <stm32h7xx.h>

#define ROM_TB1_BASE		   (0x5C000000)
#define ROM_TB1_ENT_PRESENT(entry) ((entry) | 0x01)
#define ROM_TB1_ENT_ADDROFF(entry) ((entry) & 0xFFFFF000)
#define ROM_TB1_PREPARE_COMPONENT(name)                                                                            \
	do {                                                                                                       \
		if (likely(ROM_TB1_ENT_PRESENT(sys_rom_tb1->name)))                                                \
			name = (struct bsp_debug_##name *)(ROM_TB1_ENT_ADDROFF(sys_rom_tb1->name) + ROM_TB1_BASE); \
	} while (0)

#define ROM_TB2_ENT_PRESENT(entry) ROM_TB1_ENT_PRESENT(entry)
#define ROM_TB2_ENT_ADDROFF(entry) ROM_TB1_ENT_ADDROFF(entry)
#define ROM_TB2_PREPARE_COMPONENT(name)                                                             \
	do {                                                                                        \
		if (likely(ROM_TB2_ENT_PRESENT(sys_rom_tb2->name)))                                 \
			name = (struct bsp_debug_##name *)(ROM_TB2_ENT_ADDROFF(sys_rom_tb2->name) + \
							   (uint32_t)sys_rom_tb2);                  \
	} while (0)

struct bsp_debug_common {
	uint32_t MEMTYPE;
	uint32_t PIDR4;
	uint32_t reserved3[3];
	uint32_t PIDR0;
	uint32_t PIDR1;
	uint32_t PIDR2;
	uint32_t PIDR3;
	uint32_t CIDR0;
	uint32_t CIDR1;
	uint32_t CIDR2;
	uint32_t CIDR3;
};

struct bsp_debug_sys_rom_tb1 {
	uint32_t reserved1[2];
	uint32_t swo;
	uint32_t swtf;
	uint32_t tsg;
	uint32_t sys_rom_tb2;
	uint32_t top_of_tb;
	uint32_t reserved2[1004];
	struct bsp_debug_common common;
};

struct bsp_debug_sys_rom_tb2 {
	uint32_t cti;
	uint32_t reserved1;
	uint32_t cstf;
	uint32_t etf;
	uint32_t tpiu;
	uint32_t reserved2[3];
	uint32_t top_of_tb;
	uint32_t reserved3[1002];
	struct bsp_debug_common common;
};

struct bsp_debug_swo {
	struct bsp_debug_common common;
};

struct bsp_debug_swtf {
	uint32_t reserved3[1011];
	struct bsp_debug_common common;
};
struct bsp_debug_tsg {
	uint32_t cntcr;
	uint32_t cntsr;
	uint64_t cntcv;
	uint32_t reserved1[4];
	uint32_t cntfid0;
	uint32_t reserved2[1002];
	struct bsp_debug_common common;
};
struct bsp_debug_cti {
	uint32_t reserved3[1011];
	struct bsp_debug_common common;
};
struct bsp_debug_cstf {
	uint32_t reserved3[1011];
	struct bsp_debug_common common;
};
struct bsp_debug_etf {
	uint32_t reserved3[1011];
	struct bsp_debug_common common;
};
struct bsp_debug_tpiu {
	uint32_t reserved3[1011];
	struct bsp_debug_common common;
};

static struct bsp_debug_sys_rom_tb1 *sys_rom_tb1 = (struct bsp_debug_sys_rom_tb1 *)ROM_TB1_BASE;
static struct bsp_debug_sys_rom_tb2 *sys_rom_tb2;
static struct bsp_debug_swo *swo;
static struct bsp_debug_tsg *tsg;
static struct bsp_debug_swtf *swtf;
static struct bsp_debug_cti *cti;
static struct bsp_debug_cstf *cstf;
static struct bsp_debug_etf *etf;
static struct bsp_debug_tpiu *tpiu;

static inline void bsp_debug_itm_init(void);

/**
 * @brief  Initializes the debug ITM (Instrumentation Trace Macrocell) interface.
 * 
 * This function configures the GPIO for the ITM interface and enables the 
 * necessary registers for debugging via ITM. Specifically, it sets up the 
 * GPIO pin B3 for alternate function (trace) and then enables the ITM and 
 * related core debugging features.
 * 
 * @note  This function assumes that the necessary clocks for GPIOB and the 
 *        ITM peripheral are already enabled.
 */
static inline void bsp_debug_itm_init()
{
	GPIO_InitTypeDef gpio_init;
	gpio_init.Pin = GPIO_PIN_3;
	gpio_init.Mode = GPIO_MODE_AF_PP;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_init.Alternate = GPIO_AF0_TRACE;
	HAL_GPIO_Init(GPIOB, &gpio_init);

	CoreDebug->DEMCR |= CoreDebug_DHCSR_S_RETIRE_ST_Msk;
	ITM->TCR |= ITM_TCR_ITMENA_Msk;
	ITM->TER |= 1UL;
}

/**
 * @brief This function must be called before all the other functions in this file
 * @return int Returns 0 on success, otherwise returns a negative error code
 */
int bsp_debug_init()
{
	SET_BIT(SCB->SHCSR, SCB_SHCSR_USGFAULTENA_Msk);
	SET_BIT(SCB->SHCSR, SCB_SHCSR_BUSFAULTENA_Msk);
	SET_BIT(SCB->SHCSR, SCB_SHCSR_MEMFAULTENA_Msk);

	ROM_TB1_PREPARE_COMPONENT(tsg);
	ROM_TB1_PREPARE_COMPONENT(swtf);
	ROM_TB1_PREPARE_COMPONENT(swo);
	ROM_TB1_PREPARE_COMPONENT(sys_rom_tb2);

	if (sys_rom_tb2 != NULL) {
		ROM_TB2_PREPARE_COMPONENT(cti);
		ROM_TB2_PREPARE_COMPONENT(cstf);
		ROM_TB2_PREPARE_COMPONENT(etf);
		ROM_TB2_PREPARE_COMPONENT(tpiu);
	}

	if (swo) {
		bsp_debug_itm_init();
	}
	bsp_debug_tsg_start(0UL);
	return 0;
}

/**
 * @brief The TSG starts counting from a specified value when this function is called
 * @param value The count value that the TSG start from
 * @return int Returns 0 on success, otherwise returns a negative error code
 * @note The TSG frequency is fixed to HCLK4 frequency
 */
int bsp_debug_tsg_start(uint64_t value)
{
	// TSG_CNTCR = (uint32_t)0; // Stop
	// TSG_CNTFID0 = HAL_RCC_GetHCLKFreq(); // Gated from HCLK4
	// TSG_CNTCV = value;
	// TSG_CNTCR = (uint32_t)1; // Start
	if (!tsg)
		return -ENXIO;
	tsg->cntcr = (uint32_t)0; // Stop
	tsg->cntfid0 = HAL_RCC_GetHCLKFreq(); // Gated from HCLK4
	tsg->cntcv = value;
	tsg->cntcr = (uint32_t)1; // Start
	return 0;
}

/**
 * @brief Stop the TSG.
 * @return int Returns 0 on success, otherwise returns a negative error code
 */
int bsp_debug_tsg_stop()
{
	// TSG_CNTCR = (uint32_t)0; // Stop

	if (!tsg)
		return -ENXIO;

	tsg->cntcr = (uint32_t)0; // Stop
	return 0;
}

/**
 * @brief Get the 64-bit timestamp from TSG
 *        The TSG starts counting from a specified value when bsp_utils_start_trce_ts(value)
 * @return The 64-bit timestamp
 * @warning This function will NOT check whether TSG exists
 */
__attribute__((naked)) uint64_t bsp_debug_tsg_get()
{
	__ASM volatile("push {r4}\n"
		       "ldr r4, = #0x5C005008\n"
		       "ldrd r0, r1, [r4]\n"
		       "pop {r4}\n"
		       "bx lr\n");
}

#undef ROM_TB1_ENT_PRESENT
#undef ROM_TB1_ENT_ADDROFF
#undef ROM_TB1_PREPARE_COMPONENT
#undef ROM_TB2_ENT_PRESENT
#undef ROM_TB2_ENT_ADDROFF
#undef ROM_TB2_PREPARE_COMPONENT

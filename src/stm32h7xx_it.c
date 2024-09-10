/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32h7xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <bsp.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
	/* USER CODE BEGIN NonMaskableInt_IRQn 0 */

	/* USER CODE END NonMaskableInt_IRQn 0 */
	/* USER CODE BEGIN NonMaskableInt_IRQn 1 */
	while (1) {
	}
	/* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
	/* USER CODE BEGIN HardFault_IRQn 0 */
	asm volatile(" tst lr, #4                  \n" /* Check EXC_RETURN[2] */
		     " ite eq                            \n"
		     " mrseq r0, msp                     \n"
		     " mrsne r0, psp                     \n"
		     "b print_panic                      \n"
		     : /* no output */
		     : /* no input */
		     : "r0" /* clobber */
	);

	/* USER CODE END HardFault_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_HardFault_IRQn 0 */
		/* USER CODE END W1_HardFault_IRQn 0 */
	}
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
	/* USER CODE BEGIN MemoryManagement_IRQn 0 */

	/* USER CODE END MemoryManagement_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
		/* USER CODE END W1_MemoryManagement_IRQn 0 */
	}
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
	/* USER CODE BEGIN BusFault_IRQn 0 */
	asm volatile(" tst lr, #4                  \n" /* Check EXC_RETURN[2] */
		     " ite eq                            \n"
		     " mrseq r0, msp                     \n"
		     " mrsne r0, psp                     \n"
		     "b print_panic                      \n"
		     : /* no output */
		     : /* no input */
		     : "r0" /* clobber */
	);

	/* USER CODE END BusFault_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_BusFault_IRQn 0 */
		/* USER CODE END W1_BusFault_IRQn 0 */
	}
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
	/* USER CODE BEGIN UsageFault_IRQn 0 */
	asm volatile(" tst lr, #4                  \n" /* Check EXC_RETURN[2] */
		     " ite eq                            \n"
		     " mrseq r0, msp                     \n"
		     " mrsne r0, psp                     \n"
		     "b print_panic                      \n"
		     : /* no output */
		     : /* no input */
		     : "r0" /* clobber */
	);
	/* USER CODE END UsageFault_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_UsageFault_IRQn 0 */
		/* USER CODE END W1_UsageFault_IRQn 0 */
	}
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
	/* USER CODE BEGIN SVCall_IRQn 0 */

	/* USER CODE END SVCall_IRQn 0 */
	/* USER CODE BEGIN SVCall_IRQn 1 */

	/* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
	/* USER CODE BEGIN DebugMonitor_IRQn 0 */

	/* USER CODE END DebugMonitor_IRQn 0 */
	/* USER CODE BEGIN DebugMonitor_IRQn 1 */

	/* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
	/* USER CODE BEGIN PendSV_IRQn 0 */

	/* USER CODE END PendSV_IRQn 0 */
	/* USER CODE BEGIN PendSV_IRQn 1 */

	/* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
	/* USER CODE BEGIN SysTick_IRQn 0 */

	/* USER CODE END SysTick_IRQn 0 */
	HAL_IncTick();
	/* USER CODE BEGIN SysTick_IRQn 1 */

	/* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles DMA1 stream0 global interrupt.
 */
void DMA1_Stream0_IRQHandler(void)
{
	/* USER CODE BEGIN DMA1_Stream0_IRQn 0 */

	/* USER CODE END DMA1_Stream0_IRQn 0 */
	extern struct bsp_module bsp_tty_mod;
	HAL_DMA_IRQHandler(&((struct bsp_tty_des *)bsp_tty_mod.descriptor)->_hdma_uart_tx);
	/* USER CODE BEGIN DMA1_Stream0_IRQn 1 */

	/* USER CODE END DMA1_Stream0_IRQn 1 */
}

/**
 * @brief This function handles DMA1 stream1 global interrupt.
 */
void DMA1_Stream1_IRQHandler(void)
{
	/* USER CODE BEGIN DMA1_Stream1_IRQn 0 */

	/* USER CODE END DMA1_Stream1_IRQn 0 */
	extern struct bsp_module bsp_tty_mod;
	HAL_DMA_IRQHandler(&((struct bsp_tty_des *)bsp_tty_mod.descriptor)->_hdma_uart_rx);
	/* USER CODE BEGIN DMA1_Stream1_IRQn 1 */

	/* USER CODE END DMA1_Stream1_IRQn 1 */
}

/**
  * @brief This function handles LTDC global interrupt.
  */
void LTDC_IRQHandler(void)
{
	/* USER CODE BEGIN LTDC_IRQn 0 */
	LTDC_HandleTypeDef *hltdc = &((struct bsp_lcd_des *)bsp_lcd_mod.descriptor)->_hltdc;

	/* USER CODE END LTDC_IRQn 0 */
	HAL_LTDC_IRQHandler(hltdc);
	/* USER CODE BEGIN LTDC_IRQn 1 */

	/* USER CODE END LTDC_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void print_panic(uint32_t *hardfault_args)
{
	uint32_t stacked_r0 = hardfault_args[0];
	uint32_t stacked_r1 = hardfault_args[1];
	uint32_t stacked_r2 = hardfault_args[2];
	uint32_t stacked_r3 = hardfault_args[3];
	uint32_t stacked_r12 = hardfault_args[4];
	uint32_t stacked_lr = hardfault_args[5];
	uint32_t stacked_pc = hardfault_args[6];
	uint32_t stacked_psr = hardfault_args[7];

	printf("[panic_stack]:\n");
	printf("\tr0 = %08lx, r1 = %08lx, r2 = %08lx, r3 = %08lx\n", stacked_r0, stacked_r1, stacked_r2, stacked_r3);
	printf("\tr12 = %08lx, lr = %08lx, pc = %08lx, psr = %08lx\n", stacked_r12, stacked_lr, stacked_pc,
	       stacked_psr);

	printf("[panic_setting]:\n");
#define CCR_BIT_TO_STR(bit_mask) (READ_BIT(SCB->CCR, bit_mask) ? "enabled" : "disabled")
	printf("\t ccr - configuration and control register\n");
	printf("\t\tbp - branch prediction %s\n", CCR_BIT_TO_STR(SCB_CCR_BP_Msk));
	printf("\t\tic - l1 instruction cache %s\n", CCR_BIT_TO_STR(SCB_CCR_IC_Msk));
	printf("\t\tdc - l1 data cache %s\n", CCR_BIT_TO_STR(SCB_CCR_DC_Msk));
	printf("\t\tbfhfnmign - handlers running at priority -1 and -2 ignoring data bus faults caused by load and store instructions is %s\n",
	       CCR_BIT_TO_STR(SCB_CCR_BFHFNMIGN_Msk));
	printf("\t\tdiv_0_trp - trap divide by 0 %s\n", CCR_BIT_TO_STR(SCB_CCR_DIV_0_TRP_Msk));
	printf("\t\tunalign_trp - trap unaligned halfword and word accesses %s\n",
	       CCR_BIT_TO_STR(SCB_CCR_UNALIGN_TRP_Msk));
	printf("\t\tusersetmpend - unprivileged software access to the stir %s\n",
	       CCR_BIT_TO_STR(SCB_CCR_USERSETMPEND_Msk));
	printf("\t\tnonbasethrdena - %s\n",
	       READ_BIT(SCB->CCR, SCB_CCR_NONBASETHRDENA_Msk) ?
		       "processor can enter thread mode from any level under the control of an exc_return value\n" :
		       "processor can enter thread mode only when no exception is active\n");

	printf("[panic_status]:\n");
#undef CCR_BIT_TO_STR
#define CFSR_BIT(bit_mask)  (READ_BIT(SCB->CFSR, bit_mask))
#define ABFSR_BIT(bit_mask) (READ_BIT(SCB->ABFSR, bit_mask))

	if (SCB->CFSR & 0xffff0000) {
		printf("\tufsr - usagefault status register\n");
		if (CFSR_BIT(SCB_CFSR_DIVBYZERO_Msk))
			printf("\t\tdivbyzero - a divide instruction was executed where the denominator was zero\n"
			       "\t\t\tthe pc value stacked for the exception return points to the instruction that performed the divide by zero\n");
		if (CFSR_BIT(SCB_CFSR_UNALIGNED_Msk))
			printf("\t\tunaligned - an unaligned access operation occurred\n"
			       "\t\t\tenable trapping of unaligned accesses by setting the unalign_trp bit in the ccr\n"
			       "\t\t\tunaligned ldm, stm, ldrd, and strd instructions always fault irrespective of the setting of the unalign_trp bit\n");
		if (CFSR_BIT(SCB_CFSR_NOCP_Msk))
			printf("\t\tnocp - coprocessor instruction was issued but the coprocessor was disabled or not present\n");
		if (CFSR_BIT(SCB_CFSR_INVPC_Msk))
			printf("\t\tinvpc - load an illegal exc_return value to the pc as a result of an invalid context switch\n"
			       "\t\t\tthe pc value stacked for the exception return points to the instruction\n"
			       "\t\t\ta. invalid return due to corrupted stack pointer, link register (lr), or stack content\n"
			       "\t\t\tb. ici/it bit in psr invalid for an instruction\n");
		if (CFSR_BIT(SCB_CFSR_INVSTATE_Msk))
			printf("\t\tinvstate - execute an instruction make illegal use of the execution program status register (epsr)\n"
			       "\t\t\tthe pc value stacked for the exception return points to the instruction\n"
			       "\t\t\tloading a branch target address to pc with lsb=0\n"
			       "\t\t\tstacked psr corrupted during exception or interrupt handling\n"
			       "\t\t\tvector table contains a vector address with lsb=0\n");
		if (CFSR_BIT(SCB_CFSR_UNDEFINSTR_Msk))
			printf("\t\tundefinstr - an undefined instruction was executed\n"
			       "\t\t\tthe pc value stacked for the exception return points to the undefined instruction\n"
			       "\t\t\ta. use of instructions not supported in the device\n"
			       "\t\t\tb. bad or corrupted memory contents\n");
	}
	if (SCB->CFSR & 0x0000ff00) {
		printf("\tbfsr - busfault status register\n");
		if (CFSR_BIT(SCB_CFSR_BFARVALID_Msk)) {
			printf("\t\tbfarvalid - bus fault address register (bfar) holds the address which triggered the fault\n");
			printf("\t\t\tbfar = %08lx", *((volatile uint32_t *)(0xe000ed38)));
		}
		if (CFSR_BIT(SCB_CFSR_LSPERR_Msk))
			printf("\t\tlsperr - a bus fault occurred during floating-point lazy state preservation\n");
		if (CFSR_BIT(SCB_CFSR_STKERR_Msk))
			printf("\t\tstkerr - a bus fault occurred during exception entry\n"
			       "\t\t\tthe sp is adjusted but the values in the context area on the stack might be incorrect\n"
			       "\t\t\tthe processor does not write a fault address to the bfar\n"
			       "\t\t\ta. stack pointer is corrupted or not initialized\n"
			       "\t\t\tb. stack is reaching an undefined memory region\n");
		if (CFSR_BIT(SCB_CFSR_UNSTKERR_Msk))
			printf("\t\tunstkerr - a bus fault occurred trying to return from an exception\n"
			       "\t\t\tthe original return stack is still present\n"
			       "\t\t\tthe processor does not adjust the sp from the failing return\n"
			       "\t\t\tthe processor does not performed a new save\n"
			       "\t\t\tthe processor does not write a fault address to bfar\n");
		if (CFSR_BIT(SCB_CFSR_IMPRECISERR_Msk)) {
			printf("\t\timpreciserr - a data bus error has occurred\n"
			       "\t\t\tthe return address in the stack frame is not related to the instruction that caused the error\n"
			       "\t\t\tbfar is not touched by this fault\n");
			if (SCB->ABFSR) {
				printf("\t\t\tabfsr - auxiliary bus fault status register\n");
				if (ABFSR_BIT(SCB_ABFSR_AXIM_Msk)) {
					printf("\t\t\t\tasynchronous fault on axim interface\n");
					printf("\t\t\t\taximtype=%lx 0 - okay; 1 - exokay; 2 - slverr; 3 - decerr\n",
					       ABFSR_BIT(SCB_ABFSR_AXIMTYPE_Msk) >> SCB_ABFSR_AXIMTYPE_Pos);
				}
				if (ABFSR_BIT(SCB_ABFSR_EPPB_Msk))
					printf("\t\t\t\tasynchronous fault on eppb interface\n");
				if (ABFSR_BIT(SCB_ABFSR_AHBP_Msk))
					printf("\t\t\t\tasynchronous fault on ahbp interface\n");
				if (ABFSR_BIT(SCB_ABFSR_DTCM_Msk))
					printf("\t\t\t\tasynchronous fault on dtcm interface\n");
				if (ABFSR_BIT(SCB_ABFSR_ITCM_Msk))
					printf("\t\t\t\tasynchronous fault on itcm interface\n");
			}
		}
		if (CFSR_BIT(SCB_CFSR_PRECISERR_Msk))
			printf("\t\tpreciserr - a data bus error has occurred\n"
			       "\t\t\tpc value stacked to bfar for the exception return points to the instruction that caused the fault\n");
		if (CFSR_BIT(SCB_CFSR_IBUSERR_Msk))
			printf("\t\tibuserr - a busfault on an instruction prefetch has occurred\n"
			       "\t\t\tprocessor does not write a fault address to bfar\n"
			       "\t\t\ta. branch to invalid memory regions for example caused by incorrect function pointers\n"
			       "\t\t\tb. invalid return due to corrupted stack pointer or stack content\n"
			       "\t\t\tc. incorrect entry in the exception vector table\n");
	}
	if (SCB->CFSR & 0x000000ff) {
		printf("\tmmfsr - memmanage fault status register\n");
		if (CFSR_BIT(SCB_CFSR_MMARVALID_Msk)) {
			printf("\t\tmmarvalid - memmanage fault address register (mmfar) holds the address which triggered the memmanage fault\n");
			printf("\t\t\tmmfar = %08lx\n", *((volatile uint32_t *)(0xe000ed34)));
		}
		if (CFSR_BIT(SCB_CFSR_MLSPERR_Msk))
			printf("\t\tmlsperr - a memmanage fault occurred during floating-point lazy state preservation\n");
		if (CFSR_BIT(SCB_CFSR_MSTKERR_Msk))
			printf("\t\tmstkerr - a memmanage fault occurred during exception entry\n"
			       "\t\t\ta. stack pointer is corrupted or not initialized\n"
			       "\t\t\tb. stack is reaching a region not defined by the mpu as read/write memory\n");
		if (CFSR_BIT(SCB_CFSR_MUNSTKERR_Msk))
			printf("\t\tmunstkerr - a memmanage fault occurred trying to return from an exception\n"
			       "\t\t\ta. stack pointer is corrupted\n"
			       "\t\t\tb. mpu region for the stack changed during execution of the exception handler\n");
		if (CFSR_BIT(SCB_CFSR_DACCVIOL_Msk))
			printf("\t\tdaccviol - a load or store at a location that does not permit the operation\n"
			       "\t\t\tthe processor has loaded the mmfar with the address of the attempted access\n"
			       "\t\t\tthe pc value stacked for the exception return points to the faulting instruction\n");
		if (CFSR_BIT(SCB_CFSR_IACCVIOL_Msk))
			printf("\t\tiavccviol - execute an instruction triggered an mpu execute never (xn) fault even when the mpu is disabled or not present\n"
			       "\t\t\tthe pc value stacked for the exception return points to the faulting instruction\n"
			       "\t\t\tthe processor has not written a fault address to the mmfar\n"
			       "\t\t\ta. branch to regions that are not defined in the mpu or defined as non-executable\n"
			       "\t\t\tb. invalid return due to corrupted stack content\n"
			       "\t\t\tc. Incorrect entry in the exception vector table\n");
	}
#undef ABFSR_BIT
#undef CFSR_BIT

#define HFSR_BIT(bit_mask) (READ_BIT(SCB->HFSR, bit_mask))
	if (SCB->HFSR & 0xc0000002) {
		printf("\thfsr - hardfault status register\n");
		if (HFSR_BIT(SCB_HFSR_DEBUGEVT_Msk))
			printf("\t\tdebugevt - a debug event occurred while the debug subsystem was not enabled\n");
		if (HFSR_BIT(SCB_HFSR_FORCED_Msk))
			printf("\t\tforced - a configurable fault was escalated to a hardfault\n"
			       "\t\t\tmust read the other fault status registers to find the cause of the fault\n");
		if (HFSR_BIT(SCB_HFSR_VECTTBL_Msk))
			printf("\t\tvecttbl - a fault occurred because of an issue reading from an address in the vector table\n"
			       "\t\t\tthe pc value stacked for the exception return points to the instruction that was preempted by the exception\n");
	}
#undef HFSR_BIT

	while (1) {
	}
}
/* USER CODE END 1 */

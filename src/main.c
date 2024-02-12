#include <bsp.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
#define POINTERS 262144
	uint8_t **list = (uint8_t **)malloc(POINTERS * 4);
	while (1) {
		for (size_t i = 0; i < POINTERS; i++) {
			size_t s = abs(rand()) % 32 + 1;
			list[i] = (uint8_t *)malloc(s);
		}
		bsp_led_toggle(BSP_LED_BLUE);
		HAL_Delay(100);
		for (size_t i = 0; i < POINTERS; i++) {
			free(list[i]);
		}
	}
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	printf("assertion fail at %s:%lu\n", (char *)file, line);
	__disable_irq();
	while (1) {
	}
}
#endif /* USE_FULL_ASSERT */

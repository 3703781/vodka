#include <bsp.h>
#include <stdlib.h>
#include <stdio.h>

struct bsp_module mod = { .name = "MAIN", .version = "0.1" };
BSP_MODULE_DECLARE(mod);
/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	BSP_LOG_DECLARE();
	while (1) {
		BSP_LOG_TRC("%d\r\n", rand());
		BSP_LOG_DBG("%d\r\n", rand());
		BSP_LOG_INF("%d\r\n", rand());
		BSP_LOG_WRN("%d\r\n", rand());
		BSP_LOG_ERR("%d\r\n", rand());
		BSP_LOG_CRT("%d\r\n", rand());
		BSP_LOG_ALW("%d\r\n", rand());
		HAL_Delay(rand() % 2);
		// fflush(0);
	}

	return 0;
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

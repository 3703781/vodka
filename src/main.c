#include <bsp.h>
#include <stdlib.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h> 
#include <log_task.h>

static struct bsp_module mod = { .name = "MAIN", .version = "0.1" };
BSP_MODULE_DECLARE(mod);
struct main_cb {
	TaskHandle_t * log_task1_handle;
	TaskHandle_t * log_task2_handle;
	TaskHandle_t * log_task3_handle;
};

struct main_cb main_cb;

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	// xTaskCreate(log_task1, "log_task1", 1024 / sizeof(portSTACK_TYPE), NULL, 2, main_cb.log_task1_handle);
	xTaskCreate(log_task2, "log_task2", 1024, NULL, 2, main_cb.log_task2_handle);
	xTaskCreate(log_task3, "log_task3", 8192, NULL, 2, main_cb.log_task3_handle);
	vTaskStartScheduler();
	while (1);
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
	// printf("assertion fail at %s:%lu\n", (char *)file, line);
	__disable_irq();
	while (1) {
	}
}
#endif /* USE_FULL_ASSERT */

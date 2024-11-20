#include <log_task.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <math.h>
#include <bsp.h>

static struct bsp_module mod = { .name = "LOG_TASK", .version = "0.1" };
BSP_MODULE_DECLARE(mod);

void log_task1(void *arg)
{
	BSP_LOG_DECLARE();
	UNUSED(arg);

#define DIGITS 2800
	while (1) {
		uint64_t b = 0, c = 0, d = 0, e = 0;
		uint64_t *f = pvPortMalloc((DIGITS + 1) * sizeof(uint64_t));
		uint8_t *str = pvPortMalloc(DIGITS * 4 / 14);
		uint8_t *in = str;
		if (f == NULL || str == NULL)
			continue;
		for (int i = 0; i < DIGITS; i++) f[i] = 2000;
		f[DIGITS] = 0;
		for (c = DIGITS; c > 0; c -= 14) {
			d = 0;
			for (b = c; b > 0; b--) {
				d += f[b] * 10000;
				f[b] = d % (2 * b - 1);
				d /= (2 * b - 1);
				if (b > 1)
					d *= (b - 1);
			}
			*in++ = (e + d / 10000) / 1000 + 0x30;
			*in++ = (e + d / 10000) % 1000 / 100 + 0x30;
			*in++ = (e + d / 10000) % 100 / 10 + 0x30;
			*in++ = (e + d / 10000) % 10 + 0x30;
			e = d % 10000;
		}
		*in = 0;
		BSP_LOG_DBG("%s", str);
		vPortFree(f);
		vPortFree(str);
	}
}

void log_task2(void *arg)
{

	static TickType_t last_time;
	struct bsp_disk_des *disk_des = (struct bsp_disk_des *)bsp_module_find("DISK")->descriptor;
	BSP_LOG_DECLARE();

	UNUSED(arg);
	uint8_t *buf = malloc(512);
	uint8_t buf2[512] = { 0 };

	while (1) {
		strcpy(buf, "qwertyuiopaSDFGHJKL");
		disk_des->ops.write(disk_des, buf, 0, 1);
		disk_des->ops.read(disk_des, buf2, 0, 1);
		BSP_LOG_WRN("%s\r\n", buf2);
		xTaskDelayUntil(&last_time, pdMS_TO_TICKS(1000));
	}
}

void log_task3(void *arg)
{
	float b;
	BSP_LOG_DECLARE();
	while (1) {
		UNUSED(arg);
		for (int i = 0; i < 1000000; i++) b += sinf(log10f((float)rand() / 3.1234567f) + 1);
		if (b > 0) {
			BSP_LOG_DBG("%f", b);
			__NOP();
		} else {
			BSP_LOG_DBG("%f < 0", b);
			__NOP();
		}
	}
}
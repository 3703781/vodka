#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <sys/stat.h>
#include <bsp.h>

static uint32_t fd_to_itm_instance[] = { 1, 1, 1 };

int _write(int fd, char *ptr, int len)
{
	if (fd < 0 || ptr == NULL || len < 0) {
		errno = EBADF;
		return -1;
	}

	if (len == 0)
		return 0;

	// if (fd < LWIP_SOCKET_OFFSET) {
	// 	int uart_instance = fd_to_uart_instance[fd];

	// 	Linflexd_Uart_Ip_AsyncSend(uart_instance, (uint8 *)ptr, len);
	// 	while (Linflexd_Uart_Ip_GetTransmitStatus(uart_instance,
	// 						  NULL) ==
	// 	       LINFLEXD_UART_IP_STATUS_BUSY)
	// 		taskYIELD();

	// 	return len;
	// } else {
	// 	return (int)lwip_write(fd, (const void *)ptr, (size_t)len);
	// }
	for (size_t i = 0; i < len; i++) {
		ITM_SendChar(ptr[i], fd_to_itm_instance[fd]);
	}
	return len;
}

int _read(int fd, char *buf, int count)
{
	// if (fd < LWIP_SOCKET_OFFSET) {
	// 	int read = 0;
	// 	(void)fd;
	// 	(void)buf;
	// 	for (; count > 0; --count) {
	// 		read++;
	// 	}

	// 	return read;
	// } else {
	// 	return (int)lwip_read(fd, (void *)buf, (size_t)count);
	// }
	errno = EBADF;
	return -1;
}

void *_sbrk(ptrdiff_t incr)
{
	extern char _sheap_ext_sdram, _eheap_ext_sdram;
	char *heap_low = &_sheap_ext_sdram;
	char *heap_high = &_eheap_ext_sdram;

	static char *heap_current = NULL;

	if (heap_current == NULL)
		heap_current = heap_low;

	char *heap_current_old = heap_current;

	if (heap_current + incr > heap_high) {
		bsp_led_on(BSP_LED_YELLOW);
		errno = ENOMEM;
		return (caddr_t)-1;
	}
	heap_current += incr;
	return (caddr_t)heap_current_old;
}

void __malloc_lock(struct _reent *p)
{
	(void)p;
	// configASSERT(!xPortIsInsideInterrupt());
	// vTaskSuspendAll();
}

void __malloc_unlock(struct _reent *p)
{
	(void)p;
	// (void)xTaskResumeAll();
}

void __env_lock(void)
{
	// vTaskSuspendAll();
}

void __env_unlock(void)
{
	// (void)xTaskResumeAll();
}

int _close(int file)
{
	// if (file < LWIP_SOCKET_OFFSET) {
	// 	if (file)
	// 		(void)file;
	// 	return -1;
	// } else {
	// 	return lwip_close(file);
	// }
	errno = EBADF;
	return -1;
}

int _fstat(int file, struct stat *st)
{
	(void)file;
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	(void)file;
	return 1;
}

int _lseek(int file, int ptr, int dir)
{
	(void)file;
	(void)ptr;
	(void)dir;

	return 0;
}

void _exit(int status)
{
	(void)status;
	while (1)
		;
}

void _kill(int pid, int sig)
{
	(void)pid;
	(void)sig;
	return;
}

int _getpid(void)
{
	return -1;
}

void _init(void)
{
	bsp_init();
	return;
}

int _fcntl(int s, int cmd, int val)
{
	// return lwip_fcntl(s, cmd, val);
	return -1;
}

#include <stdio.h>

void *__tls_get_addr(size_t *got)
{
	void *p = NULL;
	// p = pvTaskGetThreadLocalStorageAddress(NULL, (BaseType_t)got[1]);

	// if (!p)
	// 	Siul2_Dio_Ip_WritePin(LED_RED_PORT, LED_RED_PIN, 0);

	return p;
}

void syscall_catch(void)
{
	__asm("BKPT #0\n\t");
}

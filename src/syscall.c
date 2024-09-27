#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <bsp.h>

static uint32_t fd_to_itm_instance[] = { 1, 1, 1 };

ssize_t _read(int fd, void *buf, size_t count)
{
	UNUSED(fd);
	UNUSED(buf);
	UNUSED(count);
	return -1;
}

off_t _lseek(int fd, off_t offset, int whence)
{
	(void)fd;
	(void)offset;
	(void)whence;

	return 0;
}

ssize_t _write(int fd, const void *buf, size_t count)
{
	static struct bsp_module *mod = NULL;
	if (fd < 0 || buf == NULL || count < 0) {
		errno = EBADF;
		return -1;
	}

	if (count == 0)
		return 0;

	for (size_t i = 0; i < count; i++) {
		ITM_SendChar(((char *)buf)[i], fd_to_itm_instance[fd]);
	}
	if (mod == NULL)
		mod = bsp_module_find("TTY1");

	if (mod && mod->state == BSP_MODULE_STATE_LIVE) {
		struct bsp_tty_des *des = (struct bsp_tty_des *)mod->descriptor;
		count = des->ops.write(des, buf, count);
	}
	return count;
}

int _open(const char *pathname, int flags, ...)
{
	UNUSED(pathname);
	UNUSED(flags);
	return -1;
}

int _close(int fd)
{
	UNUSED(fd);
	return -1;
}

int _kill(pid_t pid, int sig)
{
	UNUSED(pid);
	UNUSED(sig);
	return -1;
}

void _exit(int status)
{
	UNUSED(status);
	while (1);
}

pid_t _getpid(void)
{
	return -1;
}

void *_sbrk(intptr_t incr)
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
		return (void *)-1;
	}
	heap_current += incr;
	return (caddr_t)heap_current_old;
}

int _fstat(int fd, struct stat *statbuf)
{
	UNUSED(fd);
	UNUSED(statbuf);
	return -1;
}

int _stat(const char *restrict pathname, struct stat *restrict statbuf)
{
	UNUSED(pathname);
	UNUSED(statbuf);
	return -1;
}

int _link(const char *oldpath, const char *newpath)
{
	UNUSED(oldpath);
	UNUSED(newpath);
	return -1;
}

int _unlink(const char *pathname)
{
	UNUSED(pathname);
	return -1;
}

int _gettimeofday(struct timeval *restrict tv, struct timezone *_Nullable restrict tz)
{
	UNUSED(tv);
	UNUSED(tz);
	return -1;
}

clock_t _times(struct tms *buf)
{
	UNUSED(buf);
	return -1;
}

int _isatty(int fd)
{
	return (fd <= 2) ? 1 : 0;
}

int _system(const char *command)
{
	UNUSED(command);
	return -1;
}

int _rename(const char *oldpath, const char *newpath)
{
	UNUSED(oldpath);
	UNUSED(newpath);
	return -1;
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

void _init(void)
{
	bsp_init();
	return;
}

int _fcntl(int fd, int cmd, ...)
{
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

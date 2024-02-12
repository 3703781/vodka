#ifndef __BSP_UTILS_H
#define __BSP_UTILS_H

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/slist.h>

#define SET_VAL_IF_ZERO(var, val)      \
	do {                           \
		if ((var) == 0)        \
			(var) = (val); \
	} while (0)

#define SET_VAL_IF_NULL(var, val)      \
	do {                           \
		if ((var) == NULL)     \
			(var) = (val); \
	} while (0)

#define CALL_IF_NOT_NULL(func, ...)        \
	do {                               \
		if ((func) != NULL)        \
			func(__VA_ARGS__); \
	} while (0)

#define CALL_IF_NOT_NULL_RET_PTR(func, ...)        \
	({                                         \
		void *__res = NULL;                \
		if ((func) != NULL)                \
			__res = func(__VA_ARGS__); \
		__res;                             \
	})

#define MAX_ERRNO	4095

#define IS_ERR_VALUE(x) unlikely((x) >= (unsigned long)-MAX_ERRNO)

static inline void *__must_check ERR_PTR(long error)
{
	return (void *)error;
}

static inline long __must_check PTR_ERR(const void *ptr)
{
	return (long)ptr;
}

static inline long __must_check IS_ERR(const void *ptr)
{
	return IS_ERR_VALUE((unsigned long)ptr);
}

static inline long __must_check IS_ERR_OR_NULL(const void *ptr)
{
	return !ptr || IS_ERR_VALUE((unsigned long)ptr);
}
#endif

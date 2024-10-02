#ifndef __BSP_UTILS_H
#define __BSP_UTILS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/slist.h>
#include <stm32h7xx.h>
#include <zephyr/math/ilog2.h>

#define SET_VAL_IF_ZERO(var, val)      \
	do {                           \
		if ((var) == 0)        \
			(var) = (val); \
	} while (0)

#define SET_VAL_IF_NOT_ZERO(var, val)  \
	do {                           \
		if ((var) != 0)        \
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

#define CALL_IF_NOT_NULL_RET_INT(func, ...)        \
	({                                         \
		int __res = -1;                    \
		if ((func) != NULL)                \
			__res = func(__VA_ARGS__); \
		__res;                             \
	})

#define MAX_ERRNO	4095

#define IS_ERR_VALUE(x) unlikely((x) >= (unsigned long)-MAX_ERRNO)

/**
 * @brief __roundup_pow_of_two() - round up to nearest power of two
 * @param n value to round up
 */
static inline __attribute__((const)) uint32_t __roundup_pow_of_two(uint32_t n)
{
	return (uint32_t)1 << find_msb_set(n - 1);
}

/**
 * @brief roundup_pow_of_two - round the given value up to nearest power of two
 * 
 * round the given value up to the nearest power of two.
 * the result is undefined when n == 0;
 * this can be used to initialise global variables from constant data.
 * 
 * @param n value to round up
 */
#define roundup_pow_of_two(n) \
	(__builtin_constant_p(n) ? ((n == 1) ? 1 : (1UL << (ilog2((n) - 1) + 1))) : __roundup_pow_of_two(n))

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

struct bsp_utils_bus_des {
	const uint32_t addr_lo;
	const uint32_t addr_hi;
	const char *const name;
	const uint32_t (*get_freq)(void);
};

struct bsp_utils_periph_des {
	uint32_t addr_lo;
	const uint32_t addr_hi;
	const char *name;
	const struct bsp_utils_bus_des *const bus_des;
};

struct bsp_utils_periph_des *bsp_utils_get_periph(struct bsp_utils_periph_des *des);
int bsp_utils_periph_clk(struct bsp_utils_periph_des *des, FunctionalState state);
#endif

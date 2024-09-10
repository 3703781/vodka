#ifndef __BSP_DEBUG_H
#define __BSP_DEBUG_H

#include <stdint.h>

int bsp_debug_init(void);
int bsp_debug_tsg_start(uint64_t value);
int bsp_debug_tsg_stop(void);
uint64_t bsp_debug_tsg_get(void) __attribute__((naked));

#endif
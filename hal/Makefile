this_file := $(lastword $(MAKEFILE_LIST))

target := libhal.a

objs := stm32h7xx_hal_cortex.o \
stm32h7xx_hal_rcc.o \
stm32h7xx_hal_rcc_ex.o \
stm32h7xx_hal_flash.o \
stm32h7xx_hal_flash_ex.o \
stm32h7xx_hal_gpio.o \
stm32h7xx_hal_hsem.o \
stm32h7xx_hal_dma.o \
stm32h7xx_hal_dma_ex.o \
stm32h7xx_hal_mdma.o \
stm32h7xx_hal_pwr.o \
stm32h7xx_hal_pwr_ex.o \
stm32h7xx_hal.o \
stm32h7xx_hal_i2c.o \
stm32h7xx_hal_i2c_ex.o \
stm32h7xx_hal_exti.o \
stm32h7xx_hal_tim.o \
stm32h7xx_hal_tim_ex.o \
stm32h7xx_hal_sdram.o \
stm32h7xx_hal_ltdc.o \
stm32h7xx_hal_ltdc_ex.o \
stm32h7xx_ll_fmc.o


confs :=
dirs :=
softlink_froms :=
EXTRA_CFLAGS_SELF :=
EXTRA_CFLAGS_DOWN +=

include $(target_mk)

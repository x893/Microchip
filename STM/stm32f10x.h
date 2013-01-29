#ifndef __STM32F10X_H__
#define __STM32F10X_H__

#define USE_STDPERIPH_DRIVER
#include "HardwareProfile.h"
#include "ST/STM32F10x/stm32f10x.h"

// Hardware I/O pin mappings
#define GPIO_Out_PP_2M		(GPIO_Mode_Out_PP | GPIO_Speed_2MHz)
#define GPIO_Out_PP_50M		(GPIO_Mode_Out_PP | GPIO_Speed_50MHz)
#define GPIO_AF_PP_50M		(GPIO_Mode_AF_PP | GPIO_Speed_50MHz)
#define GPIO_In_Floating	GPIO_Mode_IN_FLOATING

#define GPIO_Mode_08_15(p, b, m)	p->CRH = ((p->CRH & ~(0xFul << ((b - 8) * 4))) | ((m & 0xFul) << ((b - 8) * 4)))
#define GPIO_Mode_00_07(p, b, m)	p->CRL = ((p->CRL & ~(0xFul << ((b    ) * 4))) | ((m & 0xFul) << ((b    ) * 4)))
#define BV(x)			(1 << x)

#endif

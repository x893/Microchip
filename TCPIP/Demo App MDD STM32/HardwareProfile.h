/*********************************************************************
 *
 *	Hardware specific definitions
 *
 *********************************************************************
 * FileName:        HardwareProfile.h
 * Dependencies:    None
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.10 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.34 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder		10/03/06	Original, copied from Compiler.h
 * Ken Hesky            07/01/08    Added ZG2100-specific features
 ********************************************************************/
#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H

#define __STM32F10X__
#define STM32F10X_MD_VL
#define STM32VLDISCOVERY
#define NDEBUG

#define USE_SD_INTERFACE_WITH_SPI

#include "GenericTypeDefs.h"
#include "Compiler.h"

// Clock frequency value.
// This value is used to calculate Tick Counter value

#define GetSystemClock()		(72000000ul)
#define GetInstructionClock()	(GetSystemClock() / 1)
#define GetPeripheralClock()	(GetSystemClock() / 8ul)

#define SoftReset()				NVIC_SystemReset()
#define SoftReset()				NVIC_SystemReset()

#define TICK_IRQ_HANDLER		SysTick_Handler
#define TICK_INIT()			do {																\
								SysTick->CTRL = 0;	/* Disable counter */						\
								NVIC_SetPriority (SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);	\
								SysTick->LOAD = 0xFFFFFF;										\
								SysTick->VAL  = 0;												\
								SysTick->CTRL = SysTick_CTRL_TICKINT | SysTick_CTRL_ENABLE;		\
							} while(0)

void __bsp_ENTER_CRITICAL_SECTION(void);
void __bsp_EXIT_CRITICAL_SECTION(void);

#define BSP_ENTER_CRITICAL_SECTION()	__bsp_ENTER_CRITICAL_SECTION()
#define BSP_EXIT_CRITICAL_SECTION()		__bsp_EXIT_CRITICAL_SECTION()

// Hardware mappings
#if defined(STM32VLDISCOVERY)

	#define LED0_PIN			8
	#define LED0_PORT			GPIOC
	#define LED0_BIT			(1 << LED0_PIN)
	#define LED0_IO				((LED0_PORT->ODR & LED0_BIT) ? 1 : 0)
	#define LED0_INIT()
	#define LED0_ON()
	#define LED0_OFF()
	#define LED0_TOGGLE()

	#define LED1_PIN			9
	#define LED1_PORT			GPIOC
	#define LED1_BIT			(1 << LED1_PIN)
	#define LED1_IO				((LED1_PORT->ODR & LED1_BIT) ? 1 : 0)
	#define LED1_INIT()
	#define LED1_ON()
	#define LED1_OFF()
	#define LED1_TOGGLE()

	#define	BUTTON0_IO			(1)
	#define	BUTTON1_IO			(1)
	#define	BUTTON2_IO			(1)
	#define	BUTTON3_IO			(1)

	#define WF_CS_TRIS
	#define WF_SPI_PORT			SPI1
	#define WF_SSPBUF			WF_SPI_PORT->DR
	#define WF_SPI_CLK_INIT()	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	#define WF_SPI_SCALE		SPI_BaudRatePrescaler_2

	#define WF_SPI_SCK_PIN		GPIO_PinSource5
	#define WF_SPI_SCK_PORT		GPIOA
	#define WF_SPI_MISO_PIN		GPIO_PinSource6
	#define WF_SPI_MISO_PORT	GPIOA
	#define WF_SPI_MOSI_PIN		GPIO_PinSource7
	#define WF_SPI_MOSI_PORT	GPIOA

	#define WF_SPI_PIN_INIT()	\
		do {					\
			GPIO_Mode_00_07 ( WF_SPI_SCK_PORT,  WF_SPI_SCK_PIN,  GPIO_AF_PP_50M );		\
			GPIO_Mode_00_07 ( WF_SPI_MISO_PORT, WF_SPI_MISO_PIN, GPIO_In_Floating );	\
			GPIO_Mode_00_07 ( WF_SPI_MOSI_PORT, WF_SPI_MOSI_PIN, GPIO_AF_PP_50M );		\
		} while (0)

	#define WF_CS_INIT()
	#define WF_CS_HIGH()
	#define WF_CS_LOW()
	#define WF_INT_EXTI			EXTI_Line7
	#define WF_INT_INIT()
	#define WF_INT_IE			(0)
	#define WF_INT_IE_ENABLE()
	#define WF_INT_IE_DISABLE()

	#define WF_SetCE_LOW()
	#define WF_SetCE_HIGH()
	#define WF_SetCE_N(x)		if (x == WF_LOW) WF_SetCE_LOW();  else WF_SetCE_HIGH()
	#define WF_SetRST_LOW()
	#define WF_SetRST_HIGH()
	#define WF_SetRST_N(x)		if (x == WF_LOW) WF_SetRST_LOW(); else WF_SetRST_HIGH()

	#define STACK_USE_UART_PORT		USART2
	#define STACK_USE_UART_INIT()	\
		do {						\
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	\
			GPIO_Mode_00_07(GPIOA, 3, GPIO_In_Floating);			\
			GPIO_Mode_00_07(GPIOA, 2, GPIO_AF_PP_50M);				\
		} while(0)
		
	#define DataRdyUART()			DataRdyUSART()
	#define putrsUART(a)			putrsUSART((const char*)a)
	#define ReadUART()				ReadUSART()
	#define putsUART(a)				putsUSART((char *)a)
	#define BusyUART()				BusyUSART()
	#define WriteUART(a)			WriteUSART(a)

#endif

/*********************************************************************/
/******************* Pin and Register Definitions ********************/
/*********************************************************************/

/* SD Card definitions: Change these to fit your application when using
   an SD-card-based physical layer                                   */

#ifdef USE_SD_INTERFACE_WITH_SPI

	// Description: SD-SPI Chip Select Output bit
	#define SD_CS_TRIS
	#define SD_CS_LOW()
	#define SD_CS_HIGH()
	#define SD_CS_INIT()
			
	// Description: SD-SPI Card Detect Input bit
	#define SD_CD				(1)
	#define SD_CD_INIT()
			
	// Description: SD-SPI Write Protect Check Input bit
	#define SD_WE               (1)
	#define SD_WE_INIT()
	
	// Registers for the SPI module you want to use
	#define SD_SPI				SPI1
	#define SD_SPI_INIT()
	#define SD_SPI_INIT_SLOW()

	// Description: The SPI Buffer
	#define SPIBUF              SD_SPI->DR
	// Description: The receive buffer full bit in the SPI status register
	#define SPISTAT_RBF			(SD_SPI->SR & SPI_SR_RXNE)

	// Will generate an error if the clock speed is too low to interface to the card
	#if (GetSystemClock() < 100000)
		#error "Clock speed must exceed 100 kHz"
	#endif
#endif

#endif

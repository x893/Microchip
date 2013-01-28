/*********************************************************************
 *
 *	Hardware specific definitions for the WiFi Comm Demo Board
 *
 *********************************************************************
 * FileName:        HardwareProfile.h
 * Dependencies:    None
 * Processor:       PIC32
 * Compiler:        Microchip C32 v1.10 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2012 Microchip Technology Inc.  All rights
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
 * Chris Smith		09/30/11	Original
 ********************************************************************/
#ifndef __HARDWARE_PROFILE_H
#define __HARDWARE_PROFILE_H

#define __STM32F10X__
#define STM32F10X_HD
#define USE_STDPERIPH_DRIVER
#define NDEBUG

#include "GenericTypeDefs.h"
#include "Compiler.h"

#define GetSystemClock()		(SystemCoreClock)	// Hz
#define GetInstructionClock()	(GetSystemClock()/1)
#define GetPeripheralClock()	(GetSystemClock() / 8ull)

// Hardware mappings
//----------------------------
// LED and Button I/O pins
//----------------------------

#define SD_CS_PIN		GPIO_PinSource7
#define SD_CS_PORT		GPIOB
#define SD_CS_HIGH()	SD_CS_PORT->BSRR = BV(SD_CS_PIN)
#define SD_CS_INIT()	GPIO_Mode_00_07(SD_CS_PORT, SD_CS_PIN, GPIO_Out_PP_50M)

#define BOARD_INIT()							\
		do {									\
			RCC_APB2PeriphClockCmd(				\
				RCC_APB2Periph_GPIOA	|		\
				RCC_APB2Periph_GPIOB	|		\
				RCC_APB2Periph_GPIOC	|		\
				RCC_APB2Periph_AFIO, ENABLE		\
			);									\
			SD_CS_HIGH();						\
			SD_CS_INIT();						\
			WF_CS_HIGH();						\
			WF_CS_INIT();						\
		} while (0)

#define SW0_IO_PIN		GPIO_PinSource9
#define SW0_IO_PORT		GPIOC
#define SW0_IO			((SW0_IO_PORT->IDR & BV(SW0_IO_PIN)) ? 0 : 1)
#define SW0_INIT()		GPIO_Mode_08_15(SW0_IO_PORT, SW0_IO_PIN, GPIO_In_Floating)
#define BUTTON0_IO		SW0_IO
#define BUTTON1_IO		(1)
#define BUTTON2_IO		(1)
#define BUTTON3_IO		(1)

#define LED0_PIN		GPIO_PinSource10
#define LED0_PORT		GPIOA
#define LED0_INIT()		LED0_ON(); GPIO_Mode_08_15(LED0_PORT, LED0_PIN, GPIO_Out_PP_2M)
#define LED0_OFF()		LED0_PORT->BRR  = BV(LED0_PIN)
#define LED0_ON()		LED0_PORT->BSRR = BV(LED0_PIN)
#define LED0_INV()		LED0_PORT->ODR ^= BV(LED0_PIN)
#define LED0_IO			((LED0_PORT->ODR & BV(LED0_PIN)) ? 1 : 0)
#define LEDS_OFF()		LED0_OFF()
#define LEDS_ON()		LED0_ON()

#define LED1_INV()
#define LED1_IO			(0)
#define LED2_INV()
#define LED2_IO			(0)

#define LED3_IO			(0)
#define LED4_IO			(0)
#define LED5_IO			(0)
#define LED6_IO			(0)
#define LED7_IO			(0)

//----------------------------
// MRF24WB0M WiFi I/O pins
//----------------------------
#define WF_CS_TRIS			1

#define WF_CS_PIN			GPIO_PinSource4
#define WF_CS_PORT			GPIOA
#define WF_CS_LOW()			WF_CS_PORT->BRR  = BV(WF_CS_PIN)
#define WF_CS_HIGH()		WF_CS_PORT->BSRR = BV(WF_CS_PIN)
#define WF_CS_INIT()		GPIO_Mode_00_07(WF_CS_PORT, WF_CS_PIN, GPIO_Out_PP_50M)

#define WF_SPI_SCK_PIN		GPIO_PinSource5
#define WF_SPI_SCK_PORT		GPIOA
#define WF_SPI_MISO_PIN		GPIO_PinSource6
#define WF_SPI_MISO_PORT	GPIOA
#define WF_SPI_MOSI_PIN		GPIO_PinSource7
#define WF_SPI_MOSI_PORT	GPIOA

#define WF_SPI_PORT			SPI1
#define WF_SSPBUF			WF_SPI_PORT->DR
#define WF_SPI_CLK_INIT()	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
#define WF_SPI_SCALE		SPI_BaudRatePrescaler_4

#define WF_SPI_PIN_INIT()	\
	GPIO_Mode_00_07 ( WF_SPI_SCK_PORT,  WF_SPI_SCK_PIN,  GPIO_AF_PP_50M );		\
	GPIO_Mode_00_07 ( WF_SPI_MISO_PORT, WF_SPI_MISO_PIN, GPIO_In_Floating );	\
	GPIO_Mode_00_07 ( WF_SPI_MOSI_PORT, WF_SPI_MOSI_PIN, GPIO_AF_PP_50M )

#define WF_INT_TRIS			1
#define WF_INT_PIN			GPIO_PinSource9
#define WF_INT_PORT			GPIOA
#define WF_INT_IF_PIN()		(WF_INT_PORT->IDR & BV(WF_INT_PIN))
#define WF_INT_EXTI			EXTI_Line9
#define WF_IRQ_HANDLER		EXTI9_5_IRQHandler
#define WF_INT_IF			(EXTI->PR & WF_INT_EXTI)
#define WF_INT_IF_CLEAR()	EXTI->PR = WF_INT_EXTI
#define WF_INT_IF_SET()		EXTI->SWIER |= WF_INT_EXTI; EXTI->SWIER &= ~WF_INT_EXTI
#define WF_INT_IE			(EXTI->IMR & WF_INT_EXTI)
#define WF_INT_INIT()		do {	\
								NVIC_InitTypeDef   NVIC_InitStructure;							\
								EXTI_InitTypeDef EXTI_InitStruct;								\
																								\
								RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);			\
								RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			\
								GPIO_Mode_08_15(WF_INT_PORT, WF_INT_PIN, GPIO_In_Floating);		\
																								\
								GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, WF_INT_PIN);			\
																								\
								EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;				\
								EXTI_InitStruct.EXTI_Line = WF_INT_EXTI;						\
								EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;			\
								EXTI_InitStruct.EXTI_LineCmd = ENABLE;							\
								EXTI_Init(&EXTI_InitStruct);									\
								WF_INT_IE_DISABLE();											\
								WF_INT_IF_CLEAR();												\
								/* Enable and set EXTI9_5 Interrupt to the lowest priority */	\
								NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;				\
								NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;	\
								NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;			\
								NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					\
								NVIC_Init(&NVIC_InitStructure);									\
							} while (0)
#define WF_INT_IE_ENABLE()	EXTI->IMR |= WF_INT_EXTI
#define WF_INT_IE_DISABLE()	EXTI->IMR &= ~WF_INT_EXTI

//!	#define WF_HIBERNATE_PIN
//!	#define WF_HIBERNATE_PORT
#define WF_SetCE_N(level)

#define WF_RESET_PIN
// #define WF_RESET_PORT
// #define WF_RESET_HIGH()		WF_RESET_PORT->BSRR = BV(WF_RESET_PIN)
// #define WF_RESET_LOW()		WF_RESET_PORT->BRR = BV(WF_RESET_PIN)
#define WF_SetRST_N(level)
// if (level == WF_HIGH) WF_RESET_HIGH(); else WF_RESET_LOW()

#define SoftReset()			NVIC_SystemReset()

#define TICK_IRQ_HANDLER	SysTick_Handler
#define TICK_INIT()		do {																\
							SysTick->CTRL = 0;	/* Disable counter */						\
							NVIC_SetPriority (SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);	\
							SysTick->LOAD = 0xFFFFFF;										\
							SysTick->VAL  = 0;												\
							SysTick->CTRL = SysTick_CTRL_TICKINT | SysTick_CTRL_ENABLE;		\
						} while(0)
void TimerStop(void);
void TimerStart(void);
#define TICK_STOP()		TimerStop()
#define TICK_START()	TimerStart()

void __bsp_ENTER_CRITICAL_SECTION(void);
void __bsp_EXIT_CRITICAL_SECTION(void);

#define BSP_ENTER_CRITICAL_SECTION()	__bsp_ENTER_CRITICAL_SECTION()
#define BSP_EXIT_CRITICAL_SECTION()		__bsp_EXIT_CRITICAL_SECTION()

//----------------------------
// UART to Telnet Mapping
//----------------------------
#define BusyUART()			BusyUSART()
#define WriteUART(a)		WriteUSART(a)
#define putsUART(a)			putsUSART((char *)a)
#define putrsUART(a)		putrsUSART((const char *)a)
#define DataRdyUART()		DataRdyUSART()
#define ReadUART()			ReadUSART()
/*
#define putcUART            putchar
#define putrsUART(a)        fputs((const char*)a,(FILE *)stdout)
#define putsUART(a)         fputs((const char*)a,(FILE *)stdout)
#define DataRdyUART()       TelnetInChars()
#define ReadUART()          TelnetGet()
*/

#define STACK_USE_UART_PORT	USART2
#define STACK_USE_UART_INIT()											\
	do {																\
		USART_InitTypeDef USART_InitStructure;							\
																		\
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);			\
		GPIO_Mode_00_07(GPIOA, 3, GPIO_In_Floating);					\
		GPIO_Mode_00_07(GPIOA, 2, GPIO_AF_PP_50M);						\
																		\
		USART_InitStructure.USART_BaudRate = 115200;					\
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;		\
		USART_InitStructure.USART_StopBits = USART_StopBits_1;			\
		USART_InitStructure.USART_Parity = USART_Parity_No;				\
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	\
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	\
		USART_Init(STACK_USE_UART_PORT, &USART_InitStructure);			\
		USART_Cmd(STACK_USE_UART_PORT, ENABLE);							\
	} while (0)

#endif

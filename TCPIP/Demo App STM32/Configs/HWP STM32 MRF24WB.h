#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H

#include "Compiler.h"

#define __STM32F10X__

// Clock frequency values
// These directly influence timed events using the Tick module.  They also are used for UART and SPI baud rate generation.
#define GetSystemClock()		(72000000ul)			// Hz
#define GetInstructionClock()	(GetSystemClock() / 1)	// Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32.  Might need changing if using Doze modes.
#define GetPeripheralClock()	(GetSystemClock() / 2)	// Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32.  Divisor may be different if using a PIC32 since it's configurable.

// LEDs
/*
#define LED0_PIN		GPIO_PinSource8
#define LED0_PORT		GPIOC
#define LED0_INIT()		GPIO_Mode_08_15(LED0_PORT, LED0_PIN, GPIO_Out_PP_2M)
#define LED0_OFF()		LED0_PORT->BRR  = BV(LED0_PIN)
#define LED0_ON()		LED0_PORT->BSRR = BV(LED0_PIN)
#define LED0_TOGGLE()	LED0_PORT->ODR ^= BV(LED0_PIN)
#define LED0_READ()		((LED0_PORT->ODR & BV(LED0_PIN)) ? 1 : 0)

#define LED1_PIN		GPIO_PinSource9
#define LED1_PORT		GPIOC
#define LED1_INIT()		GPIO_Mode_08_15(LED1_PORT, LED1_PIN, GPIO_Out_PP_2M)
#define LED1_OFF()		LED1_PORT->BRR  = BV(LED1_PIN)
#define LED1_ON()		LED1_PORT->BSRR = BV(LED1_PIN)
#define LED1_TOGGLE()	LED1_PORT->ODR ^= BV(LED1_PIN)
#define LED1_READ()		((LED1_PORT->ODR & BV(LED1_PIN)) ? 1 : 0)

#define LEDS_OFF()		LED0_PORT->BRR  = (BV(LED0_PIN) | BV(LED1_PIN))
#define LEDS_ON()		LED0_PORT->BSRR = (BV(LED0_PIN) | BV(LED1_PIN))
*/

#define LED0_INIT()
#define LED0_OFF()
#define LED0_ON()
#define LED0_TOGGLE()
#define LED0_READ()		0

#define LED1_INIT()
#define LED1_OFF()
#define LED1_ON()
#define LED1_TOGGLE()
#define LED1_READ()		0

#define LEDS_OFF()
#define LEDS_ON()

#define LEDM_PIN		GPIO_PinSource8
#define LEDM_PORT		GPIOC
#define LEDM_INIT()		GPIO_Mode_08_15(LED0_PORT, LED0_PIN, GPIO_Out_PP_2M)
#define LEDM_OFF()		LED0_PORT->BRR  = BV(LED0_PIN)
#define LEDM_ON()		LED0_PORT->BSRR = BV(LED0_PIN)
#define LEDM_TOGGLE()	LED0_PORT->ODR ^= BV(LED0_PIN)

// Momentary push buttons
#define BUTTON0_PIN		GPIO_PinSource0
#define BUTTON0_INIT()	GPIO_Mode_00_07(GPIOA, BUTTON0_PIN, GPIO_In_Floating)
#define BUTTON0_PRESS()	((GPIOA->IDR & BV(BUTTON0_PIN)) == 0)
#define BUTTON0_READ()	((GPIOA->IDR & BV(BUTTON0_PIN)) ? 0 : 1)

// UART configuration (not too important since we don't have a UART 
// connector attached normally, but needed to compile if the STACK_USE_UART 
// or STACK_USE_UART2TCP_BRIDGE features are enabled.
// #define UARTTX_TRIS		(TRISFbits.TRISF3)
// #define UARTRX_TRIS		(TRISFbits.TRISF2)
#define STACK_USE_UART_PORT		USART2
#define STACK_USE_UART_INIT()	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	\
								GPIO_Mode_00_07(GPIOA, 3, GPIO_In_Floating);	\
								GPIO_Mode_00_07(GPIOA, 2, GPIO_AF_PP_50M)

//----------------------------
// MRF24WB0M WiFi I/O pins
//----------------------------

#define WF_CS_TRIS			1
#define WF_CS_PIN			GPIO_PinSource4
#define WF_CS_PORT			GPIOA
#define WF_CS_INIT()		GPIO_Mode_00_07(WF_CS_PORT, WF_CS_PIN, GPIO_Out_PP_50M)
#define WF_CS_LOW()			GPIOA->BRR  = BV(WF_CS_PIN)
#define WF_CS_HIGH()		GPIOA->BSRR = BV(WF_CS_PIN)

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
#define WF_INT_PIN			GPIO_PinSource7
#define WF_INT_PORT			GPIOB
#define WF_INT_EXTI			EXTI_Line7
#define WF_IRQ_HANDLER		EXTI9_5_IRQHandler
#define WF_INT_INIT()		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	\
							GPIO_Mode_00_07(WF_INT_PORT, WF_INT_PIN, GPIO_In_Floating)

//!	#define WF_HIBERNATE_PIN
//!	#define WF_HIBERNATE_PORT
#define WF_SetCE_N(level)

#define WF_RESET_PIN		GPIO_PinSource7
#define WF_RESET_PORT		GPIOB
#define WF_RESET_HIGH()		WF_RESET_PORT->BSRR = BV(WF_RESET_PIN)
#define WF_RESET_LOW()		WF_RESET_PORT->BRR = BV(WF_RESET_PIN)
#define WF_SetRST_N(level)	if (level == WF_HIGH) WF_RESET_HIGH(); else WF_RESET_LOW()

#define SoftReset()			NVIC_SystemReset()
#define TICK_IRQ_HANDLER	SysTick_Handler
#define TICK_INIT()			SysTick_Config(SystemCoreClock / 1000)

void __bsp_ENTER_CRITICAL_SECTION(void);
void __bsp_EXIT_CRITICAL_SECTION(void);

#define BSP_ENTER_CRITICAL_SECTION()	__bsp_ENTER_CRITICAL_SECTION()
#define BSP_EXIT_CRITICAL_SECTION()		__bsp_EXIT_CRITICAL_SECTION()

// UART mapping functions for consistent API names across 8-bit and 16 or 
// 32 bit compilers.  For simplicity, everything will use "UART" instead 
// of USART/EUSART/etc.
#define BusyUART()			BusyUSART()
#define CloseUART()			CloseUSART()
#define ConfigIntUART(a)	ConfigIntUSART(a)
#define DataRdyUART()		DataRdyUSART()
#define OpenUART(a,b,c)		OpenUSART(a,b,c)
#define ReadUART()			ReadUSART()
#define WriteUART(a)		WriteUSART(a)
#define getsUART(a,b,c)		getsUSART(b,a)
#define putsUART(a)			putsUSART((char *)a)
#define getcUART()			ReadUSART()
#define putcUART(a)			WriteUSART(a)
#define putrsUART(a)		putrsUSART((const char *)a)

#endif // #ifndef HARDWARE_PROFILE_H

/********************************************************************
* File Description:
*
*  This file defines functions used for demo board hardware
*
* Change History:
*  Rev   Date         Author    Description
*  1.0   2/15/2009    yfy       Initial revision
*  2.0   4/15/2009    yfy       MiMAC and MiApp revision
********************************************************************/

#ifndef _HARDWARE_PROFILE_H
#define _HARDWARE_PROFILE_H

#ifndef __STM32F10X__
	#define __STM32F10X__
#endif

#ifdef STM32VLDISCOVERY
	#define STM32F10X_MD
#endif

#include "GenericTypeDefs.h"
#include "ConfigApp.h"
    
/*********************************************************************/
// Choose one of the demo board that support the demo out of the box
/*********************************************************************/

// there are three ways to use NVM to store data: External EPROM, Data EEPROM and 
// programming space, with following definitions:
//  #define USE_EXTERNAL_EEPROM
//  #define USE_DATA_EEPROM
//  #define USE_PROGRAMMING_SPACE
// Each demo board has defined the method of using NVM, as
// required by Network Freezer feature.

#ifdef STM32VLDISCOVERY
    
	#define CLOCK_FREQ      		72000000ul

	// Transceiver Configuration
	#if (defined(MRF24J40) || defined(MRF49XA))

        #define PHY_CS_PIN				GPIO_PinSource4
        #define PHY_CS_PORT				GPIOB
		#define PHY_CS_INIT()			GPIO_Mode_00_07 ( PHY_CS_PORT, PHY_CS_PIN, GPIO_Out_PP_50M )
		#define PHY_CS_HIGH()			(PHY_CS_PORT->BSRR = BV(PHY_CS_PIN))
		#define PHY_CS_LOW()			(PHY_CS_PORT->BRR  = BV(PHY_CS_PIN))
		#define PHY_CS					((PHY_CS_PORT->ODR & BV(PHY_CS_PIN)) ? 1 : 0)
		#define PHY_CS_SET(x)			if (x) { PHY_CS_HIGH(); } else { PHY_CS_LOW(); }
		
        #define PHY_RESETn_PIN			GPIO_PinSource5
        #define PHY_RESETn_PORT			GPIOB
		#define PHY_RESETn_INIT()		GPIO_Mode_00_07 ( PHY_CS_PORT, PHY_CS_PIN, GPIO_Out_PP_50M )
		#define PHY_RESETn_HIGH()		(PHY_CS_PORT->BSRR = BV(PHY_CS_PIN))
		#define PHY_RESETn_LOW()		(PHY_CS_PORT->BRR  = BV(PHY_CS_PIN))

	#endif

	#if defined(MRF49XA)

		#define nFSEL_PIN				GPIO_PinSource4
		#define nFSEL_PORT				GPIOA
		#define nFSEL_INIT()			GPIO_Mode_00_07 ( nFSEL_PORT, nFSEL_PIN, GPIO_Out_PP_50M )
		#define nFSEL_HIGH()			(nFSEL_PORT->BSRR = BV(nFSEL_PIN))
		#define nFSEL_LOW()				(nFSEL_PORT->BRR  = BV(nFSEL_PIN))
		#define nFSEL					((nFSEL_PORT->ODR & BV(nFSEL_PIN)) ? 1 : 0)
		#define nFSEL_SET(x)			if (x) { nFSEL_HIGH(); } else { nFSEL_LOW(); }

		#define FINT_PIN				GPIO_PinSource1
		#define FINT_PORT				GPIOA
		#define FINT_INIT()				GPIO_Mode_00_07 ( FINT_PORT, FINT_PIN, GPIO_Out_PP_50M )
		#define FINT_HIGH()				(FINT_PORT->BSRR = BV(FINT_PIN))
		#define FINT_LOW()				(FINT_PORT->BRR  = BV(FINT_PIN))
		#define FINT_EXTI_LINE			EXTI_Line1

		#define RFIF_PIN()				(FINT_PORT->IDR & BV(FINT_PIN))
		#define RFIF_SET()				EXTI->SWIER |= FINT_EXTI_LINE;	\
										EXTI->SWIER &= ~FINT_EXTI_LINE
		#define RFIE					((EXTI->IMR & FINT_EXTI_LINE) ? 1 : 0)
		#define RFIF_CLEAR()			(EXTI->PR  = FINT_EXTI_LINE)
		#define RFIE_ENABLE()			(EXTI->IMR |=  FINT_EXTI_LINE)
		#define RFIE_DISABLE()			(EXTI->IMR &= ~FINT_EXTI_LINE)
		#define RFIE_SET(x)				if (x) { RFIE_ENABLE(); } else { RFIE_DISABLE(); }

	#elif defined(MRF24J40)

		#define PHY_WAKE_PIN
		#define PHY_WAKE_PORT
		#define PHY_WAKE_HIGH()
		#define PHY_WAKE_INIT()

		#define RF_EXTI_LINE			EXTI_Line1

		#define RFIE					((EXTI->IMR & RF_EXTI_LINE) ? 1 : 0)
		#define RF_IRQ_HANDLER			EXTI1_Handler
		
		#define RFIF_SET()				EXTI->SWIER |= RF_EXTI_LINE;	\
										EXTI->SWIER &= ~RF_EXTI_LINE
		#define RFIF_PIN()				(0)
		#define RFIF					(EXTI->PR &= RF_EXTI_LINE)
		#define RFIF_CLEAR()			(EXTI->PR  = RF_EXTI_LINE)
		#define RFIE_ENABLE()			(EXTI->IMR |=  RF_EXTI_LINE)
		#define RFIE_DISABLE()			(EXTI->IMR &= ~RF_EXTI_LINE)
		#define RFIE_SET(x)				if (x) { RFIE_ENABLE(); } else { RFIE_DISABLE(); }
		
	#elif defined(MRF89XA)

		#define IRQ1_INT_PIN			GPIO_PinSource7
		#define IRQ1_INT_PORT			GPIOB

		#define PHY_IRQ1_PIN			IRQ1_INT_PIN
		#define PHY_IRQ1_PORT			IRQ1_INT_PORT
		#define PHY_IRQ1_EXTI_LINE		EXTI_Line7
		#define PHY_IRQ1_EXTI_PORT		GPIO_PortSourceGPIOB
		#define PHY_IRQ1_IRQn			EXTI9_5_IRQn
		#define PHY_IRQ1_IRQ_HANDLER	EXTI9_5_Handler
		
		#define PHY_IRQ1				((EXTI->PR  & PHY_IRQ1_EXTI_LINE) ? 1 : 0)
		#define PHY_IRQ1_En				((EXTI->IMR & PHY_IRQ1_EXTI_LINE) ? 1 : 0)

		#define PHY_IRQ1_En_SET(x)		if (x) { PHY_IRQ1_ENABLE(); } else { PHY_IRQ1_DISABLE(); }
		#define PHY_IRQ1_CLEAR()		(EXTI->PR  = PHY_IRQ1_EXTI_LINE)

		#define PHY_IRQ1_ENABLE()		(EXTI->IMR |=  PHY_IRQ1_EXTI_LINE)
		#define PHY_IRQ1_DISABLE()		(EXTI->IMR &= ~PHY_IRQ1_EXTI_LINE)

		#define RFIE_DISABLE()			PHY_IRQ1_DISABLE()
		#define RFIE_ENABLE()			PHY_IRQ1_ENABLE()
		#define RFIE_SET(x)				PHY_IRQ1_En_SET(x)

		#define RFIF_CLEAR()			PHY_IRQ1_CLEAR()
		#define RFIF_SET()				EXTI->SWIER |= PHY_IRQ1_EXTI_LINE;	\
										EXTI->SWIER &= ~PHY_IRQ1_EXTI_LINE
		#define RFIF_PIN()				(PHY_IRQ1_PORT->IDR & BV(PHY_IRQ1_PIN))

		#define PHY_IRQ0_PIN			GPIO_PinSource4
		#define PHY_IRQ0_PORT			GPIOC
		#define PHY_IRQ0_EXTI_LINE		EXTI_Line4
		#define PHY_IRQ0				((EXTI->PR  & PHY_IRQ0_EXTI_LINE) ? 1 : 0)
		#define PHY_IRQ0_En				((EXTI->IMR & PHY_IRQ0_EXTI_LINE) ? 1 : 0)
		#define PHY_IRQ0_CLEAR()		(EXTI->PR  = PHY_IRQ0_EXTI_LINE)
		#define PHY_IRQ0_IRQ_HANDLER	EXTI4_Handler

		#define Config_nCS_PIN			GPIO_PinSource4
		#define Config_nCS_PORT			GPIOA
		#define Config_nCS_INIT()		GPIO_Mode_00_07 ( Config_nCS_PORT, Config_nCS_PIN, GPIO_Out_PP_50M )
		#define Config_nCS_HIGH()		(Config_nCS_PORT->BSRR = BV(Config_nCS_PIN))
		#define Config_nCS_LOW()		(Config_nCS_PORT->BRR  = BV(Config_nCS_PIN))

		#define Data_nCS_PIN			GPIO_PinSource10
		#define Data_nCS_PORT			GPIOA
		#define Data_nCS_INIT()			GPIO_Mode_08_15 ( Data_nCS_PORT, Data_nCS_PIN, GPIO_Out_PP_50M )
		#define Data_nCS_HIGH()			(Data_nCS_PORT->BSRR = BV(Data_nCS_PIN))
		#define Data_nCS_LOW()			(Data_nCS_PORT->BRR  = BV(Data_nCS_PIN))

	#else
		#error "Radio module not define"
	#endif

	#define PHY_IRQ1_INIT()			\
		do {						\
			NVIC_InitTypeDef NVIC_InitStructure;								\
			EXTI_InitTypeDef EXTI_InitStruct;									\
			GPIO_Mode_00_07 ( PHY_IRQ1_PORT, PHY_IRQ1_PIN,  GPIO_In_Floating );	\
			\
			GPIO_EXTILineConfig(PHY_IRQ1_EXTI_PORT, PHY_IRQ1_PIN);			\
			\
			EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;				\
			EXTI_InitStruct.EXTI_Line = PHY_IRQ1_EXTI_LINE;					\
			EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;			\
			EXTI_InitStruct.EXTI_LineCmd = ENABLE;							\
			EXTI_Init(&EXTI_InitStruct);									\
			PHY_IRQ1_DISABLE();												\
			PHY_IRQ1_CLEAR();												\
			NVIC_InitStructure.NVIC_IRQChannel = PHY_IRQ1_IRQn;				\
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;	\
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;			\
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					\
			NVIC_Init(&NVIC_InitStructure);									\
		} while (0)

	#define USE_PROGRAMMING_SPACE
	//! #define EEPROM_SHARE_SPI

	#define SPI_SDI_PIN			GPIO_PinSource6
	#define SPI_SDI_PORT		GPIOA
	#define SPI_SDO_PIN			GPIO_PinSource7
	#define SPI_SDO_PORT		GPIOA
	#define SPI_SCK_PIN			GPIO_PinSource5
	#define SPI_SCK_PORT		GPIOA

	#define SPI_SDI				((SPI_SDI_PORT->IDR & BV(SPI_SDI_PIN)) ? 1 : 0)
	#define SPI_SCK_HIGH()		(SPI_SCK_PORT->BSRR = BV(SPI_SCK_PIN))
	#define SPI_SCK_LOW()		(SPI_SCK_PORT->BRR  = BV(SPI_SCK_PIN))
	#define SPI_SDO_HIGH()		(SPI_SDO_PORT->BSRR = BV(SPI_SDO_PIN))
	#define SPI_SDO_LOW()		(SPI_SDO_PORT->BRR  = BV(SPI_SDO_PIN))

	#define SPI_PORT			SPI1
	#define SPI_CLK()			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE)
	#define SPI_SCALE			SPI_BaudRatePrescaler_2

	#define SPI_INIT()		\
		do {				\
			SPI_InitTypeDef   SPI_InitStructure;								\
			GPIO_Mode_00_07 ( SPI_SCK_PORT, SPI_SCK_PIN, GPIO_AF_PP_50M );		\
			GPIO_Mode_00_07 ( SPI_SDI_PORT, SPI_SDI_PIN,  GPIO_In_Floating );	\
			GPIO_Mode_00_07 ( SPI_SDO_PORT, SPI_SDO_PIN,  GPIO_AF_PP_50M );		\
			SPI_CLK();															\
			\
			SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	\
			SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						\
			SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					\
			SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;							\
			SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;						\
			SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							\
			SPI_InitStructure.SPI_BaudRatePrescaler = SPI_SCALE;				\
			SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					\
			SPI_InitStructure.SPI_CRCPolynomial = 0;							\
			SPI_Init(SPI_PORT, &SPI_InitStructure);								\
			\
			SPI_Cmd(SPI_PORT, ENABLE);											\
		} while (0)

	#define getcSPI1()			SPI_PORT->DR
	#define putcSPI1(x)			\
		do {					\
			while (!(SPI_PORT->SR & SPI_SR_TXE));	\
			SPI1->DR = x;							\
			while (!(SPI_PORT->SR & SPI_SR_RXNE));	\
		} while (0)

	// Momentary push buttons
	#define BUTTON_1_PIN		GPIO_PinSource0
	#define BUTTON_1_PORT		GPIOA
	#define BUTTON_1_INIT()		GPIO_Mode_00_07(BUTTON_1_PORT, BUTTON_1_PIN, GPIO_In_Floating)
	#define BUTTON_1_READ()		(BUTTON_1_PORT->IDR & BV(BUTTON_1_PIN))
	#define BUTTON_2_INIT()
	#define BUTTON_2_READ()		(1)

	#define LED_1_PIN			GPIO_PinSource8
	#define LED_1_PORT			GPIOC
	#define LED_1_INIT()		GPIO_Mode_08_15(LED_1_PORT, LED_1_PIN, GPIO_Out_PP_2M)
	#define LED_1_OFF()			(LED_1_PORT->BRR  = BV(LED_1_PIN))
	#define LED_1_ON()			(LED_1_PORT->BSRR = BV(LED_1_PIN))

	#define LED_2_PIN			GPIO_PinSource9
	#define LED_2_PORT			GPIOC
	#define LED_2_INIT()		GPIO_Mode_08_15(LED_2_PORT, LED_2_PIN, GPIO_Out_PP_2M)
	#define LED_2_OFF()			(LED_2_PORT->BRR  = BV(LED_2_PIN))
	#define LED_2_ON()			(LED_2_PORT->BSRR = BV(LED_2_PIN))
	#define LED_2_TOGGLE()		(LED_2_PORT->ODR ^= BV(LED_2_PIN))
        
	// Define SUPPORT_TWO_SPI if external EEPROM use the second SPI
	// port alone, not sharing SPI port with the transceiver
	// #define SUPPORT_TWO_SPI

	// External EEPROM SPI chip select pin definition
	// #define EE_nCS_HIGH()
	// #define EE_nCS_LOW()
	// #define EE_nCS_INIT()		EE_nCS_HIGH()

	#define SYMBOL_TIMER_INIT()		\
		do {						\
			TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;			\
			\
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	\
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	\
			\
			TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (16ul * (SystemCoreClock / 1000000ul)) - 1;	\
			TIM_TimeBaseStructure.TIM_ClockDivision = 0;													\
			TIM_TimeBaseStructure.TIM_Period = 65535;														\
			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;										\
			TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);													\
			\
			TIM_TimeBaseStructure.TIM_Prescaler = 0;		\
			TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);	\
			\
			TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);	\
			TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);			\
			TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_External1);				\
			TIM_SelectInputTrigger(TIM3, TIM_TS_ITR1);						\
			\
			TIM_Cmd(TIM3, ENABLE);	\
			TICK_START();			\
		} while (0)

	void __bsp_ENTER_CRITICAL_SECTION(void);
	void __bsp_EXIT_CRITICAL_SECTION(void);

	#define BSP_ENTER_CRITICAL_SECTION()	__bsp_ENTER_CRITICAL_SECTION()
	#define BSP_EXIT_CRITICAL_SECTION()		__bsp_EXIT_CRITICAL_SECTION()

	#define TICK_STOP()				TIM_Cmd(TIM2, DISABLE);
	#define TICK_START()			TIM_Cmd(TIM2, ENABLE);
	#define TMRL					TIM2->CNT
	#define TMRH					TIM3->CNT

	#define CONSOLE_UART_PORT		USART2
	#define CONSOLE_UART_DATA		USART2->DR
	#define CONSOLE_UART_TX_RDY()	(USART2->SR & USART_SR_TXE)
	#define CONSOLE_UART_RX_RDY()	(USART2->SR & USART_SR_RXNE)
	#define CONSOLE_UART_CLK()		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE)

	#define CONSOLE_UART_INIT()		\
		do {						\
			USART_InitTypeDef USART_InitStructure;						\
			CONSOLE_UART_CLK();											\
			GPIO_Mode_00_07(GPIOA, 3, GPIO_In_Floating);				\
			GPIO_Mode_00_07(GPIOA, 2, GPIO_AF_PP_50M);					\
			USART_InitStructure.USART_BaudRate = 115200;				\
			USART_InitStructure.USART_WordLength = USART_WordLength_8b;	\
			USART_InitStructure.USART_StopBits = USART_StopBits_1;		\
			USART_InitStructure.USART_Parity = USART_Parity_No;			\
			USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	\
			USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					\
			USART_Init(CONSOLE_UART_PORT, &USART_InitStructure);							\
			USART_Cmd(CONSOLE_UART_PORT, ENABLE);											\
		} while (0)

#endif	// MAPLE_RET6

#endif

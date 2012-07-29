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

#define USE_STDPERIPH_DRIVER
#define STM32F10X_HD
#define STM32F10X

#include "GenericTypeDefs.h"
#include "ConfigApp.h"
    
/*********************************************************************/
// Choose one of the demo board that support the demo out of the box
/*********************************************************************/
#define MAPLE_RET6

// there are three ways to use NVM to store data: External EPROM, Data EEPROM and 
// programming space, with following definitions:
//  #define USE_EXTERNAL_EEPROM
//  #define USE_DATA_EEPROM
//  #define USE_PROGRAMMING_SPACE  
// Each demo board has defined the method of using NVM, as
// required by Network Freezer feature.

#if defined(MAPLE_RET6)
    
	#define CLOCK_FREQ      72000000

	#define RFIE_SET(x)				if (x) { RFIE_ENABLE(); } else { RFIE_DISABLE(); }
	#define RFIE_GET()				(1) /* Read request */
	#define RFIE_DISABLE()			/* Disable interrupt */
	#define RFIE_ENABLE()			/* Enable interrupt */

	#define RFIF_LOW()				/* Clear request flag */
	#define RFIF_DISABLE()
	#define RFIF_ENABLE()

	#define RF_INT_PIN				GPIO_PinSource7
	#define RF_INT_PORT				GPIOB
	#define RF_INT_INIT()			GPIO_Mode_00_07 ( RF_INT_PORT, RF_INT_PIN,  GPIO_In_Floating )
	#define RF_INT_READ()			(RF_INT_PORT->IDR & BV(RF_INT_PIN))

	#define USE_EXTERNAL_EEPROM

	// Transceiver Configuration
	#if (defined(MRF24J40) || defined(MRF49XA))
        #define PHY_CS              LATBbits.LATB2
        #define PHY_CS_TRIS         TRISBbits.TRISB2
        #define PHY_RESETn          LATGbits.LATG2
        #define PHY_RESETn_TRIS     TRISGbits.TRISG2
	#endif

	#if defined(MRF49XA)

		#define nFSEL_HIGH()
		#define nFSEL_LOW()
		#define nFSEL_INIT()
		#define FINT
		#define FINT_TRIS

	#elif defined(MRF24J40)

		#define PHY_WAKE
		#define PHY_WAKE_TRIS

	#elif defined(MRF89XA)

		#define PHY_IRQ1_PIN		GPIO_PinSource3
		#define PHY_IRQ1_PORT		GPIOC
		#define PHY_IRQ1			(1) /* Read request */
									// (PHY_IRQ1_PORT->IDR & BV(GPIO_PinSource6))

		#define PHY_IRQ1_En_SET(x)	if (x) { PHY_IRQ1_ENABLE(); } else { PHY_IRQ1_DISABLE(); }
		#define PHY_IRQ1_CLEAR()	/* Reset request */

		#define PHY_IRQ1_HIGH()
		#define PHY_IRQ1_ENABLE()
		#define PHY_IRQ1_DISABLE()
		#define PHY_IRQ1_INIT()		GPIO_Mode_00_07 ( PHY_IRQ1_PORT, PHY_IRQ1_PIN,  GPIO_In_Floating )

		#define PHY_IRQ1_En			(1) /* Read interrupt enable flasg */

		#define Config_nCS_INIT()
		#define Config_nCS_HIGH()
		#define Config_nCS_LOW()

		#define Data_nCS_HIGH()
		#define Data_nCS_LOW()
		#define Data_nCS_INIT()

	#else
		#error "Radio module not define"
	#endif

	#define SPI_SDI_PIN			GPIO_PinSource6
	#define SPI_SDI_PORT		GPIOA
	#define SPI_SDO_PIN			GPIO_PinSource7
	#define SPI_SDO_PORT		GPIOA
	#define SPI_SCK_PIN			GPIO_PinSource5
	#define SPI_SCK_PORT		GPIOA

	#define SPI_SDI_READ()		((SPI_SDI_PORT->IDR & BV(SPI_SDI_PIN)) ? 1 : 0)
	#define SPI_SCK_HIGH()		(SPI_SCK_PORT->BSRR = BV(SPI_SCK_PIN))
	#define SPI_SCK_LOW()		(SPI_SCK_PORT->BRR  = BV(SPI_SCK_PIN))
	#define SPI_SDO_HIGH()		(SPI_SDO_PORT->BSRR = BV(SPI_SDO_PIN))
	#define SPI_SDO_LOW()		(SPI_SDO_PORT->BRR  = BV(SPI_SDO_PIN))

	#define SPI_PINS_INIT()		\
		GPIO_Mode_00_07 ( SPI_SCK_PORT, SPI_SCK_PIN, GPIO_AF_PP_50M );		\
		GPIO_Mode_00_07 ( SPI_SDI_PORT, SPI_SDI_PIN,  GPIO_In_Floating );	\
		GPIO_Mode_00_07 ( SPI_SDO_PORT, SPI_SDO_PIN,  GPIO_AF_PP_50M )

	#define SPI_PORT			SPI1
	#define getcSPI1()			SPI1->DR
	#define putcSPI1(x)			SPI1->DR = x

	// Momentary push buttons
	#define BUTTON_1_PIN		GPIO_PinSource0
	#define BUTTON_1_PORT		GPIOA
	#define BUTTON_1_INIT()		GPIO_Mode_00_07(BUTTON_1_PORT, BUTTON_1_PIN, GPIO_In_Floating)
	#define BUTTON_1_READ()		(BUTTON_1_PORT->IDR & BV(BUTTON_1_PIN))
	#define BUTTON_2_INIT()
	#define BUTTON_2_READ()		(1)

	#define LED_1_PIN
	#define LED_1_PORT
	#define LED_1_INIT()
	#define LED_1_OFF()
	#define LED_1_ON()

	#define LED_2_PIN
	#define LED_2_PORT
	#define LED_2_INIT()
	#define LED_2_OFF()
	#define LED_2_ON()
	#define LED_2_TOGGLE()
        
	// Define SUPPORT_TWO_SPI if external EEPROM use the second SPI
	// port alone, not sharing SPI port with the transceiver
	// #define SUPPORT_TWO_SPI

	// External EEPROM SPI chip select pin definition
	#define EE_nCS_INIT()		EE_nCS_HIGH()
	#define EE_nCS_HIGH()
	#define EE_nCS_LOW()

	#define CONSOLE_UART_PORT		USART2
	#define CONSOLE_UART_DATA		USART2->DR
	#define CONSOLE_UART_TX_RDY()	(USART2->SR & USART_SR_TXE)
	#define CONSOLE_UART_RX_RDY()	(USART2->SR & USART_SR_RXNE)

	#define INT1_IRQ_HANDLER		EXTI0_Handler
	#define INT2_IRQ_HANDLER		EXTI1_Handler

	#define TMRL					0	/* Read timer counter */

#endif
#endif

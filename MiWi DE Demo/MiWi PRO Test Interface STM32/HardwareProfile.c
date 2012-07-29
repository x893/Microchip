/*****************************************************************************
 *
 *              Main.c -- user main program
 *
 *****************************************************************************
 * FileName:        HardwareProfile.c
 * Dependencies:
 * Processor:       STM32
 * Compiler:        ARM
 * Linker:
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * Copyright © 2007-2008 Microchip Technology Inc.  All rights reserved.
 *
 * Microchip licenses to you the right to use, modify, copy and distribute 
 * Software only when embedded on a Microchip microcontroller or digital 
 * signal controller and used with a Microchip radio frequency transceiver, 
 * which are integrated into your product or third party product (pursuant 
 * to the terms in the accompanying license agreement).   
 *
 * You should refer to the license agreement accompanying this Software for 
 * additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY 
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A 
 * PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE 
 * LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, 
 * CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY 
 * DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO 
 * ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, 
 * LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, 
 * TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT 
 * NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *****************************************************************************
 * File Description:
 *
 *   This file provides configuration and basic hardware functionality 
 *   based on chosen hardware demo boards.
 *
 * Change History:
 *  Rev   Date         Description
 *  0.1   2/17/2009    Initial revision
 *****************************************************************************/
#include "SystemProfile.h"
#include "Compiler.h"
#include "WirelessProtocols/Console.h"
#include "WirelessProtocols/LCDBlocking.h"
#include "TimeDelay.h"
#include "HardwareProfile.h"

#define PIC32MX_SPI1_SDO_SCK_MASK_VALUE		(0x00000140)
#define PIC32MX_SPI1_SDI_MASK_VALUE			(0x00000080)
#define PIC32MX_INT1_MASK_VALUE				(0x00000100)
/* MAX SPI CLOCK FREQ SUPPORTED FOR MIWI TRANSCIEVER */
#define MAX_SPI_CLK_FREQ_FOR_P2P			(2000000)

#define DEBOUNCE_TIME 0x00003FFF

BOOL PUSH_BUTTON_pressed;
MIWI_TICK PUSH_BUTTON_press_time;

/*********************************************************************
 * Function:        void BoardInit( void )
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Board is initialized for P2P usage
 *
 * Overview:        This function configures the board 
 *
 * Note:            This routine needs to be called before the function 
 *                  to initialize P2P stack or any other function that
 *                  operates on the stack
 ********************************************************************/

void BoardInit(void)
{
#if defined(MAPLE_RET6)

	// set I/O ports
	BUTTON_1_INIT();
	BUTTON_2_INIT();
	LED_1_INIT();
	LED_2_INIT();

#if defined(MRF24J40) || defined(MRF49XA)
	PHY_CS_HIGH();
	PHY_CS_INIT();
	PHY_RESETn_HIGH;
	PHY_RESETn_INIT();
#endif

	RF_INT_INIT();

#if defined(MRF49XA)
	nFSEL_INIT();
	FINT_INIT();
	nFSEL_HIGH();		// nFSEL inactive
#elif defined(MRF24J40)
	PHY_WAKE_INIT();
	PHY_WAKE_HIGH();
#elif defined(MRF89XA)	//MRF89XA
	Data_nCS_HIGH();
	Config_nCS_HIGH();
	Data_nCS_INIT();
	Config_nCS_INIT();
	PHY_IRQ1_HIGH();
#else
	#error "RF module not define"
#endif

	SPI_PINS_INIT();

	#warning "Enable interrupt with falling edge"

#if defined(ENABLE_NVM)

	EE_nCS_INIT();
	EE_nCS_HIGH();

#endif

	RFIF_DISABLE();

#if defined(MRF89XA)
	PHY_IRQ1_HIGH();
#endif

	if( RF_INT_READ() == 0 )
	{
		RFIF_ENABLE();
	}

	LCDInit();

#else
	#error "Unknown demo board.  Please properly initialize the part for the board."
#endif
}


/*********************************************************************
 * Function:        void LCDTRXCount(BYTE txCount, BYTE rxCount)
 *
 * PreCondition:    LCD has been initialized
 *
 * Input:           txCount - the total number of transmitted messages
 *                  rxCount - the total number of received messages
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function display the total numbers of TX and
 *                  RX messages on the LCD, if applicable.
 *
 * Note:            This routine is only effective if Explorer16 or
 *                  PIC18 Explorer demo boards are used
 ********************************************************************/
void LCDTRXCount(BYTE txCount, BYTE rxCount)
{
#if defined(SENSOR_PORT_LCD)
	LCDErase();
	sprintf((char *)LCDText, (const char*)"TX Messages: %d", txCount);
	sprintf((char *)&(LCDText[16]), (const char*)"RX Messages: %d", rxCount);
	LCDUpdate();    
#endif
}

/*********************************************************************
 * Function:        void LCDDisplay(char *text, BYTE value, BOOL delay)
 *
 * PreCondition:    LCD has been initialized
 *
 * Input:           text - text message to be displayed on LCD
 *                  value - the text message allows up to one byte 
 *                          of variable in the message
 *                  delay - whether need to display the message for
 *                          2 second without change
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function display the text message on the LCD, 
 *                  including up to one BYTE variable, if applicable.
 *
 * Note:            This routine is only effective if Explorer16 or
 *                  PIC18 Explorer demo boards are used
 ********************************************************************/
void LCDDisplay(char *text, BYTE value, BOOL delay)
{
#if defined(SENSOR_PORT_LCD)
	LCDErase();
	sprintf((char *)LCDText, (const char*)text, value);

	LCDUpdate();
        
	// display the message for 2 seconds
	if( delay )
	{
		BYTE i;
		for(i = 0; i < 8; i++)
		{
			DelayMs(250);
		}
	}
#endif
}

/*********************************************************************
 * Function:        BYTE ButtonPressed(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Byte to indicate which button has been pressed. 
 *                  Return 0 if no button pressed.
 *
 * Side Effects:    
 *
 * Overview:        This function check if any button has been pressed
 *
 * Note:            
 ********************************************************************/
BYTE ButtonPressed(void)
{
	MIWI_TICK tickDifference;

	if(BUTTON_1_READ() == 0)
	{
		//if the button was previously not pressed
		if(PUSH_BUTTON_pressed == FALSE)
		{
			PUSH_BUTTON_pressed = TRUE;
			PUSH_BUTTON_press_time = MiWi_TickGet();
			return 1;
		}
	}
	else if(BUTTON_2_READ() == 0)
	{
		//if the button was previously not pressed
		if(PUSH_BUTTON_pressed == FALSE)
		{
			PUSH_BUTTON_pressed = TRUE;
			PUSH_BUTTON_press_time = MiWi_TickGet();
			return 2;
		}
	} 
	else
	{
		//get the current time
		MIWI_TICK t = MiWi_TickGet();

		//if the button has been released long enough
		tickDifference.Val = MiWi_TickGetDiff(t,PUSH_BUTTON_press_time);

		//then we can mark it as not pressed
		if(tickDifference.Val > DEBOUNCE_TIME)
		{
			PUSH_BUTTON_pressed = FALSE;
		}
	}
	return 0;
}

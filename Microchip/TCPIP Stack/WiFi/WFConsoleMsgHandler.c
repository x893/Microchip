/******************************************************************************

 MRF24WB0M Driver Console Msg Handler
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24WB0M WiFi controller
  -Reference: MRF24WB0M Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:		WFConsoleMsgHandler.h
 Dependencies:	TCP/IP Stack header files
 Processor:		PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 Compiler:		Microchip C32 v1.10b or higher
				Microchip C30 v3.22 or higher
				Microchip C18 v3.34 or higher
 Company:		Microchip Technology, Inc.

 Software License Agreement

 Copyright (C) 2002-2010 Microchip Technology Inc.  All rights reserved.

 Microchip licenses to you the right to use, modify, copy, and distribute:
 (i)  the Software when embedded on a Microchip microcontroller or digital 
      signal controller product ("Device") which is integrated into 
      Licensee's product; or
 (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
      ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device used in 
	  conjunction with a Microchip ethernet controller for the sole purpose 
	  of interfacing with the ethernet controller.

 You should refer to the license agreement accompanying this Software for 
 additional information regarding your rights and obligations.

 THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 NON-INFRINGEMENT. IN NO EVENT SHALL MICROCHIP BE LIABLE FOR ANY INCIDENTAL,
 SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST
 OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS BY
 THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS
 FOR INDEMNITY OR CONTRIBUTION, OR OTHER SIMILAR COSTS, WHETHER ASSERTED ON
 THE BASIS OF CONTRACT, TORT (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR
 OTHERWISE.


 Author				Date		Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 KH                 27 Jan 2010 Updated for MRF24WB0M
******************************************************************************/


//---------
// Includes
//---------
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "TCPIP Stack/TCPIP.h"
#include "TCPIP Stack/WFConsole.h"

#if defined ( WF_CONSOLE )

#include "TCPIP Stack/WFConsoleMsgHandler.h"
#include "TCPIP Stack/WFConsoleIfconfig.h"
#include "TCPIP Stack/WFConsoleIwconfig.h"
#include "TCPIP Stack/WFConsoleIwpriv.h"

typedef struct dataStructDescriptor
{
    UINT16  dataFormat;
    void *  p_validateFunc;
    void *  p_dest;
} tDataStructDescriptor;


#define kWFValidateWithU8               (0)
#define kWFValidateWithU16              (1)
#define kWFValidateWithS8               (2)
#define kWFValidateWithX8               (3)

extern UINT8 g_hibernate_state;
//============================================================================
// Function Prototypes
//============================================================================

static void    do_help_msg(void);
static void    do_get_wfver_cmd(void);
static void    do_cls_cmd(void);

/*****************************************************************************
 * FUNCTION: process_cmd
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Determines which command has been received and processes it.
 *****************************************************************************/
void process_cmd(void)
{
    BOOL new_arg;
    UINT8 i;


    g_ConsoleContext.argc = 0;
    new_arg = TRUE;

    // Get pointers to each token in the command string
    TokenizeCmdLine(g_ConsoleContext.rxBuf);

    // if command line nothing but white kWFSpace or a linefeed
    if ( g_ConsoleContext.argc == 0u )
    {
        return;   // nothing to do
    }

    // change the command itself (token[0]) to lower case
    for (i = 0; i < strlen((char *)g_ConsoleContext.argv[0]); ++i)
    {
        g_ConsoleContext.argv[0][i] = tolower(g_ConsoleContext.argv[0][i]);
    }


    if ( IS_ECHO_ON() )
    {
        putrsUART("\n\r");
    }

    switch (GetCmdId())
    {

        case HELP_MSG:
            do_help_msg();
			WFConsoleSetMsgFlag();
            break;

        case GET_WF_VERSION_MSG:
            do_get_wfver_cmd();
            break;

        case RESET_HOST:
            Reset();
            break;

        case CLEAR_SCREEN_MSG:
            do_cls_cmd();
            break;

#if defined(WF_CONSOLE_IFCFGUTIL)
        case IFCONFIG_MSG:
            do_ifconfig_cmd();
            break;

        case IWCONFIG_MSG:
            do_iwconfig_cmd();
            break;

        case IWPRIV_MSG:
            do_iwpriv_cmd();
            break;
#endif // WF_CONSOLE_IFCFGUTIL

        default:
			WFConsoleSetMsgFlag();
            break;
    }
}

BOOL convertAsciiToHexInPlace( INT8 *p_string, UINT8 expectedHexBinSize )
{

    INT8  ascii_buffer[3];
    UINT8  hex_binary_index = 0;
    INT8  *hex_string_start = p_string;
    UINT16 hex_buffer = 0;

    /* gobble up any hex prefix */
    if ( memcmppgm2ram (hex_string_start, (const ROM FAR char*) "0x", 2) == 0 )
         hex_string_start+=2;

   if ( strlen( (char *) hex_string_start) != (expectedHexBinSize*2) )
      return FALSE;

    while ( hex_binary_index < expectedHexBinSize )
    {

      memcpy ( ascii_buffer, (const char*) hex_string_start, 2 );
      ascii_buffer[2] = '\0';

      /* convert the hex string to a machine hex value */
      if ( !ConvertASCIIHexToBinary( ascii_buffer,&hex_buffer) )
        return FALSE;

      p_string[hex_binary_index++] = (UINT8) hex_buffer;

      hex_string_start +=2;

    }

    return TRUE;

}

static void do_cls_cmd(void)
{
    Output_Monitor_Hdr();
}


static void do_help_msg(void)
{
    UINT8 i;

    putrsUART("\n\r");
    for (i = 0; i < g_numCmds; ++i)
    {
        putrsUART( (ROM FAR char *) g_consoleCmd[i].p_cmdName);
        putrsUART("\r\t\t");
        putrsUART( (ROM FAR char*) g_consoleCmd[i].p_cmdHelp);
        putrsUART("\n\r");
    }

}

/*****************************************************************************
 * FUNCTION: do_get_wfver_cmd
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Processes get WF device information
 *****************************************************************************/

static void do_get_wfver_cmd(void)
{
 	tWFDeviceInfo  deviceInfo;

	if (g_hibernate_state)
	{
		WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode - command failed.", TRUE);
		return;
	}

	WF_GetDeviceInfo(&deviceInfo);
	WFConsolePrintRomStr("Firmware version   0x", FALSE);
	WFConsolePrintHex(deviceInfo.romVersion, 2);
	WFConsolePrintHex(deviceInfo.patchVersion, 2);
	WFConsolePrintRomStr("", TRUE);  

	WFConsolePrintRomStr("Host Driver version        ", FALSE);
	WFConsolePrintRomStr(WF_HOST_DRIVER_VERSION_NUMBER, TRUE);
}

#endif /* WF_CONSOLE */










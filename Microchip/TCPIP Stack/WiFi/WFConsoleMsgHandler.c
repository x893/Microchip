/******************************************************************************

 MRF24W Driver Console Msg Handler
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

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
 KH                 27 Jan 2010 Updated for MRF24W
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

extern tWFHibernate WF_hibernate;

//============================================================================
// Function Prototypes
//============================================================================

static void    do_help_msg(void);
static void    do_get_wfver_cmd(void);
static void    do_cls_cmd(void);

#if defined(MRF24WG)  
    static void    do_wps_pin_cmd(void);
    static void    do_wps_push_button_cmd(void);
    static void    do_wps_get_credentials_cmd(void);
#endif   

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
    //!!!BOOL new_arg;
    UINT8 i;


    g_ConsoleContext.argc = 0;
    //!!!new_arg = TRUE;

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

#if defined(MRF24WG)
        case WPS_PIN_MSG:
            do_wps_pin_cmd();
            break;
            
        case WPS_PUSHBUTTON_MSG:
            do_wps_push_button_cmd();
            break;
            
        case WPS_GET_CREDENTIALS_MSG:
            do_wps_get_credentials_cmd();
            break;
            
#endif /* MRF24WG */


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
    if ( memcmppgm2ram (hex_string_start, (ROM FAR char*) "0x", 2) == 0 )
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
	if (WF_hibernate.state)
	{
		WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode - command failed.", TRUE);
		return;
	}
	WF_GetDeviceInfo(&deviceInfo);
	
	#if defined(MRF24WG)
	    WFConsolePrintRomStr("MRF24WG firmware version: 0x", FALSE);

	#else
	    WFConsolePrintRomStr("MRF24WB firmware version:     0x", FALSE);	
	#endif

	WFConsolePrintHex(deviceInfo.romVersion, 2);
	WFConsolePrintHex(deviceInfo.patchVersion, 2);
	WFConsolePrintRomStr("", TRUE);  

	WFConsolePrintRomStr("Host Driver version:      ", FALSE);
	WFConsolePrintRomStr(WF_HOST_DRIVER_VERSION_NUMBER, TRUE);
}

#if defined(MRF24WG)
static void do_wps_pin_cmd(void)
{
    UINT8 pinLength;
    UINT8 pin[8];   // presume pin won't be greater than 8 digits
    UINT8 i;
    UINT8 pinChar;
    
      
    if (ARGC == 1)
    {
        WFConsolePrintRomStr("Missing PIN parameter", TRUE);
        return;
    }    
    
    if (ARGC > 2)
    {
        WFConsolePrintRomStr("Too many parameters", TRUE);
        return;
    }  
    
    pinLength = strlen((char *)ARGV[1]);   
    
    
    memset(pin, 0x00, sizeof(pin));
    for (i = 0; i < pinLength; ++i)
    {
        pinChar = ARGV[1][i];
        
        if ((pinChar < '0') || (pinChar > '9'))
        {
            WFConsolePrintRomStr("PIN must be all digits", TRUE);
            return;
        }    

        pin[i] = pinChar - '0';  // convert pin digit from ASCII to binary
        
 
    }    
    
    WF_CPSetSecurity(1, WF_SECURITY_WPS_PIN, 0, pin, pinLength);
    
}    

static void do_wps_push_button_cmd(void)
{
    if (ARGC > 1)
    {
        WFConsolePrintRomStr("Too many parameters", TRUE);
        return;
    }  
    
    WF_CPSetSecurity(1, WF_SECURITY_WPS_PUSH_BUTTON, 0, NULL, 0);

} 

static void do_wps_get_credentials_cmd(void)
{
    tWFWpsCred cred;
    int i;
    char buf[6];
    
    WF_CPGetWPSCredentials(1, &cred);


    if (cred.ssidLen > 0)
    {
        if (cred.ssidLen > 32)
        {
            putrsUART("SSID length is greater than 32, probably bad credential data\r\n");
            return;
        }
            
        putrsUART("SSID: ");
        for (i = 0; i < cred.ssidLen; ++i) 
        {
            sprintf(buf, "%c", cred.ssid[i]);        
            putsUART(buf);
        }    
        putrsUART("'\r\n");
    } 
    
    putrsUART("Net Key:\r\n  ");
    for (i = 0; i < sizeof(cred.netKey); ++i)
    {
        if ( ((i % 16) == 0) && ( i != 0) )
        {
            putrsUART("\r\n  ");
        }    
        
        sprintf(buf, "%02X ", cred.netKey[i]);
        putsUART(buf);
    }  
    putrsUART("\r\n"); 
    
    putrsUART("Auth Type: ");
    sprintf(buf, "%d\r\n", cred.authType);
    putsUART(buf);
    
    putrsUART("Enc Type: ");
    sprintf(buf, "%d\r\n", cred.encType);
    putsUART(buf);
    
    putrsUART("Net ID: ");
    sprintf(buf, "%d\r\n", cred.netIdx);
    putsUART(buf);
    
    putrsUART("Key ID: ");
    sprintf(buf, "%d\r\n", cred.keyIdx); 
    putsUART(buf);    
    
    putrsUART("BSSID: ");
    for (i = 0; i < 6; ++i)
    {
        sprintf(buf, "%02X ", cred.bssid[i]);
        putsUART(buf);
    }    
    putrsUART("\r\n");
    
    
} 

#if 0
	UINT8 ssid[32];
	UINT8 netKey[64];
	UINT16 authType;
	UINT16 encType;
	UINT8 netIdx;
	UINT8 ssidLen;
	UINT8 keyIdx;
	UINT8 keyLen;
	UINT8 bssid[6];   
#endif	

#endif /* MRF24WG */

#endif /* WF_CONSOLE */










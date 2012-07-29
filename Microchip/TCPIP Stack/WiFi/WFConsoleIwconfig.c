/******************************************************************************

 MRF24WB0M Driver iwconfig
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24WB0M WiFi controller
  -Reference: MRF24WB0M Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:		WFConsoleIwconfig.c
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


//============================================================================
//                                  Includes
//============================================================================
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "TCPIP Stack/TCPIP.h"
#include "TCPIP Stack/WFConsole.h"

#if defined( WF_CONSOLE_IFCFGUTIL )

#include "TCPIP Stack/WFConsoleIwconfig.h"
#include "TCPIP Stack/WFConsoleMsgs.h"
#include "TCPIP Stack/WFConsoleMsgHandler.h"

#if defined( EZ_CONFIG_SCAN )
#include "TCPIP Stack/WFEasyConfig.h"
#endif /* EZ_CONFIG_SCAN */



//============================================================================
//                                  Constants
//============================================================================

//============================================================================
//                                  Globals
//============================================================================
extern UINT8 ConnectionProfileID;

#if defined( WF_HOST_SCAN )
extern BOOL gHostScanNotAllowed;
UINT8 hostScanProfileID;
#endif

static BOOL		iwconfigCbInitialized = FALSE;
tWFIwconfigCb	iwconfigCb;


//============================================================================
//                                  Local Function Prototypes
//============================================================================
static void iwconfigDisplayStatus(void);
static BOOL iwconfigSetSsid(void);
static BOOL iwconfigSetMode(void);
static BOOL iwconfigSetChannel(void);
static BOOL iwconfigSetPower(void);
static BOOL iwconfigSetDomain(void);
static BOOL iwconfigSetRTS(void);
static BOOL iwconfigSetTxRate(void);
extern UINT16 WFStartScan(void);

UINT8 g_hibernate_state = WF_HB_NO_SLEEP;
UINT8 g_wakeup_notice = FALSE;

/*****************************************************************************
 * FUNCTION: do_iwconfig_cmd
 *
 * RETURNS: None
 *
 * PARAMS:    None
 *
 * NOTES:   Responds to the user invoking iwconfig
 *****************************************************************************/
void do_iwconfig_cmd(void)
{

	if (!g_hibernate_state && !iwconfigSetCb() )
		return;

    // if user only typed in iwconfig with no other parameters
    if (ARGC == 1u)
    {
		if (!g_hibernate_state)
			iwconfigDisplayStatus();
		else
			WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode - command failed.", TRUE);
		return;
    }

	if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "wakeup") == 0) )
    {
		if (!g_wakeup_notice)
    		g_wakeup_notice = TRUE;
		WFConsolePrintRomStr("The Wi-Fi module is awake.", TRUE);	
		return;
	}

	if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "hibernate") == 0) )
    {
		if (!g_hibernate_state)
		{
    		g_hibernate_state = WF_HB_ENTER_SLEEP;
			g_wakeup_notice = FALSE;
			WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode.", TRUE);
		}
		else
			WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode.", TRUE);
		return;
	}
	
	if (g_hibernate_state)
	{
		WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode - command failed.", TRUE);
		return;
	}

	if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "ssid") == 0) )
    {
    	if (!g_hibernate_state && !iwconfigSetSsid())
			return;
	}
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "mode") == 0) )
    {
    	if (!g_hibernate_state && !iwconfigSetMode())
			return;
	}
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "channel") == 0) )
    {
    	if (!g_hibernate_state && !iwconfigSetChannel())
			return;
	}
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "power") == 0) )
    {
    	if (!g_hibernate_state && !iwconfigSetPower())
			return;
	}
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "domain") == 0) )
    {
    	if (!g_hibernate_state && !iwconfigSetDomain())
			return;
	}
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "rts") == 0) )
    {
    	if (!g_hibernate_state && !iwconfigSetRTS())
			return;
	}
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "txrate") == 0) )
    {
    	if (!g_hibernate_state && !iwconfigSetTxRate())
			return;
	}

	#if (defined(EZ_CONFIG_SCAN) && !defined(WF_HOST_SCAN)) && !defined(__18CXX)
	else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "scan") == 0) )
    {
    	if (!g_hibernate_state) {
			WFConsolePrintRomStr("Scan...", TRUE);
        	if (WFStartScan() == WF_SUCCESS)
	    	{
				WFConsolePrintRomStr("Scan completed.", TRUE);
            	SCAN_SET_DISPLAY(SCANCXT.scanState);
            	SCANCXT.displayIdx = 0;
        	}
			else
				WFConsolePrintRomStr("Scan failed.", TRUE);	
    	}
	    return;
	}
	#endif /* EZ_CONFIG_SCAN */


	#if (defined(EZ_CONFIG_SCAN) && defined(WF_HOST_SCAN)) && !defined(__18CXX)
	else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "scan") == 0) )
    {
    	if (!g_hibernate_state) {
			
			if (gHostScanNotAllowed)
					WFConsolePrintRomStr("Scan not allowed.", TRUE);
			else {
				gHostScanNotAllowed = TRUE;
				WFInitScan();
				WFConsolePrintRomStr("Scanning...", TRUE);
				if ((3u <= ARGC) && strcmppgm2ram((char*)ARGV[2], "all") == 0)
					hostScanProfileID = WF_SCAN_ALL;
				else
					hostScanProfileID = ConnectionProfileID;		// Default - current profile
       			if (WFStartScan() == WF_SUCCESS){
					WFConsolePrintRomStr("Scan completed.", TRUE);
				}
				else {
					WFConsolePrintRomStr("Scan failed.", TRUE);
					gHostScanNotAllowed = FALSE;
				}
			}
		}
		else 
			WFConsolePrintRomStr("In hibernate mode - scan is not allowed.", TRUE);	
	    return;
	}
	else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "scanresults") == 0) )
    {
 	   	if (IS_SCAN_IN_PROGRESS(SCANCXT.scanState) || gHostScanNotAllowed)
			WFConsolePrintRomStr("Scanning is still in process...try again later.", TRUE);
		else {
			gHostScanNotAllowed = TRUE;
			SCAN_SET_DISPLAY(SCANCXT.scanState);
        	SCANCXT.displayIdx = 0;
			if (SCANCXT.numScanResults > 0) {
				while (IS_SCAN_STATE_DISPLAY(SCANCXT.scanState))
				{
					WFDisplayScanMgr();
				}
			}
			else {
				gHostScanNotAllowed = FALSE;
				WFConsolePrintRomStr("Nothing to display ... please run \"scan\" first", TRUE);
			}
		}
		return;
	}
	#endif /* WF_HOST_SCAN */

    else
    {
		WFConsolePrintRomStr("Unknown parameter", TRUE);
		return;
	}
}


/*****************************************************************************
 * FUNCTION: iwconfigSetCb
 *
 * RETURNS: TRUE or FALSE
 *
 * PARAMS:  None
 *
 * NOTES:   Set the iwconfigCb structure
 *****************************************************************************/
BOOL iwconfigSetCb(void)
{
	UINT8 cpId, newCpId;

	if ( !iwconfigCbInitialized ) // first time call of iwconfigSetCb
	{
		memset(&iwconfigCb, 0, sizeof(iwconfigCb));
		iwconfigCbInitialized = TRUE;
	}

	WF_GetPowerSaveState(&iwconfigCb.powerSaveState);
	if (iwconfigCb.powerSaveState == WF_PS_HIBERNATE)
	{
		WFConsolePrintRomStr("WF device hibernated", TRUE);
		return FALSE;
	}

	WF_CMGetConnectionState(&iwconfigCb.connState, &cpId);

	if ( iwconfigCb.cpId == WF_CURRENT_CPID_NONE )
	{
		if ( cpId == WF_CURRENT_CPID_NONE )
		{
			WF_CPCreate(&newCpId);
			iwconfigCb.cpId = newCpId;
		}
		else if ( cpId == WF_CURRENT_CPID_LIST )
		{
			WFConsolePrintRomStr("Conection profile list not supported", TRUE);
			return FALSE;
		}
		else
		{
			iwconfigCb.cpId = cpId; // use the application-created profile
		}
	}
	else // WF_MIN_NUM_CPID <= iwconfigCb.cpId && iwconfigCb.cpId <= WF_MAX_NUM_CPID
	{
		if ( cpId == WF_CURRENT_CPID_NONE )
		{
			// continue to use iwconfigCb.cpId
		}
		else if ( cpId == WF_CURRENT_CPID_LIST )
		{
			WFConsolePrintRomStr("Conection profile list not supported", TRUE);

			WF_CPDelete(iwconfigCb.cpId);
			iwconfigCb.cpId = WF_CURRENT_CPID_NONE;

			return FALSE;
		}
		else if ( cpId != iwconfigCb.cpId )
		{
			WF_CPDelete(iwconfigCb.cpId);
			iwconfigCb.cpId = cpId; // use the application-created profile
		}
		else // cpId == iwconfigCb.cpId
		{
			// contine to use iwconfigCb.cpId
		}
	}

    if ((iwconfigCb.connState == WF_CSTATE_NOT_CONNECTED) || (iwconfigCb.connState == WF_CSTATE_CONNECTION_PERMANENTLY_LOST))
    {
        iwconfigCb.isIdle = TRUE;
    }    
    else
    {
        iwconfigCb.isIdle = FALSE;
    }    

	return TRUE;
}

/*****************************************************************************
 * FUNCTION: iwconfigDisplayStatus
 *
 * RETURNS:	None
 *
 * PARAMS:	None
 *
 * NOTES:	Responds to the user invoking iwconfig with no parameters
 *****************************************************************************/
static void iwconfigDisplayStatus(void)
{
	UINT8 *p;
	UINT8 tmp;
	UINT8 connectionState;
	UINT8 cpId;

	union
	{
		struct
		{
			UINT8 List[WF_CHANNEL_LIST_LENGTH];
			UINT8 Num;
		} Channel;

		UINT8 Domain;

		struct
		{
			UINT8 String[WF_MAX_SSID_LENGTH+1];
			UINT8 Len;
		} Ssid;

		struct
		{
			UINT8 NetworkType;
		} Mode;

		struct
		{
			UINT16 Threshold;
		} Rts;
	} ws; // workspace

	// cpId
	{
		WFConsolePrintRomStr("\tcpid:     ", FALSE);
		WFConsolePrintInteger(iwconfigCb.cpId, 'd');
		WFConsolePrintRomStr("", TRUE);
	}

	// channel
	{
		WF_CAGetChannelList(ws.Channel.List, &ws.Channel.Num);
		WFConsolePrintRomStr("\tchannel:  ", FALSE);

		p = ws.Channel.List;
		tmp = ws.Channel.Num;

		while ( --tmp > 0u )
		{
			WFConsolePrintInteger(*p, 'd');
			WFConsolePrintRomStr(",", FALSE);
			p++;
        }

		WFConsolePrintInteger(*p, 'd');
		WFConsolePrintRomStr("", TRUE);
	}

	// domain
	{
		WF_GetRegionalDomain(&ws.Domain);

		WFConsolePrintRomStr("\tdomain:   ", FALSE);

		if ( ws.Domain == WF_DOMAIN_FCC )
		{
			WFConsolePrintRomStr("fcc", TRUE);
		}
		else if ( ws.Domain == WF_DOMAIN_IC )
		{
			WFConsolePrintRomStr("ic", TRUE);
		}
		else if ( ws.Domain == WF_DOMAIN_ETSI )
		{
			WFConsolePrintRomStr("etsi", TRUE);
		}
		else if ( ws.Domain == WF_DOMAIN_SPAIN )
		{
			WFConsolePrintRomStr("spain", TRUE);
		}
		else if ( ws.Domain == WF_DOMAIN_FRANCE )
		{
			WFConsolePrintRomStr("france", TRUE);
		}
		else if ( ws.Domain == WF_DOMAIN_JAPAN_A )
		{
			WFConsolePrintRomStr("japana", TRUE);
		}
		else if ( ws.Domain == WF_DOMAIN_JAPAN_B )
		{
			WFConsolePrintRomStr("japanb", TRUE);
		}
		else
		{
			WFConsolePrintRomStr("unknown", TRUE);
		}
	}

	// rts
	{
		WF_GetRtsThreshold(&ws.Rts.Threshold);

		WFConsolePrintRomStr("\trts:      ", FALSE);
		WFConsolePrintInteger(ws.Rts.Threshold, 'd');
		WFConsolePrintRomStr("", TRUE);
	}

	// mode
	{

        WF_CMGetConnectionState(&connectionState, &cpId);
        WF_CPGetNetworkType(iwconfigCb.cpId, &ws.Mode.NetworkType);
        
		WFConsolePrintRomStr("\tmode:     ", FALSE);

		if (iwconfigCb.isIdle)
		{
    		if (iwconfigCb.connState == WF_CSTATE_NOT_CONNECTED)
    		{
		    	WFConsolePrintRomStr("idle", TRUE);
		    }
		    else if (iwconfigCb.connState == WF_CSTATE_CONNECTION_PERMANENTLY_LOST)
		    {
		    	WFConsolePrintRomStr("idle (connection permanently lost)", TRUE);    		    
    		}   
    		else
    		{
        		WFConsolePrintRomStr("idle (?)", TRUE);    		    
            }  		 	
		}
		else
		{
			WF_CPGetNetworkType(iwconfigCb.cpId, &ws.Mode.NetworkType);
			if (ws.Mode.NetworkType == WF_INFRASTRUCTURE)
			{
                if (iwconfigCb.connState == WF_CSTATE_CONNECTION_IN_PROGRESS)
                {
    				WFConsolePrintRomStr("managed (connection in progress)", TRUE);
    		    }
    		    else if (iwconfigCb.connState == WF_CSTATE_CONNECTED_INFRASTRUCTURE)
    		    {
	                WFConsolePrintRomStr("managed", TRUE);        		    
        		}    
        		else if (iwconfigCb.connState == WF_CSTATE_RECONNECTION_IN_PROGRESS)
        		{
                    WFConsolePrintRomStr("managed (reconnection in progress)", TRUE);        		    
                }  		
                else 
                {
                    WFConsolePrintRomStr("managed (?)", TRUE);        		                        
                }    
			}
			else if (ws.Mode.NetworkType == WF_ADHOC)
			{
                if (iwconfigCb.connState == WF_CSTATE_CONNECTION_IN_PROGRESS)
                {
    				WFConsolePrintRomStr("adhoc (connection in progress)", TRUE);
    		    }
    		    else if (iwconfigCb.connState == WF_CSTATE_CONNECTED_ADHOC)
    		    {
	                WFConsolePrintRomStr("adhoc", TRUE);        		    
        		}    
        		else if (iwconfigCb.connState == WF_CSTATE_RECONNECTION_IN_PROGRESS)
        		{
                    WFConsolePrintRomStr("adhoc (reconnection in progress)", TRUE);        		    
                }  		
                else 
                {
                    WFConsolePrintRomStr("adhoc (?)", TRUE);        		                        
                }    

				WFConsolePrintRomStr("adhoc", TRUE);
			}
			else
			{
				WFConsolePrintRomStr("unknown", TRUE);
			}
		}
	}

	// ssid
	{
		WF_CPGetSsid(iwconfigCb.cpId, ws.Ssid.String, &ws.Ssid.Len);
		ws.Ssid.String[ws.Ssid.Len] = '\0';

		WFConsolePrintRomStr("\tssid:     ", FALSE);
		WFConsolePrintRamStr(ws.Ssid.String, TRUE);
	}

	// power
	{
		switch (iwconfigCb.powerSaveState)
		{
		case WF_PS_PS_POLL_DTIM_ENABLED:
			WFConsolePrintRomStr("\tpwrsave:  enabled", TRUE);
			WFConsolePrintRomStr("\tdtim rx:  enabled", TRUE);
			break;
		case WF_PS_PS_POLL_DTIM_DISABLED:
			WFConsolePrintRomStr("\tpwrsave:  enabled", TRUE);
			WFConsolePrintRomStr("\tdtim rx:  disabled", TRUE);
			break;
		case WF_PS_OFF:
			WFConsolePrintRomStr("\tpwrsave:  disabled", TRUE);
			break;
		default:
			WFConsolePrintRomStr("\tpwrsave:  unknown(", FALSE);
			WFConsolePrintInteger(iwconfigCb.powerSaveState, 'd');
			WFConsolePrintRomStr(")", TRUE);
			break;
		}
	}
}

static BOOL iwconfigSetSsid(void)
{
	if (ARGC < 3u)
	{
		WFConsolePrintRomStr("Missing value for last parameter", TRUE);
		return FALSE;
	}

	if (ARGC > 3u)
	{
		WFConsolePrintRomStr("SSID may not contain space for this demo", TRUE);
		return FALSE;
	}

	WF_CPSetSsid(iwconfigCb.cpId, (UINT8 *)ARGV[2], strlen((char*)ARGV[2]));

	return TRUE;
}

static BOOL iwconfigSetMode(void)
{
	UINT8 networkType;

	WF_CPGetNetworkType(iwconfigCb.cpId, &networkType);

    if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "idle") == 0) )
    {
		if ( iwconfigCb.isIdle )
		{
			WFConsolePrintRomStr("Already in the idle mode", TRUE);
		}
		else
		{
		    WF_PsPollDisable();
			WF_CMDisconnect();
			#if defined (WF_HOST_SCAN)
				gHostScanNotAllowed = FALSE;			// don't allow host scan during connection
			#endif
		}
	}
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "managed") == 0) )
    {
		if ( iwconfigCb.isIdle )
		{
			#if defined (WF_HOST_SCAN)
				gHostScanNotAllowed = TRUE;			// don't allow host scan during connection
			#endif

			WF_CPSetNetworkType(iwconfigCb.cpId, WF_INFRASTRUCTURE);
			WF_CMConnect(iwconfigCb.cpId);

		}
		else
		{
			WF_CPGetNetworkType(iwconfigCb.cpId, &networkType);
			if (networkType == WF_INFRASTRUCTURE)
			{
				WFConsolePrintRomStr("Already in the managed mode", TRUE);
			}
			else
			{
				WF_CMDisconnect();

				WF_CPSetNetworkType(iwconfigCb.cpId, WF_INFRASTRUCTURE);
				WF_CMConnect(iwconfigCb.cpId);
			}
		}
	}
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "adhoc") == 0) )
    {
        if ( iwconfigCb.isIdle )

        {
            WF_CPSetNetworkType(iwconfigCb.cpId, WF_ADHOC);
            WF_CPSetAdHocBehavior(iwconfigCb.cpId, WF_ADHOC_CONNECT_THEN_START);
            WF_CMConnect(iwconfigCb.cpId);
        }
		else
		{
			WF_CPGetNetworkType(iwconfigCb.cpId, &networkType);
			if (networkType == WF_ADHOC)
			{
				WFConsolePrintRomStr("Already in the adhoc mode", TRUE);
			}
			else
			{
				WF_CMDisconnect();

				WF_CPSetNetworkType(iwconfigCb.cpId, WF_ADHOC);
				WF_CMConnect(iwconfigCb.cpId);
			}
		}
	}
	else
	{
		WFConsolePrintRomStr("Unknown parameter", TRUE);
		return FALSE;
	}

	return TRUE;
}

static BOOL iwconfigSetChannel(void)
{
	UINT8 *p1, *p2;
	UINT8 *p_channelList;
	UINT8 index = 0;
	UINT16 temp;

	if (ARGC < 3u)
	{
		WFConsolePrintRomStr("Missing value for last parameter", TRUE);
		return FALSE;
	}

	if ( !iwconfigCb.isIdle )
	{
		WFConsolePrintRomStr("Channel can only be set in idle mode", TRUE);
		return FALSE;
	}

	p_channelList = (UINT8*) ARGV[2];
	p1 = p2 = p_channelList;

    if ( strlen( (char*) p_channelList) == 0u )
    	return FALSE;

	if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "all") == 0) )
	{
		WF_CASetChannelList(p_channelList, 0); // reset to domain default channel list
		return TRUE;
	}

    do
    {
       if ( (p2 = (UINT8*) strchr( (const char *) p1, (int) ',')) != NULL )
       {
          *p2='\0';
          p2++;
       }

       if( !ConvertASCIIUnsignedDecimalToBinary((INT8 *)p1, &temp) )
          return  FALSE;

       p1 = p2;
       p_channelList[index] = (UINT8) temp;
       index++;

    } while (  p2 != NULL );

    WF_CASetChannelList(p_channelList, index);

	return TRUE;
}

static BOOL iwconfigSetPower(void)
{
	if (ARGC < 3u)
	{
		WFConsolePrintRomStr("Missing value for last parameter", TRUE);
		return FALSE;
	}

    if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "reenable") == 0) )
    {	// reenable power saving
		WF_PsPollEnable(TRUE);
	}
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "disable") == 0) )
    {	// disable power saving
		WF_PsPollDisable();
	}
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "unicast") == 0) )
    {	// enable power saving but don't poll for DTIM
		WF_PsPollEnable(FALSE);
	}
	else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "all") == 0) )
    {	// enable power saving and poll for DTIM
		WF_PsPollEnable(TRUE);
	}
	else
	{
		WFConsolePrintRomStr("Unknown parameter", TRUE);
		return FALSE;
	}

	return TRUE;
}

static BOOL iwconfigSetDomain(void)
{
	UINT8 domain;

	if (ARGC < 3u)
	{
		WFConsolePrintRomStr("Missing value for last parameter", TRUE);
		return FALSE;
	}

	if ( !iwconfigCb.isIdle )
	{
		WFConsolePrintRomStr("Domain can only be set in idle mode", TRUE);
		return FALSE;
	}

    if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "fcc") == 0) )
    {
		domain = WF_DOMAIN_FCC;
	}
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "ic") == 0) )
    {
		domain = WF_DOMAIN_IC;
	}
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "etsi") == 0) )
    {
		domain = WF_DOMAIN_ETSI;
	}
	else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "spain") == 0) )
    {
		domain = WF_DOMAIN_SPAIN;
	}
	else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "france") == 0) )
    {
		domain = WF_DOMAIN_FRANCE;
	}
	else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "japana") == 0) )
    {
		domain = WF_DOMAIN_JAPAN_A;
	}
	else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "japanb") == 0) )
    {
		domain = WF_DOMAIN_JAPAN_B;
	}
	else
	{
		WFConsolePrintRomStr("Unknown domain", TRUE);
		return FALSE;
	}

	WF_SetRegionalDomain(domain);
	WF_CASetChannelList(NULL, 0); // reset to domain default channel list

	return TRUE;
}

static BOOL iwconfigSetRTS(void)
{
	UINT16 rtsThreshold;

	if (ARGC < 3u)
	{
		WFConsolePrintRomStr("Missing value for last parameter", TRUE);
		return FALSE;
	}

    if( !ConvertASCIIUnsignedDecimalToBinary(ARGV[2], &rtsThreshold) )
        return  FALSE;

	WF_SetRtsThreshold(rtsThreshold);

	return TRUE;
}

static BOOL iwconfigSetTxRate(void)
{
	return FALSE;
}
#endif  /* WF_CONSOLE_IFCFGUTIL */

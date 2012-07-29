/******************************************************************************

 MRF24WB0M Driver 
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24WB0M WiFi controller
  -Reference: MRF24WB0M Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:		WFEasyConfig.c
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
 Steve Collmeyer    24 Sep 2009 Initial
 Brad Rex           09 Feb 2010 Update for RoadRunner
******************************************************************************/
#include "HardwareProfile.h"
#include "TCPIPConfig.h"

#if defined(WF_CS_TRIS)

#include <string.h> /* for memcpy */   //SCC2????

#include "TCPIP Stack/WFEasyConfig.h"
#include "TCPIP Stack/WFApi.h"
#include "TCPIP Stack/WFConsole.h"

#if defined ( EZ_CONFIG_SCAN )
tWFScanCtx  g_ScanCtx;
#endif /* EZ_CONFIG_SCAN */

#if defined	( WF_HOST_SCAN)
extern BOOL gHostScanNotAllowed;
#endif

#if defined(STACK_USE_EZ_CONFIG)
/* Easy Config Globals */
extern UINT8 ConnectionProfileID;   //SCC2 ??? what to do with this...

tWFEasyConfigCtx g_easyConfigCtx;

/* Easy Config Private Functions */
static int WFEasyConfigProcess(void);

void WFEasyConfigInit()
{
    CFGCXT.ssid[0] = 0;
    CFGCXT.security = WF_SECURITY_OPEN;
    CFGCXT.key[0] = 0;
    CFGCXT.defaultWepKey = WF_WEP_KEY_INVALID;
    CFGCXT.type = WF_INFRASTRUCTURE;
    CFGCXT.cfg_state = cfg_stopped;
    CFGCXT.isWifiNeedToConfigure = 0;

    #if defined (EZ_CONFIG_STORE)
    CFGCXT.isWifiDoneConfigure = AppConfig.dataValid;
    #endif
    return;
}

void WFEasyConfigMgr()
{
    if (CFGCXT.isWifiNeedToConfigure) {
        if (WFEasyConfigProcess()) {
            //Has been configured, clear flag
            CFGCXT.isWifiNeedToConfigure = 0;
            CFGCXT.isWifiDoneConfigure = 1;
        }
    }
    return;
}

static int WFEasyConfigProcess(void)
{
    UINT8 ConnectionProfileID;
    UINT8 ConnectionState;
    
    #if defined (EZ_CONFIG_STALL)
    if (CFGCXT.cfg_state == cfg_stopped)
	{
        /* State machine just started get current time stamp */
        CFGCXT.cfg_state = cfg_stalled;
        CFGCXT.timeStart = TickGet();
        return 0;
    }
    
    /* Wait for stall time to expire */
    if (CFGCXT.cfg_state == cfg_stalled)
	{
        UINT32 time = TickGet();
        if ((time - CFGCXT.timeStart) < WF_STALL_TIME_MS)
            return 0;
    }
    
    #endif //EZ_CONFIG_STALL
  
    /* We will re-use the current profile */
    WF_CMGetConnectionState(&ConnectionState, &ConnectionProfileID);

    /* Need to disconnect */
    WF_CMDisconnect();

    /* Delete profile */
    WF_CPDelete(ConnectionProfileID);

    /* Create and prepare new profile */
    WF_CPCreate(&ConnectionProfileID);

    /* Now set the various connection profile parameters */

    /* Set SSID... */
    if (CFGCXT.ssid)
#if defined(__18CXX)
        WF_CPSetSsid(ConnectionProfileID, 
        	//(ROM char *)CFGCXT.ssid,   Note (VMH): fixed compile warning - not sure why this is necessary.
        	CFGCXT.ssid, 
            strlen(CFGCXT.ssid));  
#else
        WF_CPSetSsid(ConnectionProfileID, 
            CFGCXT.ssid, 
            strlen((char*)CFGCXT.ssid));  
#endif       

    /* Now deal with security... */
    switch ((BYTE)CFGCXT.security) {
        case WF_SECURITY_OPEN: /* No security */
            WF_CPSetSecurity(ConnectionProfileID, WF_SECURITY_OPEN, 0, 0, 0);
            break; 

        case WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE:
            if (CFGCXT.key) {
                WF_CPSetSecurity(ConnectionProfileID, WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE, 0, CFGCXT.key, strlen((char *)CFGCXT.key));
            }
            break;

        case WF_SECURITY_WPA_AUTO_WITH_KEY:
            if (CFGCXT.key) {
                WF_CPSetSecurity(ConnectionProfileID, WF_SECURITY_WPA_AUTO_WITH_KEY, 0, CFGCXT.key, 32);
            }
            break;

        case WF_SECURITY_WEP_40:
            {
                BYTE  keys[20];
                int   i;

                if (CFGCXT.key) {
                    /* Clear key */
                    for (i = 0; i < 20; i++)
                        keys[i] = 0;
                    memcpy(keys, (void*)CFGCXT.key, 20);
                    WF_CPSetSecurity(ConnectionProfileID, WF_SECURITY_WEP_40, CFGCXT.defaultWepKey, keys, 20);
                }
            }
            break;

        case WF_SECURITY_WEP_104:
            {
                BYTE  keys[52];
                int   i;

                if (CFGCXT.key) {
                    /* Clear key */
                    for (i = 0; i < 52; i++)
                        keys[i] = 0;
                    memcpy(keys, (void*)CFGCXT.key, 52);
                    WF_CPSetSecurity(ConnectionProfileID, WF_SECURITY_WEP_104, CFGCXT.defaultWepKey, keys, 52);
                }
            }
            break;
    }
 
    #if defined (EZ_CONFIG_STORE)
    SaveAppConfig(&AppConfig);
    #endif

    /* Set wlan mode */
    WF_CPSetNetworkType(ConnectionProfileID, CFGCXT.type);

    /* Kick off connection now... */
    WF_CMConnect(ConnectionProfileID);
    /* Change state and return TRUE to show we are done! */
    CFGCXT.cfg_state = cfg_stopped;

    return 1;
}
#endif /* STACK_USE_EZ_CONFIG */

#if defined ( EZ_CONFIG_SCAN ) || defined (WF_HOST_SCAN)
void WFInitScan(void)
{
    SCANCXT.scanState = 0;
    SCANCXT.numScanResults = 0;
    SCANCXT.displayIdx = 0;

    return;
}

UINT16 WFStartScan(void)
{
   /* If scan already in progress bail out */
   if (IS_SCAN_IN_PROGRESS(SCANCXT.scanState))
       return WF_ERROR_OPERATION_CANCELLED;

   WF_Scan(0xff);

   SCAN_SET_IN_PROGRESS(SCANCXT.scanState);
   /* Should they be invalidated??? */
   //SCAN_CLEAR_VALID(SCANCXT.scanState);

   return WF_SUCCESS;
}

#if defined (WF_HOST_SCAN)
UINT16 WFStartHostScan(UINT8 profile_id)
{
   /* If scan already in progress bail out */
   if (IS_SCAN_IN_PROGRESS(SCANCXT.scanState))
       return WF_ERROR_OPERATION_CANCELLED;

   WF_Scan(profile_id);

   SCAN_SET_IN_PROGRESS(SCANCXT.scanState);
   /* Should they be invalidated??? */
   //SCAN_CLEAR_VALID(SCANCXT.scanState);

   return WF_SUCCESS;
}
#endif

UINT16 WFRetrieveScanResult(UINT8 Idx, tWFScanResult *p_ScanResult)
{
    if (Idx >= SCANCXT.numScanResults)
        return WF_ERROR_INVALID_PARAM;

    WF_ScanGetResult(Idx, p_ScanResult);
    p_ScanResult->ssid[p_ScanResult->ssidLen] = 0; /* Terminate */

    return WF_SUCCESS;
}

void WFScanEventHandler(UINT16 scanResults)
{
    /* Cache number APs found in scan */
    SCANCXT.numScanResults = scanResults;

    /* Clear the scan in progress */
    SCAN_CLEAR_IN_PROGRESS(SCANCXT.scanState);
    SCAN_SET_VALID(SCANCXT.scanState);

    return;
}
#endif /* EZ_CONFIG_SCAN */

#if defined ( WF_CONSOLE ) && defined ( EZ_CONFIG_SCAN ) 
extern void
WFDisplayScanMgr()
{
    tWFScanResult   bssDesc;
    char ssid[32];
	char rssiChan[48];
	int	count;

    if (SCANCXT.numScanResults == 0)
       return;
    if (!IS_SCAN_STATE_DISPLAY(SCANCXT.scanState))
       return;

    if (IS_SCAN_IN_PROGRESS(SCANCXT.scanState))
       return;

    if (!IS_SCAN_STATE_VALID(SCANCXT.scanState))
       return;

    WFRetrieveScanResult(SCANCXT.displayIdx, &bssDesc);

    /* Display SSID */
	count = SCANCXT.displayIdx + 1;
    sprintf(ssid, "%d SSID: %s\r\n", count, bssDesc.ssid);
    putsUART(ssid);

	/* Display SSID  & Channel */
    /* RSSI_MAX : 200, RSSI_MIN : 106 */
    sprintf(rssiChan, "  => RSSI: %u, Channel: %u\r\n", bssDesc.rssi, bssDesc.channel);
    putsUART(rssiChan);

    if (++SCANCXT.displayIdx == SCANCXT.numScanResults)  {
        SCAN_CLEAR_DISPLAY(SCANCXT.scanState);
        SCANCXT.displayIdx = 0;
#if defined(WF_CONSOLE) & defined(STACK_USE_UART)
        WFConsoleReleaseConsoleMsg();
#endif

#if defined(WF_HOST_SCAN)
		gHostScanNotAllowed = FALSE;
#endif

    }

    return;
}
#endif /* WF_CONSOLE */

#endif /* WF_CS_TRIS */

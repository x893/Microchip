/*********************************************************************
 *
 *  Main Application Entry Point
 *   -Demonstrates how to call and use the Microchip WiFi Module and
 *    TCP/IP stack
 *   -Reference: Microchip TCP/IP Stack Help (TCPIP Stack Help.chm)
 *
 *********************************************************************
 * FileName:        MainDemo.c
 * Dependencies:    TCPIP.h
 * Processor:       PIC32
 * Compiler:        Microchip C32 v1.11b or higher
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
 *	ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *	used in conjunction with a Microchip ethernet controller for
 *	the sole purpose of interfacing with the ethernet controller.
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
 * File Description:
 * Change History:
 * Date         Comment
 * ----------   -----------------------------------------
 * 2/21/2012    Initial release
 ********************************************************************/

#include "MainDemo.h"

#if defined( WF_CONSOLE )
	#include "TCPIP Stack/WFConsole.h"
	#include "IperfApp.h"
#endif 

#ifndef __STM32F10X__
	// Set configuration fuses
	#pragma config FNOSC = FRCPLL, FPLLIDIV = DIV_2, FPLLMUL = MUL_20, FPLLODIV = DIV_2, FPBDIV = DIV_1, FWDTEN = OFF, POSCMOD = OFF, FSOSCEN = OFF, CP = OFF
#endif
//--------------------------------------------------------------------------------
// These defines use the C preprocessor to create the AppConfig structure elements
//--------------------------------------------------------------------------------
// MDEF(R,N) macro concatenates tokens to form "MY_DEFAULT_<R>BYTE<N>"
// For example MDEF(IP_ADDR_,4) expands to "MY_DEFAULT_IP_ADDR_BYTE4"

#define MDEF(R,N) MY_DEFAULT_ ## R ## BYTE ## N

// BUILD_MYDEFAULT32 invokes the MDEF(R,N) macro 4 times to create a 32 bit
// value from 4 byte values.
//
// For example BUILD_MYDEFAULT32(IP_ADDR_) expands to
// (MY_DEFAULT_IP_ADDR_BYTE4<<24|MY_DEFAULT_IP_ADDR_BYTE3<<16
//  |MY_DEFAULT_IP_ADDR_BYTE2<<8|MY_DEFAULT_IP_ADDR_BYTE1)

#define BUILD_MYDEFAULT32(R) (MDEF(R,4) << 24 | MDEF(R,3) << 16 | MDEF(R,2) << 8 | MDEF(R,1))

// MACD(N) expands to MY_DEFAULT_MAC_BYTE<N>
// For example MACD(1) expands to MY_DEFAULT_MAC_BYTE1

#define MACD(N) MY_DEFAULT_MAC_BYTE ## N

// Application-dependent structure used to contain IP address information
APP_CONFIG AppConfig =
{
	{ BUILD_MYDEFAULT32(IP_ADDR_)		},
	{ BUILD_MYDEFAULT32(MASK_)			},
	{ BUILD_MYDEFAULT32(GATE_)			},
	{ BUILD_MYDEFAULT32(PRIMARY_DNS_)	},
	{ BUILD_MYDEFAULT32(SECONDARY_DNS_)	},
	{ BUILD_MYDEFAULT32(IP_ADDR_)		},
	{ BUILD_MYDEFAULT32(MASK_)			},
	MY_DEFAULT_HOST_NAME,
	{1, 1},	// Flags
	{ {MACD(1), MACD(2), MACD(3), MACD(4), MACD(5), MACD(6)} }
};

// Private helper functions.
void InitializeBoard(void);

void _general_exception_handler(unsigned cause, unsigned status)
{
    Nop();
    Nop();
}

// ************************************************************
// Main application entry point.
// ************************************************************
int main(void)
{
    static DWORD t = 0;
	DWORD dwLastIP = 0;

    // Initialize application specific hardware
    InitializeBoard();

#if defined(USE_LCD)
	// Initialize and display the stack version on the LCD
	LCDInit();
	DelayMs(100);
	strcpypgm2ram((char*)LCDText, "TCPStack " TCPIP_STACK_VERSION "  "
		"                "); 
	LCDUpdate();
#endif

	// Initialize stack-related hardware components that may be 
	// required by the UART configuration routines
    TickInit();

#if defined(STACK_USE_MPFS2)
	MPFSInit();
#endif

    // Initialize core stack layers (MAC, ARP, TCP, UDP) and
    // application modules (HTTP, SNMP, etc.)
    StackInit();

    // Initialize WiFi Scan State Machine NV variables
    WFInitScan();

    // Establish WiFi Connection
    WiFiConnect();

    // Initialize Zeroconf Link-Local state-machine.
	#ifdef STACK_USE_ZEROCONF_LINK_LOCAL
    ZeroconfLLInitialize();

    // Initialize DNS Host-Name from TCPIPConfig.h.
    mDNSInitialize(MY_DEFAULT_HOST_NAME);
    mDNSServiceRegister(
            (const char *) AppConfig.NetBIOSName,	// base name of the service
            "_http._tcp.local",			    // type of the service
            80,				                // TCP or UDP port, at which this service is available
            ((const BYTE *)"path=/index.htm"),	// TXT info
            1,								    // auto rename the service when if needed
            NULL,							    // no callback function
            NULL							    // no application context
            );

    mDNSMulticastFilterRegister();			
	#endif

    #if defined(WF_CONSOLE)
    // Initialize the WiFi Console App
	WFConsoleInit();
    #endif

    // Now that all items are initialized, begin the co-operative
    // multitasking loop.  This infinite loop will continuously
    // execute all stack-related tasks, as well as your own
    // application's functions.  Custom functions should be added
    // at the end of this loop.
    // Note that this is a "co-operative mult-tasking" mechanism
    // where every task performs its tasks (whether all in one shot
    // or part of it) and returns so that other tasks can do their
    // job.
    // If a task needs very long time to do its job, it must be broken
    // down into smaller pieces so that other tasks can have CPU time.
    while(1)
    {
        // Blink LED0 twice per sec when unconfigured, once per sec after config
        if((TickGet() - t >= TICK_SECOND/(4ul - (CFGCXT.isWifiDoneConfigure*2ul))))
        {
            t = TickGet();
            LED0_INV();
        }

        // This task performs normal stack task including checking
        // for incoming packet, type of packet and calling
        // appropriate stack entity to process it.
        StackTask();

        // This task invokes each of the core stack application tasks
        StackApplications();

	#ifdef STACK_USE_ZEROCONF_LINK_LOCAL
        // This task handles Zeroconf Link-Local events and takes the actions
        // accoding to current-state and event-notifications from ARP-Module.
        ZeroconfLLProcess();

        // This routine calls the mDNS state-machine
        mDNSProcess();
	#endif
        // Process application specific tasks here.
        // Any custom modules or processing you need to do should
        // go here.
	#if defined(WF_CONSOLE)
		WFConsoleProcess();
		WFConsoleProcessEpilogue();
	#endif

		if (dwLastIP != AppConfig.MyIPAddr.Val)
		{
			dwLastIP = AppConfig.MyIPAddr.Val;

#if defined(STACK_USE_UART)
			putrsUART((ROM char*)"\r\nNew IP Address: ");
#endif

			DisplayIPValue(AppConfig.MyIPAddr);

#if defined(STACK_USE_UART)
			putrsUART((ROM char*)"\r\n");
#endif
		}
    }
}

/****************************************************************************
  Function:
    static void InitializeBoard(void)

  Description:
    This routine initializes the hardware.

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void InitializeBoard(void)
{
    // Note: WiFi Module hardware Initialization handled by StackInit() Library Routine
	BOARD_INIT();

    // LEDs
    LEDS_OFF();
    LED0_INIT();

    // Push Button
    SW0_INIT();

#if defined(STACK_USE_UART)
	STACK_USE_UART_INIT();
#endif
}

/****************************************************************************
  Function:
    void DisplayIPValue(IP_ADDR IPVal)

  Description:
    This routine formats and prints the current IP Address.

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void DisplayIPValue(IP_ADDR IPVal)
{
	//	printf("%u.%u.%u.%u", IPVal.v[0], IPVal.v[1], IPVal.v[2], IPVal.v[3]);
	BYTE IPDigit[4];
	BYTE i;
#ifdef USE_LCD
	BYTE j;
	BYTE LCDPos = 16;
#endif

	for(i = 0; i < sizeof(IP_ADDR); i++)
	{
		uitoa((WORD)IPVal.v[i], IPDigit);

#if defined(STACK_USE_UART)
		putsUART((char *) IPDigit);
#endif

#ifdef USE_LCD
		for(j = 0; j < strlen((char*)IPDigit); j++)
		{
			LCDText[LCDPos++] = IPDigit[j];
		}
		if (i == sizeof(IP_ADDR) - 1)
			break;
		LCDText[LCDPos++] = '.';
#else
		if (i == sizeof(IP_ADDR) - 1)
			break;
#endif

#if defined(STACK_USE_UART)
		while(BusyUART());
		WriteUART('.');
#endif
	}

#ifdef USE_LCD
	if (LCDPos < 32u)
		LCDText[LCDPos] = 0;
	LCDUpdate();
#endif
}

/*
* This macro uniquely defines this file as the main entry point.
* There should only be one such definition in the entire project,
* and this file must define the AppConfig variable as described below.
*/
#define THIS_IS_STACK_APPLICATION

// Include all headers for any enabled TCPIP Stack functions
#include "TCPIP Stack/TCPIP.h"

#if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
	#include "TCPIP_Stack/ZeroconfLinkLocal.h"
#endif

#if defined(STACK_USE_ZEROCONF_MDNS_SD)
	#include "TCPIP_Stack/ZeroconfMulticastDNS.h"
#endif

// Include functions specific to this stack application
#include "MainDemo.h"

// Used for Wi-Fi assertions
#define WF_MODULE_NUMBER   WF_MODULE_MAIN_DEMO

// Declare AppConfig structure and some other supporting stack variables
APP_CONFIG AppConfig;
//! static unsigned short wOriginalAppConfigChecksum;	// Checksum of the ROM defaults for AppConfig
BYTE AN0String[8];

// Private helper functions.
// These may or may not be present in all applications.
static void InitAppConfig(void);
static void InitializeBoard(void);
static void ProcessIO(void);

#if defined(WF_CS_TRIS)
	static void WF_Connect(void);
	extern BOOL gRFModuleVer1209orLater;
#endif

//
// Main application entry point.
//
int main(void)
{
	static DWORD t = 0;
	static DWORD dwLastIP = 0;

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

	// Initialize Stack and application related NV variables into AppConfig.
	InitAppConfig();

#if defined(STACK_USE_UART)
		DoUARTConfig();
#endif

	// Initiates board setup process if button is depressed on startup
	if (BUTTON0_PRESS())
	{
#if defined(EEPROM_CS_TRIS) || defined(SPIFLASH_CS_TRIS)
		// Invalidate the EEPROM contents if BUTTON0 is held down for more than 4 seconds
		DWORD StartTime = TickGet();
		LEDS_OFF();

		while(BUTTON0_PRESS())
		{
			if (TickGet() - StartTime > 4 * TICK_SECOND)
			{
	#if defined(EEPROM_CS_TRIS)
				XEEBeginWrite(0x0000);
				XEEWrite(0xFF);
				XEEWrite(0xFF);
				XEEEndWrite();
	#elif defined(SPIFLASH_CS_TRIS)
				SPIFlashBeginWrite(0x0000);
				SPIFlashWrite(0xFF);
				SPIFlashWrite(0xFF);
	#endif

	#if defined(STACK_USE_UART)
				putrsUART("\r\n\r\nBUTTON0 held for more than 4 seconds.  Default settings restored.\r\n\r\n");
	#endif
				LEDS_ON();
				while((LONG)(TickGet() - StartTime) <= (LONG)(9 * TICK_SECOND / 2));
				LEDS_OFF();
				while(BUTTON0_PRESS());
				Reset();
				break;
			}
		}
#endif
	}

	// Initialize core stack layers (MAC, ARP, TCP, UDP) and
	// application modules (HTTP, SNMP, etc.)
	StackInit();

#if defined(WF_CS_TRIS)
	WF_Connect();
#endif

	// Initialize any application-specific modules or functions/
	// For this demo application, this only includes the
	// UART 2 TCP Bridge
#if defined(STACK_USE_UART2TCP_BRIDGE)
	UART2TCPBridgeInit();
#endif

#if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
	ZeroconfLLInitialize();
#endif

#if defined(STACK_USE_ZEROCONF_MDNS_SD)
	mDNSInitialize(MY_DEFAULT_HOST_NAME);
	mDNSServiceRegister(
		(const char *) "DemoWebServer",		// base name of the service
		"_http._tcp.local",					// type of the service
		80,									// TCP or UDP port, at which this service is available
		((const BYTE *)"path=/index.htm"),	// TXT info
		1,									// auto rename the service when if needed
		NULL,								// no callback function
		NULL								// no application context
		);

	mDNSMulticastFilterRegister();			
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
		// Blink LED0 (right most one) every second.
		if ((TickGet() - t) >= TICK_SECOND / 2ul)
		{
			t = TickGet();
			LED1_TOGGLE();
		}

		// This task performs normal stack task including checking
		// for incoming packet, type of packet and calling
		// appropriate stack entity to process it.
		StackTask();

#if defined(WF_CS_TRIS)
		if (gRFModuleVer1209orLater)
			WiFiTask();
#endif

		// This tasks invokes each of the core stack application tasks
		StackApplications();

#if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
		ZeroconfLLProcess();
#endif

#if defined(STACK_USE_ZEROCONF_MDNS_SD)
		mDNSProcess();
		// Use this function to exercise service update function
		// HTTPUpdateRecord();
#endif

		// Process application specific tasks here.
		// For this demo app, this will include the Generic TCP 
		// client and servers, and the SNMP, Ping, and SNMP Trap
		// demos.  Following that, we will process any IO from
		// the inputs on the board itself.
		// Any custom modules or processing you need to do should
		// go here.

#if defined(STACK_USE_GENERIC_TCP_CLIENT_EXAMPLE)
		GenericTCPClient();
#endif

#if defined(STACK_USE_GENERIC_TCP_SERVER_EXAMPLE)
		GenericTCPServer();
#endif

#if defined(STACK_USE_SMTP_CLIENT)
		SMTPDemo();
#endif

#if defined(STACK_USE_ICMP_CLIENT)
		PingDemo();
#endif

#if defined(STACK_USE_SNMP_SERVER) && !defined(SNMP_TRAP_DISABLED)
		//User should use one of the following SNMP demo
		// This routine demonstrates V1 or V2 trap formats with one variable binding.
		SNMPTrapDemo();

	#if defined(SNMP_STACK_USE_V2_TRAP) || defined(SNMP_V1_V2_TRAP_WITH_SNMPV3)
		//This routine provides V2 format notifications with multiple (3) variable bindings
		//User should modify this routine to send v2 trap format notifications with the required varbinds.
		//SNMPV2TrapDemo();
	#endif 
		if (gSendTrapFlag)
			SNMPSendTrap();
#endif

#if defined(STACK_USE_BERKELEY_API)
		BerkeleyTCPClientDemo();
		BerkeleyTCPServerDemo();
		BerkeleyUDPClientDemo();
#endif

		ProcessIO();

		// If the local IP address has changed (ex: due to DHCP lease change)
		// write the new IP address to the LCD display, UART, and Announce 
		// service
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

#if defined(STACK_USE_ANNOUNCE)
			AnnounceIP();
#endif

#if defined(STACK_USE_ZEROCONF_MDNS_SD)
			mDNSFillHostRecord();
#endif
		}
	}
}

#if defined(WF_CS_TRIS)
/*****************************************************************************
* FUNCTION: WF_Connect
*
* RETURNS:  None
*
* PARAMS:   None
*
*  NOTES:   Connects to an 802.11 network.  Customize this function as needed 
*           for your application.
*****************************************************************************/
static void WF_Connect(void)
{
	UINT8 ConnectionProfileID;
	UINT8 channelList[] = MY_DEFAULT_CHANNEL_LIST;

	/* create a Connection Profile */
	WF_CPCreate(&ConnectionProfileID);

#if defined(STACK_USE_UART)
	putrsUART("Set SSID (");
	putsUART(AppConfig.MySSID);
	putrsUART(")\r\n");
#endif
	WF_CPSetSsid(
		ConnectionProfileID,
		AppConfig.MySSID,
		AppConfig.SsidLength
	);

#if defined(STACK_USE_UART)
	putrsUART("Set Network Type\r\n");
#endif
	WF_CPSetNetworkType(ConnectionProfileID, MY_DEFAULT_NETWORK_TYPE);

#if defined(STACK_USE_UART)
	putrsUART("Set Scan Type\r\n");
#endif
	WF_CASetScanType(MY_DEFAULT_SCAN_TYPE);

#if defined(STACK_USE_UART)
	putrsUART("Set Channel List\r\n");
#endif
	WF_CASetChannelList(channelList, sizeof(channelList));

#if defined(STACK_USE_UART)
	putrsUART("Set list retry count\r\n");
#endif

	// The Retry Count parameter tells the WiFi Connection manager how many attempts to make when trying
	// to connect to an existing network.  In the Infrastructure case, the default is to retry forever so that
	// if the AP is turned off or out of range, the radio will continue to attempt a connection until the
	// AP is eventually back on or in range.  In the Adhoc case, the default is to retry 3 times since the 
	// purpose of attempting to establish a network in the Adhoc case is only to verify that one does not
	// initially exist.  If the retry count was set to WF_RETRY_FOREVER in the AdHoc mode, an AdHoc network
	// would never be established.  The constants MY_DEFAULT_LIST_RETRY_COUNT_ADHOC and 
	// MY_DEFAULT_LIST_RETRY_COUNT_INFRASTRUCTURE have been created specifically for the June 2011 MAL release.
#if defined(EZ_CONFIG_STORE)
	if (AppConfig.networkType == WF_ADHOC)
		WF_CASetListRetryCount(MY_DEFAULT_LIST_RETRY_COUNT_ADHOC);
	else
		WF_CASetListRetryCount(MY_DEFAULT_LIST_RETRY_COUNT_INFRASTRUCTURE);
#else
	#if (MY_DEFAULT_NETWORK_TYPE == WF_ADHOC)
		WF_CASetListRetryCount(MY_DEFAULT_LIST_RETRY_COUNT_ADHOC);
	#else
		WF_CASetListRetryCount(MY_DEFAULT_LIST_RETRY_COUNT_INFRASTRUCTURE);
	#endif
#endif

#if defined(STACK_USE_UART)        
	putrsUART("Set Event Notify\r\n");    
#endif
	WF_CASetEventNotificationAction(MY_DEFAULT_EVENT_NOTIFICATION_LIST);

#if defined(STACK_USE_UART)
	putrsUART("Set Beacon Timeout\r\n");
#endif

	WF_CASetBeaconTimeout(40);

	if (gRFModuleVer1209orLater)
	{
		// If WEP security is used, set WEP Key Type.  The default WEP Key Type is Shared Key.
		if (AppConfig.SecurityMode == WF_SECURITY_WEP_40 || AppConfig.SecurityMode == WF_SECURITY_WEP_104)
		{
			WF_CPSetWepKeyType(ConnectionProfileID, MY_DEFAULT_WIFI_SECURITY_WEP_KEYTYPE);
		}
	}

	/* Set Security */
#if defined(STACK_USE_UART)
	#if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_OPEN)
		putrsUART("Set Security (Open)\r\n");
	#elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_40)
		putrsUART("Set Security (WEP40)\r\n");
	#elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_104)
		putrsUART("Set Security (WEP104)\r\n");
	#elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_WITH_KEY 
		putrsUART("Set Security (WPA with key)\r\n");
	#elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA2_WITH_KEY 
		putrsUART("Set Security (WPA2 with key)\r\n");
	#elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_WITH_PASS_PHRASE
		putrsUART("Set Security (WPA with pass phrase)\r\n");
	#elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA2_WITH_PASS_PHRASE
		putrsUART("Set Security (WPA2 with pass phrase)\r\n");    
	#elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_KEY
		putrsUART("Set Security (WPA with key, auto-select)\r\n");
	#elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE
		putrsUART("Set Security (WPA with pass phrase, auto-select)\r\n");
	#endif /* MY_DEFAULT_WIFI_SECURITY_MODE */
#endif

	WF_CPSetSecurity(ConnectionProfileID,
		AppConfig.SecurityMode,
		AppConfig.WepKeyIndex,   /* only used if WEP enabled */
		AppConfig.SecurityKey,
		AppConfig.SecurityKeyLength);

#if MY_DEFAULT_PS_POLL == WF_ENABLED
	WF_PsPollEnable(TRUE);
	if (gRFModuleVer1209orLater)
		WFEnableDeferredPowerSave();
#elif defined (WF_USE_POWER_SAVE_FUNCTIONS)
	WF_PsPollDisable();
#endif

#ifdef WF_AGGRESSIVE_PS
	if (gRFModuleVer1209orLater)
		WFEnableAggressivePowerSave();
#endif

#if defined(STACK_USE_UART)                     
	putrsUART("Start WiFi Connect\r\n");        
#endif

	WF_CMConnect(ConnectionProfileID);
}
#endif /* WF_CS_TRIS */


// Writes an IP address to the LCD display and the UART as available
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

// Processes A/D data from the potentiometer
static void ProcessIO(void)
{
	// Convert 10-bit value into ASCII string
	uitoa(12345, AN0String);
}


/****************************************************************************
Function:
static void InitializeBoard(void)

Description:
This routine initializes the hardware.  It is a generic initialization
routine for many of the Microchip development boards, using definitions
in HardwareProfile.h to determine specific initialization.

Precondition:
None

Parameters:
None - None

Returns:
None

Remarks:
None
***************************************************************************/
static void InitializeBoard(void)
{
#if defined(STACK_USE_UART)
	USART_InitTypeDef USART_InitStructure;
#endif

	RCC_APB2PeriphClockCmd(
		RCC_APB2Periph_GPIOA	|
		RCC_APB2Periph_GPIOB	|
		RCC_APB2Periph_GPIOC	|
		RCC_APB2Periph_AFIO, ENABLE
	);

	BOARD_INIT();

	LED0_INIT();
	LED1_INIT();
	BUTTON0_INIT();

#if defined WF_CS_TRIS
	WF_CS_HIGH();
	WF_CS_INIT();
#endif

#if defined(STACK_USE_UART)

	STACK_USE_UART_INIT();

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(STACK_USE_UART_PORT, &USART_InitStructure);
	USART_Cmd(STACK_USE_UART_PORT, ENABLE);

#endif
}

/*********************************************************************
* Function:        void InitAppConfig(void)
*
* PreCondition:    MPFSInit() is already called.
*
* Input:           None
*
* Output:          Write/Read non-volatile config variables.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            None
********************************************************************/
// MAC Address Serialization using a MPLAB PM3 Programmer and 
// Serialized Quick Turn Programming (SQTP). 
// The advantage of using SQTP for programming the MAC Address is it
// allows you to auto-increment the MAC address without recompiling 
// the code for each unit.  To use SQTP, the MAC address must be fixed
// at a specific location in program memory.  Uncomment these two pragmas
// that locate the MAC address at 0x1FFF0.  Syntax below is for MPLAB C 
// Compiler for PIC18 MCUs. Syntax will vary for other compilers.
//#pragma romdata MACROM=0x1FFF0
static ROM BYTE SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};
//#pragma romdata

static void InitAppConfig(void)
{
#if defined(EEPROM_CS_TRIS) || defined(SPIFLASH_CS_TRIS)
	unsigned char vNeedToSaveDefaults = 0;
#endif

	while(1)
	{
		// Start out zeroing all AppConfig bytes to ensure all fields are 
		// deterministic for checksum generation
		memset((void*)&AppConfig, 0x00, sizeof(AppConfig));

		AppConfig.Flags.bIsDHCPEnabled = FALSE; //! TRUE;
		AppConfig.Flags.bInConfigMode = TRUE;
		memcpypgm2ram((void*)&AppConfig.MyMACAddr, (ROM void*)SerializedMACAddress, sizeof(AppConfig.MyMACAddr));
		//		{
		//			_prog_addressT MACAddressAddress;
		//			MACAddressAddress.next = 0x157F8;
		//			_memcpy_p2d24((char*)&AppConfig.MyMACAddr, MACAddressAddress, sizeof(AppConfig.MyMACAddr));
		//		}
		AppConfig.MyIPAddr.Val = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | MY_DEFAULT_IP_ADDR_BYTE4<<24ul;
		AppConfig.DefaultIPAddr.Val = AppConfig.MyIPAddr.Val;
		AppConfig.MyMask.Val = MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2<<8ul | MY_DEFAULT_MASK_BYTE3<<16ul | MY_DEFAULT_MASK_BYTE4<<24ul;
		AppConfig.DefaultMask.Val = AppConfig.MyMask.Val;
		AppConfig.MyGateway.Val = MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2<<8ul | MY_DEFAULT_GATE_BYTE3<<16ul | MY_DEFAULT_GATE_BYTE4<<24ul;
		AppConfig.PrimaryDNSServer.Val = MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2<<8ul  | MY_DEFAULT_PRIMARY_DNS_BYTE3<<16ul  | MY_DEFAULT_PRIMARY_DNS_BYTE4<<24ul;
		AppConfig.SecondaryDNSServer.Val = MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2<<8ul  | MY_DEFAULT_SECONDARY_DNS_BYTE3<<16ul  | MY_DEFAULT_SECONDARY_DNS_BYTE4<<24ul;


		// SNMP Community String configuration
#if defined(STACK_USE_SNMP_SERVER)
		{
			BYTE i;
			static ROM char * ROM cReadCommunities[] = SNMP_READ_COMMUNITIES;
			static ROM char * ROM cWriteCommunities[] = SNMP_WRITE_COMMUNITIES;
			ROM char * strCommunity;

			for(i = 0; i < SNMP_MAX_COMMUNITY_SUPPORT; i++)
			{
				// Get a pointer to the next community string
				strCommunity = cReadCommunities[i];
				if (i >= sizeof(cReadCommunities)/sizeof(cReadCommunities[0]))
					strCommunity = "";

				// Ensure we don't buffer overflow.  If your code gets stuck here, 
				// it means your SNMP_COMMUNITY_MAX_LEN definition in TCPIPConfig.h 
				// is either too small or one of your community string lengths 
				// (SNMP_READ_COMMUNITIES) are too large.  Fix either.
				if (strlenpgm(strCommunity) >= sizeof(AppConfig.readCommunity[0]))
					while(1);

				// Copy string into AppConfig
				strcpypgm2ram((char*)AppConfig.readCommunity[i], strCommunity);

				// Get a pointer to the next community string
				strCommunity = cWriteCommunities[i];
				if (i >= sizeof(cWriteCommunities)/sizeof(cWriteCommunities[0]))
					strCommunity = "";

				// Ensure we don't buffer overflow.  If your code gets stuck here, 
				// it means your SNMP_COMMUNITY_MAX_LEN definition in TCPIPConfig.h 
				// is either too small or one of your community string lengths 
				// (SNMP_WRITE_COMMUNITIES) are too large.  Fix either.
				if (strlenpgm(strCommunity) >= sizeof(AppConfig.writeCommunity[0]))
					while(1);

				// Copy string into AppConfig
				strcpypgm2ram((char*)AppConfig.writeCommunity[i], strCommunity);
			}
		}
#endif

		// Load the default NetBIOS Host Name
		memcpypgm2ram(AppConfig.NetBIOSName, (ROM void*)MY_DEFAULT_HOST_NAME, 16);
		FormatNetBIOSName(AppConfig.NetBIOSName);

#if defined(WF_CS_TRIS)
		// Load the default SSID Name
		WF_ASSERT(sizeof(MY_DEFAULT_SSID_NAME) <= sizeof(AppConfig.MySSID));
		memcpypgm2ram(AppConfig.MySSID, (ROM void*)MY_DEFAULT_SSID_NAME, sizeof(MY_DEFAULT_SSID_NAME));
		AppConfig.SsidLength = sizeof(MY_DEFAULT_SSID_NAME) - 1;

		AppConfig.SecurityMode = MY_DEFAULT_WIFI_SECURITY_MODE;
		AppConfig.WepKeyIndex  = MY_DEFAULT_WEP_KEY_INDEX;

#if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_OPEN)
		memset(AppConfig.SecurityKey, 0x00, sizeof(AppConfig.SecurityKey));
		AppConfig.SecurityKeyLength = 0;

#elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_40
		memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_WEP_KEYS_40, sizeof(MY_DEFAULT_WEP_KEYS_40) - 1);
		AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_WEP_KEYS_40) - 1;

#elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_104
		memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_WEP_KEYS_104, sizeof(MY_DEFAULT_WEP_KEYS_104) - 1);
		AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_WEP_KEYS_104) - 1;

#elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_WITH_KEY)	|| \
	(MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA2_WITH_KEY)	|| \
	(MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_KEY)
		memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_PSK, sizeof(MY_DEFAULT_PSK) - 1);
		AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_PSK) - 1;

#elif	(MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_WITH_PASS_PHRASE)		|| \
		(MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA2_WITH_PASS_PHRASE)	|| \
		(MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE)
		memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_PSK_PHRASE, sizeof(MY_DEFAULT_PSK_PHRASE) - 1);
		AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_PSK_PHRASE) - 1;

#else 
#error "No security defined"
#endif /* MY_DEFAULT_WIFI_SECURITY_MODE */

#endif

		// Compute the checksum of the AppConfig defaults as loaded from ROM
		//! wOriginalAppConfigChecksum = CalcIPChecksum((BYTE*)&AppConfig, sizeof(AppConfig));
		CalcIPChecksum((BYTE*)&AppConfig, sizeof(AppConfig));

#if defined(EEPROM_CS_TRIS) || defined(SPIFLASH_CS_TRIS)
		{
			NVM_VALIDATION_STRUCT NVMValidationStruct;

			// Check to see if we have a flag set indicating that we need to 
			// save the ROM default AppConfig values.
			if (vNeedToSaveDefaults)
				SaveAppConfig(&AppConfig);

			// Read the NVMValidation record and AppConfig struct out of EEPROM/Flash
#if defined(EEPROM_CS_TRIS)
			{
				XEEReadArray(0x0000, (BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
				XEEReadArray(sizeof(NVMValidationStruct), (BYTE*)&AppConfig, sizeof(AppConfig));
			}
#elif defined(SPIFLASH_CS_TRIS)
			{
				SPIFlashReadArray(0x0000, (BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
				SPIFlashReadArray(sizeof(NVMValidationStruct), (BYTE*)&AppConfig, sizeof(AppConfig));
			}
#endif

			// Check EEPROM/Flash validitity.  If it isn't valid, set a flag so 
			// that we will save the ROM default values on the next loop 
			// iteration.
			if ((NVMValidationStruct.wConfigurationLength != sizeof(AppConfig)) ||
				(NVMValidationStruct.wOriginalChecksum != wOriginalAppConfigChecksum) ||
				(NVMValidationStruct.wCurrentChecksum != CalcIPChecksum((BYTE*)&AppConfig, sizeof(AppConfig))))
			{
				// Check to ensure that the vNeedToSaveDefaults flag is zero, 
				// indicating that this is the first iteration through the do 
				// loop.  If we have already saved the defaults once and the 
				// EEPROM/Flash still doesn't pass the validity check, then it 
				// means we aren't successfully reading or writing to the 
				// EEPROM/Flash.  This means you have a hardware error and/or 
				// SPI configuration error.
				if (vNeedToSaveDefaults)
				{
					while(1);
				}

				// Set flag and restart loop to load ROM defaults and save them
				vNeedToSaveDefaults = 1;
				continue;
			}

			// If we get down here, it means the EEPROM/Flash has valid contents 
			// and either matches the ROM defaults or previously matched and 
			// was run-time reconfigured by the user.  In this case, we shall 
			// use the contents loaded from EEPROM/Flash.
			break;
		}
#endif
		break;
	}
}

#if defined(EEPROM_CS_TRIS) || defined(SPIFLASH_CS_TRIS)
void SaveAppConfig(const APP_CONFIG *ptrAppConfig)
{
	NVM_VALIDATION_STRUCT NVMValidationStruct;

	// Ensure adequate space has been reserved in non-volatile storage to 
	// store the entire AppConfig structure.  If you get stuck in this while(1) 
	// trap, it means you have a design time misconfiguration in TCPIPConfig.h.
	// You must increase MPFS_RESERVE_BLOCK to allocate more space.
#if defined(STACK_USE_MPFS2)
	if (sizeof(NVMValidationStruct) + sizeof(AppConfig) > MPFS_RESERVE_BLOCK)
		while(1);
#endif

	// Get proper values for the validation structure indicating that we can use 
	// these EEPROM/Flash contents on future boot ups
	NVMValidationStruct.wOriginalChecksum = wOriginalAppConfigChecksum;
	NVMValidationStruct.wCurrentChecksum = CalcIPChecksum((BYTE*)ptrAppConfig, sizeof(APP_CONFIG));
	NVMValidationStruct.wConfigurationLength = sizeof(APP_CONFIG);

	// Write the validation struct and current AppConfig contents to EEPROM/Flash
#if defined(EEPROM_CS_TRIS)
	XEEBeginWrite(0x0000);
	XEEWriteArray((BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
	XEEWriteArray((BYTE*)ptrAppConfig, sizeof(APP_CONFIG));
#else
	SPIFlashBeginWrite(0x0000);
	SPIFlashWriteArray((BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
	SPIFlashWriteArray((BYTE*)ptrAppConfig, sizeof(APP_CONFIG));
#endif
}
#endif


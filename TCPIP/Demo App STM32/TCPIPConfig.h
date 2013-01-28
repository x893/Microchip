#ifndef TCPIPCONFIG_STUB_H
#define TCPIPCONFIG_STUB_H

#if	defined( CFG_INCLUDE_MapleRET6_MRF24WB )	||	\
	defined( CFG_INCLUDE_STM32VLDISC_MRF24WB )
	#include "Configs/TCPIP STM32 MRF24WB.h"
#else
	#error "Missing project macro definition to select proper TCPIPConfig.h"
#endif

#endif

#ifndef __HARDWAREPROFILE_H__
#define __HARDWAREPROFILE_H__

#if defined( CFG_INCLUDE_MapleRET6_MRF24WB )

	// Maple RET6
	#define STM32F10X_HD
	#include "Configs/HWP STM32 MRF24WB.h"

#elif defined( CFG_INCLUDE_STM32VLDISC_MRF24WB )

	// STM32VL-DISCOVERY
	#define STM32F10X_MD
	#include "HWP STM32VL-DISC MRF24WB.h"

#else
	#error "No extended HWP...h included."
#endif

#endif

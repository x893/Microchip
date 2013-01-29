#ifndef __HARDWAREPROFILE_H__
#define __HARDWAREPROFILE_H__

#define __STM32F10X__

#if defined( CFG_INCLUDE_MAPLE_RET6 )

	// Maple RET6
	#define STM32F10X_HD
	#define HSE_VALUE 8000000UL
	#include "Configs/HWP MAPLE-R6 MRF24WB.h"

#elif defined( CFG_INCLUDE_STM32VL_DISCOVERY )

	// STM32VL-DISCOVERY
	#define STM32F10X_MD_VL
	#define HSE_VALUE 8000000UL
	#include "Configs/HWP STM32VL-DISC MRF24WB.h"

#else
	#error "No extended HWP...h included."
#endif

#endif

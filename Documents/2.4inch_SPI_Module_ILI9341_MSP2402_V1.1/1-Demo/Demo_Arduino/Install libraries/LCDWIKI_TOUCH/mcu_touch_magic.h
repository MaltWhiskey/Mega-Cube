#ifndef _mcu_touch_magic_h_
#define _mcu_touch_magic_h_

#define TCLK_LOW 		*tclkPort &= ~tclkPinSet
#define TCLK_HIGH   	*tclkPort |= tclkPinSet
#define TCS_LOW  		*tcsPort &= ~tcsPinSet
#define TCS_HIGH    	*tcsPort |= tcsPinSet
#define TDIN_LOW  		*tdinPort &= ~tdinPinSet
#define TDIN_HIGH		*tdinPort |= tdinPinSet
#define TDOUT_STATE 	((*tdoutPort) & tdoutPinSet)
#define TIRQ_STATE		((*tirqPort) & tirqPinSet)

#endif
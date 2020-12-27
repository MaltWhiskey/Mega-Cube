/* Teensyduino Core Library
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#if defined(__IMXRT1062__)
#include "FlexIO_t4.h"



//-----------------------------------------------------------------------------
// FlexIO Hardware structures
//-----------------------------------------------------------------------------
extern void IRQHandler_FlexIO1();
extern void IRQHandler_FlexIO2();
extern void IRQHandler_FlexIO3();

FlexIOHandlerCallback *flex1_Handler_callbacks[4] = {nullptr, nullptr, nullptr, nullptr};
FlexIOHandlerCallback *flex2_Handler_callbacks[4] = {nullptr, nullptr, nullptr, nullptr};
FlexIOHandlerCallback *flex3_Handler_callbacks[4] = {nullptr, nullptr, nullptr, nullptr};

//-----------------------------------------------------------------------------
// T4.1 board
//-----------------------------------------------------------------------------
#if defined(ARDUINO_TEENSY41)
const FlexIOHandler::FLEXIO_Hardware_t FlexIOHandler::flex1_hardware = {
	CCM_CCGR5, CCM_CCGR5_FLEXIO1(CCM_CCGR_ON),
	IRQ_FLEXIO1, 
	&IRQHandler_FlexIO1,
	DMAMUX_SOURCE_FLEXIO1_REQUEST0, DMAMUX_SOURCE_FLEXIO1_REQUEST1, DMAMUX_SOURCE_FLEXIO1_REQUEST2, DMAMUX_SOURCE_FLEXIO1_REQUEST3,
	2,       3,    4,    5,  33,    49,   50,   52,   54, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	4,       5,    6,    8,  7,     13,   14,   12,   15, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

const FlexIOHandler::FLEXIO_Hardware_t FlexIOHandler::flex2_hardware = {
	CCM_CCGR3, CCM_CCGR3_FLEXIO2(CCM_CCGR_ON),
	IRQ_FLEXIO2, 
	&IRQHandler_FlexIO2,
	DMAMUX_SOURCE_FLEXIO2_REQUEST0, DMAMUX_SOURCE_FLEXIO2_REQUEST1, DMAMUX_SOURCE_FLEXIO2_REQUEST2, DMAMUX_SOURCE_FLEXIO2_REQUEST3,
	6,       7,    8,    9,  10,    11,   12,   13,   32,   34,   35,   36,   37, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	10,     17,   16,   11,  0,      2,    1,    3,   12,   29,   28,   18,   19, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

const FlexIOHandler::FLEXIO_Hardware_t FlexIOHandler::flex3_hardware = {
	CCM_CCGR7, CCM_CCGR7_FLEXIO3(CCM_CCGR_ON),
	IRQ_FLEXIO3, 
	&IRQHandler_FlexIO3,
	0xff, 0xff, 0xff, 0xff,  // No DMA Sources? 
	7,       8,   14,   15,   16,   17,   18,   19,   20,  21,    22,   23,   26,   27,   34,   35,   36,   37,   38,   39,   40,   41, 
	17,     16,    2,    3,    7,    6,    1,    0,   10,   11,    8,    9,   14,   15,   29,   28,   18,   19,   12,   13,    4,    5, 
	0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 
};

//-----------------------------------------------------------------------------
// T4 board
//-----------------------------------------------------------------------------
#else
const FlexIOHandler::FLEXIO_Hardware_t FlexIOHandler::flex1_hardware = {
	CCM_CCGR5, CCM_CCGR5_FLEXIO1(CCM_CCGR_ON),
	IRQ_FLEXIO1, 
	&IRQHandler_FlexIO1,
	DMAMUX_SOURCE_FLEXIO1_REQUEST0, DMAMUX_SOURCE_FLEXIO1_REQUEST1, DMAMUX_SOURCE_FLEXIO1_REQUEST2, DMAMUX_SOURCE_FLEXIO1_REQUEST3,
	2,       3,    4,    5,  33,  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	4,       5,    6,    8,  7,   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x14, 0x14, 0x14, 0x14, 0x14, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

const FlexIOHandler::FLEXIO_Hardware_t FlexIOHandler::flex2_hardware = {
	CCM_CCGR3, CCM_CCGR3_FLEXIO2(CCM_CCGR_ON),
	IRQ_FLEXIO2, 
	&IRQHandler_FlexIO2,
	DMAMUX_SOURCE_FLEXIO2_REQUEST0, DMAMUX_SOURCE_FLEXIO2_REQUEST1, DMAMUX_SOURCE_FLEXIO2_REQUEST2, DMAMUX_SOURCE_FLEXIO2_REQUEST3,
	6,       7,    8,    9,  10,    11,   12,   13,   32, 0xff, 0xff, 0xff, 0xff, 0xff,
	10,     17,   16,   11,  0,      2,    1,    3,   12, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0xff, 0xff, 0xff, 0xff, 0xff,
};

const FlexIOHandler::FLEXIO_Hardware_t FlexIOHandler::flex3_hardware = {
	CCM_CCGR7, CCM_CCGR7_FLEXIO3(CCM_CCGR_ON),
	IRQ_FLEXIO3, 
	&IRQHandler_FlexIO3,
	0xff, 0xff, 0xff, 0xff,  // No DMA Sources? 
	7,       8,   14,   15,   16,   17,   18,   19,   20,  21,    22,   23,   26,   27,   
	17,     16,    2,    3,    7,    6,    1,    0,   10,   11,    8,    9,   14,   15,    
	0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19,
};
#endif

static FlexIOHandler flexIO1((uintptr_t)&IMXRT_FLEXIO1_S, (uintptr_t)&FlexIOHandler::flex1_hardware, (uintptr_t)flex1_Handler_callbacks);
static FlexIOHandler flexIO2((uintptr_t)&IMXRT_FLEXIO2_S, (uintptr_t)&FlexIOHandler::flex2_hardware, (uintptr_t)flex2_Handler_callbacks);
static FlexIOHandler flexIO3((uintptr_t)&IMXRT_FLEXIO3_S, (uintptr_t)&FlexIOHandler::flex3_hardware, (uintptr_t)flex3_Handler_callbacks);

FlexIOHandler *FlexIOHandler::flexIOHandler_list[] = {&flexIO1, &flexIO2, &flexIO3};


//-----------------------------------------------------------------------------
// Interrupt functions
//-----------------------------------------------------------------------------
void IRQHandler_FlexIO1() {
	FlexIOHandlerCallback **ppfhc = flex1_Handler_callbacks;
//	Serial.printf("FI1: %x %x %x ", FLEXIO1_SHIFTSTAT, FLEXIO1_SHIFTSIEN, FLEXIO1_SHIFTERR);
	for (uint8_t i = 0; i < 4; i++) {
		if (*ppfhc) {
			if ((*ppfhc)->call_back(&flexIO1)) return;
		}
		ppfhc++;
	}
	flexIO1.IRQHandler();
//	Serial.printf(" %x %x %x\n", FLEXIO1_SHIFTSTAT, FLEXIO1_SHIFTSIEN, FLEXIO1_SHIFTERR);
	 asm("dsb");
}


void IRQHandler_FlexIO2() {
	FlexIOHandlerCallback **ppfhc = flex2_Handler_callbacks;
	for (uint8_t i = 0; i < 4; i++) {
		if (*ppfhc) {
			if ((*ppfhc)->call_back(&flexIO2)) return;
		}
		ppfhc++;
	}
	flexIO2.IRQHandler();
	 asm("dsb");
}

void IRQHandler_FlexIO3() {
	FlexIOHandlerCallback **ppfhc = flex3_Handler_callbacks;
	for (uint8_t i = 0; i < 4; i++) {
		if (*ppfhc) {
			if ((*ppfhc)->call_back(&flexIO3)) return;
		}
		ppfhc++;
	}
	flexIO3.IRQHandler();
	 asm("dsb");
}

//-----------------------------------------------------------------------------
// Map IO pins to their Flex object and the flex pin 
//-----------------------------------------------------------------------------
FlexIOHandler *FlexIOHandler::mapIOPinToFlexIOHandler(uint8_t pin, uint8_t &flex_pin)
{
  FlexIOHandler *pflex = nullptr;

	for (uint8_t iflex = 0; iflex < (sizeof(flexIOHandler_list) / sizeof(flexIOHandler_list[0])); iflex++) {
		pflex = flexIOHandler_list[iflex];
		flex_pin = pflex->mapIOPinToFlexPin(pin);
		if (flex_pin != 0xff) {
			return pflex;
		}
	}
	return nullptr;
}

//-----------------------------------------------------------------------------
// Map IO pins to their Flex object and the flex pin 
//-----------------------------------------------------------------------------
uint8_t FlexIOHandler::mapIOPinToFlexPin(uint8_t pin)
{

  	// Now lets walk through all of the pins associated with this object. 
	for (uint8_t i = 0; i < CNT_FLEX_PINS; i++ ) {
  		if (hardware().io_pin[i] == pin) {
			#ifdef DEBUG_FlexIO
			Serial.println("Enable flexio clock");
			#endif
			hardware().clock_gate_register |= hardware().clock_gate_mask;

			return hardware().flex_pin[i];
		}
	}
	return 0xff;
}

//-----------------------------------------------------------------------------
// Set an IO pin into Flex Mode
//-----------------------------------------------------------------------------
bool FlexIOHandler::setIOPinToFlexMode(uint8_t pin) {
	for (uint8_t i = 0; i < CNT_FLEX_PINS; i++ ) {
		if (hardware().io_pin[i] == pin) {
			  *(portConfigRegister(pin)) = hardware().io_pin_mux[i];
			  return true;
		}
	}
	return false;
}

// return the index of the ojbect 1:->0 2:->1 later 3:->2
int FlexIOHandler::FlexIOIndex()	
{
	for (uint8_t iflex = 0; iflex < (sizeof(flexIOHandler_list) / sizeof(flexIOHandler_list[0])); iflex++) {
		if (flexIOHandler_list[iflex] == this) return iflex;
	}
	return -1;
}

//-----------------------------------------------------------------------------
// Request and release Timers and Shifters
//-----------------------------------------------------------------------------
// TODO: Get count of timers/shifters/buffers out of object...
// Also handle cnt > 1...
// Currently lets support 1 or 2.
uint8_t FlexIOHandler::requestTimers(uint8_t cnt) {
	uint8_t mask = (cnt == 1)? 0x1 : 0x3;
	for (uint8_t i = 0; i < (5-cnt); i++) {
		if (!(_used_timers & mask)) {
			_used_timers |= mask;
			return i;
		}
		mask <<= 1;
	}
	return 0xff;
}

uint8_t FlexIOHandler::requestShifter(uint8_t not_dma_channel) {
	uint8_t mask = 0x1 ;
	for (uint8_t i = 0; i < 4; i++) {
		if (!(_used_shifters & mask)) {
			if ((not_dma_channel == 0xff) || (hardware().shifters_dma_channel[i] != not_dma_channel)) {
	  			_used_shifters |= mask;
	  			return i;
	  		}
		}
		mask <<= 1;
	}
	return 0xff;
}

uint8_t FlexIOHandler::shiftersDMAChannel(uint8_t n) {
	if (n < 4) return hardware().shifters_dma_channel[n];
	return 0xff; 
}

bool FlexIOHandler::claimTimer(uint8_t timer) {
	uint8_t mask = 1<<timer;
	if (!(_used_timers & mask)) {
		_used_timers |= mask;
		return true;
	}
	return false;
}
bool FlexIOHandler::claimShifter(uint8_t shifter) {
	uint8_t mask = 1<<shifter;
	if (!(_used_shifters & mask)) {
		_used_shifters |= mask;
		return true;
	}
	return false;	
}

void FlexIOHandler::IRQHandler() {
  
}

void FlexIOHandler::freeTimers(uint8_t n, uint8_t cnt) {
	if (n == 0xff) return;	// don't free if we did not allocate
	uint8_t mask = (cnt == 1)? 0x1 : 0x3;
	for (;n < 0; n--) mask <<= 1;
	_used_timers &= ~mask;
}

void FlexIOHandler::freeShifter(uint8_t n) {
	if (n == 0xff) return;	// don't free if we did not allocate
	uint8_t mask = 0x1;
	for (;n < 0; n--) mask <<= 1;
	_used_shifters &= ~mask;
}

//-----------------------------------------------------------------------------
// Add a call back object to be called when the IRQ is called
//-----------------------------------------------------------------------------
bool FlexIOHandler::addIOHandlerCallback(FlexIOHandlerCallback *callback) {
	FlexIOHandlerCallback **pflex_handler = (FlexIOHandlerCallback**)_callback_list_addr;

	for (uint8_t i=0; i < 4; i++) {
		if (*pflex_handler == nullptr) {
			*pflex_handler = callback;
			// See if we need to enable the interrupt...
			if (!_irq_initialized) {
				attachInterruptVector(hardware().flex_irq, hardware().flex_isr);
				//NVIC_SET_PRIORITY(hardware().irq, hardware().irq_priority);	// maybe should put into hardware...
				NVIC_ENABLE_IRQ(hardware().flex_irq);
				_irq_initialized = true;
			}
			return true;
		}
		else if (*pflex_handler == callback) return true;	// don't need to add again... 
		pflex_handler++;	// look at next one. 
	}
	return false;
}

//-----------------------------------------------------------------------------
// Remove callbacks
//-----------------------------------------------------------------------------
bool FlexIOHandler::removeIOHandlerCallback(FlexIOHandlerCallback *callback) {
	FlexIOHandlerCallback **pflex_handler = (FlexIOHandlerCallback**)_callback_list_addr;

	for (uint8_t i=0; i < 4; i++) {
		if (*pflex_handler == callback) {
			*pflex_handler = nullptr;
			return true;
		}
		pflex_handler++;	// look at next one. 
	}
	return false;
}


//-----------------------------------------------------------------------------
// Compute the Flex IO clock rate. 
//-----------------------------------------------------------------------------
uint32_t FlexIOHandler::computeClockRate()  {
	// Todo: add all of this stuff into hardware()... 
	uint32_t pll_clock; // = 480000000U;	// Assume PPL3_SEL
	uint8_t clk_sel;
	uint8_t clk_pred;
	uint8_t clk_podf;
	if ((IMXRT_FLEXIO_t *)port_addr == &IMXRT_FLEXIO1_S) {
		// FlexIO1... 
		clk_sel = (CCM_CDCDR >> 7) & 0x3;
		clk_pred = (CCM_CDCDR >> 12) & 0x7;
		clk_podf = (CCM_CDCDR >> 9) & 0x7;
	} else {
		// FlexIO2... 
		clk_sel = (CCM_CSCMR2 >> 19) & 0x3;
		clk_pred = (CCM_CS1CDR >> 9) & 0x7;
		clk_podf = (CCM_CS1CDR >> 25) & 0x7;
	}
	// TODO - look at the actual clock select
	switch (clk_sel) {
		case 1: 
			pll_clock = 508240000U;
			break;
		case 3:
		default: 
			pll_clock = 480000000U;
			break;
	}
	return pll_clock / (uint32_t)((clk_pred+1) * (clk_podf+1));
}

//-----------------------------------------------------------------------------
// Try to set clock settings
//-----------------------------------------------------------------------------
void FlexIOHandler::setClockSettings(uint8_t clk_sel, uint8_t clk_pred, uint8_t clk_podf)  {
	// Todo: add all of this stuff into hardware()... 
	// warning this does no validation of the values passed in...
	if (clk_sel == 2) {
		// PLL4 - We may need to enable this clock!
		CCM_ANALOG_PLL_VIDEO_CLR = CCM_ANALOG_PLL_ARM_POWERDOWN | CCM_ANALOG_PLL_ARM_BYPASS;
		CCM_ANALOG_PLL_VIDEO_SET = CCM_ANALOG_PLL_ARM_ENABLE;
		Serial.printf("CCM_ANALOG_PLL_VIDEO: %x\n", CCM_ANALOG_PLL_VIDEO);
	}
	if ((IMXRT_FLEXIO_t *)port_addr == &IMXRT_FLEXIO1_S) {
		// FlexIO1... 
		// need to turn clock off...
		hardware().clock_gate_register &= ~hardware().clock_gate_mask;

		CCM_CDCDR = (CCM_CDCDR & ~(CCM_CDCDR_FLEXIO1_CLK_SEL(3) | CCM_CDCDR_FLEXIO1_CLK_PRED(7) | CCM_CDCDR_FLEXIO1_CLK_PODF(7))) 
			| CCM_CDCDR_FLEXIO1_CLK_SEL(clk_sel) | CCM_CDCDR_FLEXIO1_CLK_PRED(clk_pred) | CCM_CDCDR_FLEXIO1_CLK_PODF(clk_podf);

		// turn clock back on
		hardware().clock_gate_register |= hardware().clock_gate_mask;
	} else {
		// FlexIO2... 
		// need to turn clock off...
		hardware().clock_gate_register &= ~hardware().clock_gate_mask;

		CCM_CSCMR2 = (CCM_CSCMR2 & ~(CCM_CSCMR2_FLEXIO2_CLK_SEL(3))) | CCM_CSCMR2_FLEXIO2_CLK_SEL(clk_sel);
		CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_FLEXIO2_CLK_PRED(7)|CCM_CS1CDR_FLEXIO2_CLK_PODF(7)) )
			| CCM_CS1CDR_FLEXIO2_CLK_PRED(clk_pred) | CCM_CS1CDR_FLEXIO2_CLK_PODF(clk_podf);

		// turn clock back on
		hardware().clock_gate_register |= hardware().clock_gate_mask;
	}
}
#endif

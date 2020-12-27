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

//#define DEBUG_FlexIO

#ifndef _FLEX_IO_T4_H_
#define _FLEX_IO_T4_H_
#include <Arduino.h>
#if !defined(__IMXRT1062__)
#error "Sorry, Flex IO only works on Teensy 4.x boards"
#endif

// forward reference
class FlexIOHandler; 

class FlexIOHandlerCallback {
public:
	virtual bool call_back (FlexIOHandler *pflex) = 0;
};

class FlexIOHandler {
public:
	static const uint8_t CNT_SHIFTERS = 4;
#if defined(ARDUINO_TEENSY41)
	static const uint8_t CNT_FLEX_PINS = 22;
	static const uint8_t CNT_FLEX_IO_OBJECT = 3;
#else
	// T4
	static const uint8_t CNT_FLEX_PINS = 14;
	static const uint8_t CNT_FLEX_IO_OBJECT = 3;
#endif
	typedef struct {
		volatile uint32_t &clock_gate_register;
		const uint32_t clock_gate_mask;
		const IRQ_NUMBER_t  flex_irq;
		void    (*flex_isr)();
		const uint8_t  shifters_dma_channel[CNT_SHIFTERS];
		const uint8_t  io_pin[CNT_FLEX_PINS];
		const uint8_t  flex_pin[CNT_FLEX_PINS];
		const uint8_t  io_pin_mux[CNT_FLEX_PINS];
	} FLEXIO_Hardware_t;

	static const FLEXIO_Hardware_t flex1_hardware;
	static const FLEXIO_Hardware_t flex2_hardware;
	static const FLEXIO_Hardware_t flex3_hardware;

  constexpr FlexIOHandler(uintptr_t myport, uintptr_t myhardware, uintptr_t callback_list)
  	: port_addr(myport), hardware_addr(myhardware), _callback_list_addr(callback_list) {
  }

  	// A static one that can map across all FlexIO controller
    static FlexIOHandler *mapIOPinToFlexIOHandler(uint8_t pin, uint8_t &flex_pin);
	static FlexIOHandler *flexIOHandler_list[3];

    // A simple one that maps within a controller
  	uint8_t mapIOPinToFlexPin(uint8_t);

	IMXRT_FLEXIO_t & port() { return *(IMXRT_FLEXIO_t *)port_addr; }
	const FLEXIO_Hardware_t & hardware() { return *(const FLEXIO_Hardware_t *)hardware_addr; }
	int FlexIOIndex();		// return the index of the ojbect 1:->0 2:->1 later 3:->2

	uint8_t requestTimers(uint8_t cnt=1);
	uint8_t requestShifter(uint8_t not_dma_channel=0xff);
	uint8_t shiftersDMAChannel(uint8_t n);
	bool claimTimer(uint8_t timer);
	bool claimShifter(uint8_t shifter);   

	void freeTimers(uint8_t n, uint8_t cnt=1);
	void freeShifter(uint8_t n);

	bool setIOPinToFlexMode(uint8_t pin);
	bool addIOHandlerCallback(FlexIOHandlerCallback *callback);
	bool removeIOHandlerCallback(FlexIOHandlerCallback *callback);

	uint32_t computeClockRate();

	// clksel(0-3PLL4, Pll3 PFD2 PLL5, *PLL3_sw)
	// clk_pred(0, 1, 2, 7) - divide (n+1)
	// clk_podf(0, *7) divide (n+1)
	// So default is 480mhz/16
	void setClockSettings(uint8_t clk_sel, uint8_t clk_pred, uint8_t clk_podf);

	void IRQHandler(void);

protected: 
	uintptr_t 		port_addr;
	uintptr_t		 hardware_addr;
	uintptr_t		_callback_list_addr;
	uint8_t         _used_timers = 0;
	uint8_t         _used_shifters = 0;
	bool			_irq_initialized = false;
  
};

#endif //_FLEX_IO_T4_H_
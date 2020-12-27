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
#include <Arduino.h>
#include "FlexIO_t4.h"
#ifndef _FLEX_SERIAL_H_
#define _FLEX_SERIAL_H_



class FlexSerial : public Stream, public FlexIOHandlerCallback
{
public:
	enum {TX_BUFFER_SIZE=64, RX_BUFFER_SIZE=40};
	FlexSerial(int rxPin=-1, int txPin=-1, 
			uint8_t rx_flex=0xff, uint8_t rx_timer=0xff, uint8_t rx_shifter=0xff, 
			uint8_t tx_flex=0xff, uint8_t tx_timer=0xff, uint8_t tx_shifter=0xff) :
		_rxPin(rxPin), _txPin(txPin), 
		_rx_timer(rx_timer), _rx_shifter(rx_shifter), _tx_timer(tx_timer), _tx_shifter(tx_shifter)
	{
		if (rx_flex < (uint8_t)(sizeof(FlexIOHandler::flexIOHandler_list)/sizeof(FlexIOHandler::flexIOHandler_list[0])))
			_rx_pflex = FlexIOHandler::flexIOHandler_list[rx_flex];
		if (tx_flex < (uint8_t)(sizeof(FlexIOHandler::flexIOHandler_list)/sizeof(FlexIOHandler::flexIOHandler_list[0])))
			_tx_pflex = FlexIOHandler::flexIOHandler_list[tx_flex];
	}	 
	

	~FlexSerial() { end(); }
	bool begin(uint32_t baud, bool inverse_logic = false);
	void end(void);

	virtual int available(void);
	virtual int peek(void);
	virtual void flush(void);
	virtual size_t write(uint8_t c);
	virtual int read(void);
	using Print::write;

	void clear(void);
	int availableForWrite(void);

	// Call back from flexIO when ISR hapens
	virtual bool call_back (FlexIOHandler *pflex);
	FlexIOHandler  *flexIOHandlerTX() {return _tx_pflex;}
	FlexIOHandler  *flexIOHandlerRX() {return _rx_pflex;}



private:
	int _rxPin;
	int _txPin;

	// others that get passed through constructor
	uint8_t 			_rx_timer;
	uint8_t 			_rx_shifter;

	uint8_t 			_tx_timer;
	uint8_t 			_tx_shifter;

	// Variables for tranmitter
	FlexIOHandler *_tx_pflex = nullptr;

	uint8_t 		_tx_flex_pin;
	uint8_t 		_tx_shifter_mask;
	uint8_t			_tx_buffer[TX_BUFFER_SIZE];
	uint16_t		_tx_buffer_head = 0;
	uint16_t 		_tx_buffer_tail = 0;
	volatile uint8_t	_transmitting = 0;
	uint8_t			_tx_timer_mask = 0;
	static const uint32_t FLUSH_TIMEOUT = 1000;	// 1 second... 

	// Variables for receiver
	FlexIOHandler *_rx_pflex = nullptr;
	uint8_t 		_rx_flex_pin;
	uint8_t 		_rx_shifter_mask;
	uint8_t			_rx_buffer[RX_BUFFER_SIZE];
	uint16_t		_rx_buffer_head = 0;
	uint16_t 		_rx_buffer_tail = 0;


};
#endif //_FLEX_SERIAL_H_
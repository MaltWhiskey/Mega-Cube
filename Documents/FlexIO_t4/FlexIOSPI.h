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
#include <DMAChannel.h>
#include <EventResponder.h>

#ifndef _FLEXIO_SPI_H_
#define _FLEXIO_SPI_H_

#ifndef LSBFIRST
#define LSBFIRST 0
#endif
#ifndef MSBFIRST
#define MSBFIRST 1
#endif

#ifndef SPI_MODE0
#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C
#endif
#define SPI_MODE_TRANSMIT_ONLY 0x80 	// Hack to allow higher speeds when transmit only 

// Pretty stupid settings for now...
class FlexIOSPISettings {
public:
	FlexIOSPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) : _clock(clock), 
		_bitOrder(bitOrder), _dataMode(dataMode) {};

	uint32_t _clock;
	uint8_t _bitOrder;
	uint8_t	_dataMode;
};

class FlexIOSPI : public FlexIOHandlerCallback
{
public:
	enum {TX_BUFFER_SIZE=64, RX_BUFFER_SIZE=40};
	FlexIOSPI(int mosiPin, int misoPin, int sckPin,  int csPin=-1) :
		_mosiPin(mosiPin), _sckPin(sckPin), _misoPin(misoPin), _csPin(csPin) {};

	~FlexIOSPI() { end(); }
	bool begin();
	void end(void);

	uint8_t transfer(uint8_t b);	// transfer one byte
	uint16_t transfer16(uint16_t w);

	void inline transfer(void *buf, size_t count) {transfer(buf, buf, count);}
	void setTransferWriteFill(uint8_t ch ) {_transferWriteFill = ch;}
	void transfer(const void * buf, void * retbuf, size_t count);

	// Asynch support (DMA )
	bool transfer(const void *txBuffer, void *rxBuffer, size_t count,  EventResponderRef  event_responder);

	static void _dma_rxISR0(void);
	static void _dma_rxISR1(void);
	inline void dma_rxisr(void);

	void beginTransaction(FlexIOSPISettings settings);
	void endTransaction(void);

	// have ability to retrieve the FLEXIO object
	FlexIOHandler  *flexIOHandler() {return _pflex;}

	// Call back from flexIO when ISR hapens
	virtual bool call_back (FlexIOHandler *pflex);


private:
	int _mosiPin;
	int _sckPin;
	int _misoPin;
	int _csPin;

	uint8_t			_transferWriteFill = 0;
	uint8_t			_in_transaction_flag = 0;

	uint32_t 		_clock = 0;
	uint8_t 		_bitOrder = MSBFIRST;
	uint8_t			_dataMode = SPI_MODE0;
	volatile uint32_t *_shiftBufInReg = nullptr;
	volatile uint32_t *_shiftBufOutReg = nullptr;


	// FlexIO variables.
	FlexIOHandler  *_pflex = nullptr;
	uint8_t 		_mosi_flex_pin = 0xff;
	uint8_t 		_sck_flex_pin = 0xff;
	uint8_t 		_miso_flex_pin = 0xff;
	uint8_t 		_cs_flex_pin = 0xff;
	uint8_t 		_timer = 0xff;
	uint8_t 		_tx_shifter = 0xff;
	uint8_t 		_tx_shifter_mask = 0;
	uint8_t 		_rx_shifter = 0xff;
	uint8_t 		_rx_shifter_mask = 0;

	// DMA - Async support
	bool initDMAChannels();
	enum DMAState { notAllocated, idle, active, completed};
	enum {MAX_DMA_COUNT=32767};
	DMAState     	_dma_state = DMAState::notAllocated;
	uint32_t		_dma_count_remaining = 0;	// How many bytes left to output after current DMA completes
	DMAChannel   	*_dmaTX = nullptr;
	DMAChannel    	*_dmaRX = nullptr;
	EventResponder *_dma_event_responder = nullptr;
	static FlexIOSPI  *_dmaActiveObjects[FlexIOHandler::CNT_FLEX_IO_OBJECT];
};
#endif //_FLEX_SPI_H_
#include "FlexSerial.h"
#define BAUDRATE 115200
#define FLEXIO1_CLOCK (480000000L/16) // Again assuming default clocks?

//#define DEBUG_FlexSerial
//#define DEBUG_FlexSerial_CALL_BACK
//#define DEBUG_digitalWriteFast(pin, state) digitalWriteFast(pin, state)
//#define DEBUG_digitalToggleFast(pin)	digitalWriteFast(pin, !digitalReadFast(pin));

#define DEBUG_digitalWriteFast(pin, state) 
#define DEBUG_digitalToggleFast(pin)
#define DEBUG_PIN_CALLBACK 30
#define DEBUG_PIN_CALLBACK_READ 31
#define DEBUG_PIN_CALLBACK_WRITE 32
#define DEBUG_PIN_WRITE_TIMER_INT 29


//=============================================================================
// FlexSerial::Begin
//=============================================================================
bool FlexSerial::begin(uint32_t baud, bool inverse_logic) {
	// BUGBUG - may need to actual Clocks to computer baud...
	uint16_t baud_div =  (FLEXIO1_CLOCK/baud)/2 - 1;                                   
	//-------------------------------------------------------------------------
	// TX Pin setup - if requested
	//-------------------------------------------------------------------------
	if (_txPin != -1) {
		if (_tx_pflex != nullptr)  {
			_tx_flex_pin = _tx_pflex->mapIOPinToFlexPin(_txPin);
			if (_tx_flex_pin == 0xff) {
				Serial.printf("FlexSerial - Failed to map TX pin %d to FlexIO\n", _txPin);
				return false;
			}
		} else {
			_tx_pflex = FlexIOHandler::mapIOPinToFlexIOHandler(_txPin, _tx_flex_pin);
			if (_tx_pflex == nullptr) {
				Serial.printf("FlexSerial - Failed to map TX pin %d to FlexIO\n", _txPin);
				return false;
			}

		}
		// BUGBUG need to handle restarts...
		IMXRT_FLEXIO_t *p = &_tx_pflex->port();
#ifdef DEBUG_FlexSerial
		Serial.printf("pin %d maps to: %x, port: %x", _txPin, (uint32_t)_tx_pflex, (uint32_t)p);
		if (p == &IMXRT_FLEXIO1_S) Serial.print("(FLEXIO1)");
		else if (p == &IMXRT_FLEXIO2_S) Serial.print("(FLEXIO2)");
		else if (p == &IMXRT_FLEXIO3_S) Serial.print("(FLEXIO3)");
		Serial.printf(" pin %u\n", _tx_flex_pin);  Serial.flush();
#endif		
		if (_tx_timer <= 3) {
			if (!_tx_pflex->claimTimer(_tx_timer)) {
				Serial.printf("FlexSerial - Failed to claim TX timer(%d)\n", _tx_timer);
				return false;
			}
		} else {
			_tx_timer = _tx_pflex->requestTimers();
			if (_tx_timer == 0xff) {
				Serial.printf("FlexSerial - Failed to allocate TX timer(%d)\n", _tx_timer);
				return false;
			}
		}
		if (_tx_shifter <= 3) {
			if (!_tx_pflex->claimShifter(_tx_shifter)) {
				Serial.printf("FlexSerial - Failed to claim TX shifter(%d)\n", _tx_shifter);
				return false;
			}
		} else {
			_tx_shifter = _tx_pflex->requestShifter();
			if (_tx_shifter == 0xff) {
				Serial.printf("FlexSerial - Failed to allocate TX shifter(%d)\n", _tx_shifter);
				return false;
			}
		}


		_tx_shifter_mask = 1 << _tx_shifter;
		_tx_timer_mask = 1 << _tx_timer;

#ifdef DEBUG_FlexSerial
		Serial.printf("timer index: %d shifter index: %d mask: %x\n", _tx_timer, _tx_shifter, _tx_shifter_mask);
		// lets try to configure a tranmitter like example
		Serial.println("Before configure flexio");
#endif
		p->SHIFTCFG[_tx_shifter] = FLEXIO_SHIFTCFG_SSTOP(3) | FLEXIO_SHIFTCFG_SSTART(2); //0x0000_0032;
		p->SHIFTCTL[_tx_shifter] = FLEXIO_SHIFTCTL_PINCFG(3) | FLEXIO_SHIFTCTL_SMOD(2) |
		                              FLEXIO_SHIFTCTL_TIMSEL(_tx_timer) | FLEXIO_SHIFTCTL_PINSEL(_tx_flex_pin); // 0x0003_0002;
		p->TIMCMP[_tx_timer] = 0xf00 | baud_div; //0xF01; //0x0000_0F01;		//
		p->TIMCFG[_tx_timer] = FLEXIO_TIMCFG_TSTART | FLEXIO_TIMCFG_TSTOP(2) |
		                          FLEXIO_TIMCFG_TIMENA(2) |  FLEXIO_TIMCFG_TIMDIS(2); //0x0000_2222;
		p->TIMCTL[_tx_timer] = FLEXIO_TIMCTL_TIMOD(1) | FLEXIO_TIMCTL_TRGPOL | FLEXIO_TIMCTL_TRGSRC
		                          | FLEXIO_TIMCTL_TRGSEL(4*_tx_shifter + 1) | FLEXIO_TIMCTL_PINSEL(_tx_flex_pin);  // 0x01C0_0001;

		__disable_irq();
		p->CTRL = FLEXIO_CTRL_FLEXEN;
		//p->SHIFTSTAT = _tx_shifter_mask;   // Clear out the status. Maybe causes NULL char to output?
		p->SHIFTSIEN &= ~_tx_shifter_mask;  // disable interrupt on this one...
		__enable_irq();

		// Set the IO pin into FLEXIO mode
		_tx_pflex->setIOPinToFlexMode(_txPin);
		_tx_pflex->addIOHandlerCallback(this);
		// Lets print out some of the settings and the like to get idea of state
		#ifdef DEBUG_FlexSerial
		Serial.printf("CCM_CDCDR: %x\n", CCM_CDCDR);
		Serial.printf("VERID:%x PARAM:%x CTRL:%x PIN: %x\n", p->VERID, p->PARAM, p->CTRL, p->PIN);
		Serial.printf("SHIFTSTAT:%x SHIFTERR=%x TIMSTAT=%x\n", p->SHIFTSTAT, p->SHIFTERR, p->TIMSTAT);
		Serial.printf("SHIFTSIEN:%x SHIFTEIEN=%x TIMIEN=%x\n", p->SHIFTSIEN, p->SHIFTEIEN, p->TIMIEN);
		Serial.printf("SHIFTSDEN:%x SHIFTSTATE=%x\n", p->SHIFTSDEN, p->SHIFTSTATE);
		Serial.printf("SHIFTCTL:%x %x %x %x\n", p->SHIFTCTL[0], p->SHIFTCTL[1], p->SHIFTCTL[2], p->SHIFTCTL[3]);
		Serial.printf("SHIFTCFG:%x %x %x %x\n", p->SHIFTCFG[0], p->SHIFTCFG[1], p->SHIFTCFG[2], p->SHIFTCFG[3]);
		Serial.printf("TIMCTL:%x %x %x %x\n", p->TIMCTL[0], p->TIMCTL[1], p->TIMCTL[2], p->TIMCTL[3]);
		Serial.printf("TIMCFG:%x %x %x %x\n", p->TIMCFG[0], p->TIMCFG[1], p->TIMCFG[2], p->TIMCFG[3]);
		Serial.printf("TIMCMP:%x %x %x %x\n", p->TIMCMP[0], p->TIMCMP[1], p->TIMCMP[2], p->TIMCMP[3]);
		#else
		// There is some timing issue with these, that the prints solved.  This appears to work as well.
	   	delay(1);

		#endif
	}

	//-------------------------------------------------------------------------
	// RX Pin setup - if requested
	//-------------------------------------------------------------------------
	if (_rxPin != -1) {
		if (_rx_pflex != nullptr)  {
			_rx_flex_pin = _rx_pflex->mapIOPinToFlexPin(_rxPin);
			if (_rx_flex_pin == 0xff) {
				Serial.printf("FlexSerial - Failed to map RX pin %d to FlexIO\n", _rxPin);
				return false;
			}
		} else {
			_rx_pflex = FlexIOHandler::mapIOPinToFlexIOHandler(_rxPin, _rx_flex_pin);
			if (_rx_pflex == nullptr) {
				Serial.printf("FlexSerial - Failed to map RX pin %d to FlexIO\n", _rxPin);
				return false;
			}

		}

		IMXRT_FLEXIO_t *p = &_rx_pflex->port();
#ifdef DEBUG_FlexSerial
		Serial.printf("pin %d maps to: %x, port: %x", _rxPin, (uint32_t)_rx_pflex, (uint32_t)p);
		if (p == &IMXRT_FLEXIO1_S) Serial.print("(FLEXIO1)");
		else if (p == &IMXRT_FLEXIO2_S) Serial.print("(FLEXIO2)");
		else if (p == &IMXRT_FLEXIO3_S) Serial.print("(FLEXIO3)");
		Serial.printf(" pin %x\n", _rx_flex_pin);
#endif		
		if (_rx_timer <= 3) {
			if (!_rx_pflex->claimTimer(_rx_timer)) {
				Serial.printf("FlexSerial - Failed to claim RX timer(%d)\n", _rx_timer);
				return false;
			}
		} else {
			_rx_timer = _rx_pflex->requestTimers();
			if (_rx_timer == 0xff) {
				Serial.printf("FlexSerial - Failed to allocate RX timer(%d)\n", _rx_timer);
				return false;
			}
		}
		if (_rx_shifter <= 3) {
			if (!_rx_pflex->claimShifter(_rx_shifter)) {
				Serial.printf("FlexSerial - Failed to claim RX shifter(%d)\n", _rx_shifter);
				return false;
			}
		} else {
			uint8_t dma_channel_to_avoid = 0xff;
			if (_tx_pflex == _rx_pflex) dma_channel_to_avoid = _tx_pflex->shiftersDMAChannel(_tx_shifter);

			_rx_shifter = _rx_pflex->requestShifter(dma_channel_to_avoid);
			if (_rx_shifter == 0xff) {
				Serial.printf("FlexSerial - Failed to allocate RX shifter(%d)\n", _rx_shifter);
				return false;
			}
		}
		_rx_shifter_mask = 1 << _rx_shifter;

#ifdef DEBUG_FlexSerial
		Serial.printf("timer index: %d shifter index: %d mask: %x\n", _rx_timer, _rx_shifter, _rx_shifter_mask);
#endif
		// lets try to configure a receiver like example
		Serial.println("Before configure flexio");
		p->SHIFTCFG[_rx_shifter] = FLEXIO_SHIFTCFG_SSTOP(3) | FLEXIO_SHIFTCFG_SSTART(2); //0x0000_0032;
		p->SHIFTCTL[_rx_shifter] = FLEXIO_SHIFTCTL_TIMPOL | FLEXIO_SHIFTCTL_SMOD(1) |
		                              FLEXIO_SHIFTCTL_TIMSEL(_rx_timer) | FLEXIO_SHIFTCTL_PINSEL(_rx_flex_pin); // 0x0080_0001;

		p->TIMCMP[_rx_timer] = 0xf00 | baud_div; //0xF01; //0x0000_0F01;		//
		p->TIMCFG[_rx_timer] = FLEXIO_TIMCFG_TSTART | FLEXIO_TIMCFG_TSTOP(2) |
		                          FLEXIO_TIMCFG_TIMENA(4) | FLEXIO_TIMCFG_TIMDIS(2) |
		                          FLEXIO_TIMCFG_TIMRST(4) | FLEXIO_TIMCFG_TIMOUT(2); //0x204_2422

		p->TIMCTL[_rx_timer] = FLEXIO_TIMCTL_TIMOD(1) | FLEXIO_TIMCTL_PINPOL | FLEXIO_TIMCTL_PINSEL(_rx_flex_pin);;  // 0x0000_0081;
		p->CTRL = FLEXIO_CTRL_FLEXEN;									// make sure it is enabled. 
		//p->SHIFTSTAT = _rx_shifter_mask;   // Clear out the status.

		// Set the IO pin into FLEXIO mode
		_rx_pflex->setIOPinToFlexMode(_rxPin);
		_rx_pflex->addIOHandlerCallback(this);
		__disable_irq();
		p->SHIFTSIEN |= _rx_shifter_mask;  // enable interrupt on this one...

		__enable_irq();
		// Lets print out some of the settings and the like to get idea of state
#ifdef DEBUG_FlexSerial
		Serial.printf("CCM_CDCDR: %x\n", CCM_CDCDR);
		Serial.printf("VERID:%x PARAM:%x CTRL:%x PIN: %x\n", p->VERID, p->PARAM, p->CTRL, p->PIN);
		Serial.printf("SHIFTSTAT:%x SHIFTERR=%x TIMSTAT=%x\n", p->SHIFTSTAT, p->SHIFTERR, p->TIMSTAT);
		Serial.printf("SHIFTSIEN:%x SHIFTEIEN=%x TIMIEN=%x\n", p->SHIFTSIEN, p->SHIFTEIEN, p->TIMIEN);
		Serial.printf("SHIFTSDEN:%x SHIFTSTATE=%x\n", p->SHIFTSDEN, p->SHIFTSTATE);
		Serial.printf("SHIFTCTL:%x %x %x %x\n", p->SHIFTCTL[0], p->SHIFTCTL[1], p->SHIFTCTL[2], p->SHIFTCTL[3]);
		Serial.printf("SHIFTCFG:%x %x %x %x\n", p->SHIFTCFG[0], p->SHIFTCFG[1], p->SHIFTCFG[2], p->SHIFTCFG[3]);
		Serial.printf("TIMCTL:%x %x %x %x\n", p->TIMCTL[0], p->TIMCTL[1], p->TIMCTL[2], p->TIMCTL[3]);
		Serial.printf("TIMCFG:%x %x %x %x\n", p->TIMCFG[0], p->TIMCFG[1], p->TIMCFG[2], p->TIMCFG[3]);
		Serial.printf("TIMCMP:%x %x %x %x\n", p->TIMCMP[0], p->TIMCMP[1], p->TIMCMP[2], p->TIMCMP[3]);
#endif
	}

	return true;
}

void FlexSerial::end(void) {
	// If the transmit was allocated free it now as well as timers and shifters.
	if (_tx_pflex) {
		_tx_pflex->freeTimers(_tx_timer);
		_tx_timer = 0xff;
		_tx_pflex->freeShifter(_tx_shifter);
		_tx_shifter = 0xff;

		_tx_pflex->removeIOHandlerCallback(this);
		_tx_pflex = nullptr;	
	}

}


void FlexSerial::flush(void) {
	// I know this is not fully correct yet...
	// May need to do one extra call back in ISR (at least)
	uint32_t start_time = millis();
	while (_transmitting) {
		if ((millis()-start_time) > FLUSH_TIMEOUT) {
			Serial.println("*** FlexSerial::flush Timeout ***");
			return;
		}
		yield(); // wait
	}
}

size_t FlexSerial::write(uint8_t c) {
	uint32_t head;
	head = _tx_buffer_head;
	if (++head >= TX_BUFFER_SIZE) head = 0;
	while (_tx_buffer_tail == head) {
		/*
		int priority = nvic_execution_priority();
		if (priority <= hardware->irq_priority) {
			if ((port->STAT & LPUART_STAT_TDRE)) {
				uint32_t tail = _tx_buffer_tail;
				if (++tail >= TX_BUFFER_SIZE) tail = 0;
				if (tail < tx_buffer_size_) {
					n = tx_buffer_[tail];
				} else {
					n = tx_buffer_storage_[tail-tx_buffer_size_];
				}
				port->DATA  = n;
				_tx_buffer_tail = tail;
			}
		} else if (priority >= 256) 
		*/
		{
			yield(); // wait
		} 
	}
	//digitalWrite(5, LOW);
	//Serial.printf("WR %x %d %d %d %x %x\n", c, head, tx_buffer_size_,  TX_BUFFER_SIZE, (uint32_t)tx_buffer_, (uint32_t)tx_buffer_storage_);
	_tx_buffer[_tx_buffer_head] = c;
	__disable_irq();
	_transmitting = 1;
	_tx_buffer_head = head;
	_tx_pflex->port().SHIFTSIEN |= _tx_shifter_mask;  // enable interrupt on this one...
	_tx_pflex->port().TIMIEN &= ~_tx_timer_mask;	// Remove any timer interrupts
	_tx_pflex->port().TIMSTAT = _tx_timer_mask;  // Clear the state. 

	__enable_irq();
	//digitalWrite(3, LOW);
	return 1;
/*

	IMXRT_FLEXIO_t *p = &_tx_pflex->port();

	while (!(p->SHIFTSTAT & _tx_shifter_mask)) ; // wait until it says there is room for next output
	p->SHIFTBUF[_tx_shifter] = c;  // put the next byte out
	return 1;
*/
}

int FlexSerial::available(void) {
	uint32_t head, tail;

	head = _rx_buffer_head;
	tail = _rx_buffer_tail;
	if (head >= tail) return RX_BUFFER_SIZE - 1 - head + tail;
	return tail - head - 1;
}

int FlexSerial::peek(void) {
	if (_rx_buffer_head == _rx_buffer_tail) return -1;
	return _rx_buffer[_tx_buffer_tail] ;
}

int FlexSerial::read(void) {
	int return_value = -1;
	if (_rx_buffer_head != _rx_buffer_tail) {
		return_value = _rx_buffer[_rx_buffer_tail++] ;
		if (_rx_buffer_tail >= RX_BUFFER_SIZE) 
			_rx_buffer_tail = 0;
	}
	return return_value;
}

void FlexSerial::clear(void) {
	//rx_buffer_head_ = rx_buffer_tail_;
}

int FlexSerial::availableForWrite(void) {
	uint32_t head, tail;

	head = _tx_buffer_head;
	tail = _tx_buffer_tail;
	if (head >= tail) return TX_BUFFER_SIZE - 1 - head + tail;
	return tail - head - 1;
}


bool FlexSerial::call_back (FlexIOHandler *pflex) {
	DEBUG_digitalWriteFast(DEBUG_PIN_CALLBACK, HIGH);
	// check for RX
	IMXRT_FLEXIO_t *p = &pflex->port();
	if (pflex == _rx_pflex) {
		if (_rx_pflex->port().SHIFTSTAT & _rx_shifter_mask) {
			DEBUG_digitalWriteFast(DEBUG_PIN_CALLBACK_READ, HIGH);
			uint8_t c = _rx_pflex->port().SHIFTBUFBYS[_rx_shifter] & 0xff;
			uint32_t head;
			head = _rx_buffer_head;
			if (++head >= RX_BUFFER_SIZE) head = 0;
			// don't save char if buffer is full...
			if (_tx_buffer_tail != head) {
				_rx_buffer[_rx_buffer_head] = c;
				_rx_buffer_head = head;
			}
			DEBUG_digitalWriteFast(DEBUG_PIN_CALLBACK_READ, LOW);
		}
	}

	// See if we we have a TX event
	if (pflex == _tx_pflex) {
		if ((p->SHIFTSTAT & _tx_shifter_mask) && (p->SHIFTSIEN & _tx_shifter_mask)) {
			#ifdef DEBUG_FlexSerial_CALL_BACK
			if (p == &IMXRT_FLEXIO1_S) Serial.print(_txPin, DEC);
			#endif
			DEBUG_digitalWriteFast(DEBUG_PIN_CALLBACK_WRITE, HIGH);
			if (_tx_buffer_head != _tx_buffer_tail) {
				#ifdef DEBUG_FlexSerial_CALL_BACK
				if (p == &IMXRT_FLEXIO1_S) {
					if ((_tx_buffer[_tx_buffer_tail] >= ' ') &&	(_tx_buffer[_tx_buffer_tail] <= '~'))
						Serial.printf("(%c)", _tx_buffer[_tx_buffer_tail]);
					else
						Serial.printf("(%02x)", _tx_buffer[_tx_buffer_tail]);

				}
				#endif
				p->SHIFTBUF[_tx_shifter] = _tx_buffer[_tx_buffer_tail++] ;
				if (_tx_buffer_tail >= TX_BUFFER_SIZE) _tx_buffer_tail = 0;
			}
			if (_tx_buffer_head == _tx_buffer_tail) {
				__disable_irq();
				p->SHIFTSIEN &= ~_tx_shifter_mask;  // disable interrupt on this one...
				// BUGBUG: This is not the right place for this!
				p->TIMIEN |= _tx_timer_mask;	// Try turning on Timer error...
				p->TIMSTAT = _tx_timer_mask;  // Clear the state. 
				__enable_irq();
				#ifdef DEBUG_FlexSerial_CALL_BACK
				if (p == &IMXRT_FLEXIO1_S) Serial.print("*");
				#endif
			}
			DEBUG_digitalWriteFast(DEBUG_PIN_CALLBACK_WRITE, LOW);
		} 
		else if (p->TIMIEN & p->TIMSTAT & _tx_timer_mask) {
			DEBUG_digitalToggleFast(DEBUG_PIN_WRITE_TIMER_INT);
			__disable_irq();
			if (_transmitting >= 2) {
				// We wait for an extr timer interrupt 
				p->TIMIEN &= ~_tx_timer_mask; // turn it off
				_transmitting = 0;
			} else {
				_transmitting++;	
			}
			p->TIMSTAT = _tx_timer_mask;  // Clear the state. 
			__enable_irq();
		}
		// Check for error condition
		if (p->SHIFTERR & _tx_shifter_mask) {
			#ifdef DEBUG_FlexSerial_CALL_BACK
			if (p == &IMXRT_FLEXIO1_S) Serial.printf("%u$", _txPin);
			#endif
			__disable_irq();
			p->SHIFTERR = _tx_shifter_mask;  // Clear the error condition
			p->SHIFTSTAT = _tx_shifter_mask;  // Clear the state value
			if (_tx_buffer_head == _tx_buffer_tail) p->SHIFTSIEN &= ~_tx_shifter_mask;
			__enable_irq();
		}
	}
	DEBUG_digitalWriteFast(DEBUG_PIN_CALLBACK, LOW);
   asm("dsb");  // not sure if this will here or not. 

	return false;  // right now always return false... 
}

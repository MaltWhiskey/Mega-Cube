#ifndef _mcu_spi_magic_
#define _mcu_spi_magic_

//#define write8(d) {Spi_Write(d);}
#define write8(d) Spi_Write(d)
#define read8(dst) { dst=Spi_Read();}
#define setWriteDir() 
#define setReadDir()  

#define RD_ACTIVE   0
#define RD_IDLE     0
#define WR_ACTIVE   0
#define WR_IDLE     0
#if defined(__AVR__)
#define CD_COMMAND  *spicdPort &= spicdPinUnset     
#define CD_DATA     *spicdPort |= spicdPinSet
#define CS_ACTIVE   *spicsPort &= spicsPinUnset
#define CS_IDLE     *spicsPort |= spicsPinSet
#define MISO_STATE(x) { x = *spimisoPort&spimisoPinSet;}
#define MOSI_LOW *spimosiPort &= spimosiPinUnset
#define MOSI_HIGH   *spimosiPort |= spimosiPinSet
#define CLK_LOW  *spiclkPort &= spiclkPinUnset
#define CLK_HIGH    *spiclkPort |= spiclkPinSet

#elif defined(ARDUINO_ARCH_ESP8266)
#define CD_COMMAND  (digitalWrite(_cd,LOW))    
#define CD_DATA     (digitalWrite(_cd,HIGH)) 
#define CS_ACTIVE   (digitalWrite(_cs,LOW)) 
#define CS_IDLE     (digitalWrite(_cs,HIGH)) 
#define MISO_STATE(x) { x = digitalRead(_miso);}
#define MOSI_LOW    (digitalWrite(_mosi,LOW)) 
#define MOSI_HIGH   (digitalWrite(_mosi,HIGH)) 
#define CLK_LOW     (digitalWrite(_clk,LOW)) 
#define CLK_HIGH    (digitalWrite(_clk,HIGH)) 
#endif

#define WR_STROBE { }
#define RD_STROBE { }  

//#define write16(d) { uint8_t h = (d)>>8, l = (d&0xFF); write8(h); write8(l); }
#define write16(d) write8(d>>8); write8(d)
#define read16(dst) { uint8_t hi; read8(hi); read8(dst); dst |= (hi << 8); }
//#define writeCmd8(x) { CD_COMMAND; write8(x); CD_DATA;}
#define writeCmd8(x) CD_COMMAND; write8(x)
//#define writeData8(x) {  write8(x) }
#define writeData8(x)  CD_DATA; write8(x) 

//#define writeCmd16(x) { CD_COMMAND; write16(x); CD_DATA; }
//#define writeData16(x) { write16(x)}

#define writeCmd16(x)  CD_COMMAND; write16(x)
#define writeData16(x)  CD_DATA; write16(x)
#define writeData18(x)  CD_DATA; write8((x>>8)&0xF8);write8((x>>3)&0xFC);write8(x<<3)


//#define writeCmdData8(a, d) { CD_COMMAND; write8(a); CD_DATA; write8(d); }
//#define writeCmdData16(a, d) { \
  //uint8_t hi, lo; \
 // hi = (a) >> 8; lo = (a); CD_COMMAND; write8(hi); write8(lo); \
 // hi = (d) >> 8; lo = (d); CD_DATA   ; write8(hi); write8(lo); }

#define writeCmdData8(a, d) CD_COMMAND; write8(a); CD_DATA; write8(d)
#define writeCmdData16(a, d)  CD_COMMAND; write8(a>>8); write8(a); CD_DATA; write8(d>>8); write8(d)

#endif // _mcu_spi_magic_

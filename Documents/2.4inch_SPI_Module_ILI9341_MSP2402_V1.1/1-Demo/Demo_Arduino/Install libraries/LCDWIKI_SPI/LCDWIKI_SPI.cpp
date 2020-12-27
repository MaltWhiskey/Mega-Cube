// IMPORTANT: LIBRARY MUST BE SPECIFICALLY CONFIGURED FOR EITHER TFT SHIELD
// OR BREAKOUT BOARD USAGE.

// Lcdwiki GUI library with init code from Rossum
// MIT license


#if defined(__SAM3X8E__)
	#include <include/pio.h>
    #define PROGMEM
    #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
    #define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif

#if defined(ARDUINO_ARCH_ESP8266)
#define USE_HWSPI_ONLY
#endif

#include <SPI.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include "LCDWIKI_SPI.h"
#include "lcd_spi_registers.h"
#include "mcu_spi_magic.h"

#define TFTLCD_DELAY16  0xFFFF
#define TFTLCD_DELAY8   0x7F
#define MAX_REG_NUM     24


static uint8_t SH1106_buffer[1024] = {0};

//static uint8_t have_reset;

//#define LEFT_SHIFT(x) (1<<x) //x value 0:mipi dcs rev1
                             //        1:auto readinc
                             //        2:read bgr
                             //        3:read low and high
                             //        4:read 24 bits
                             //        5:xsa and xea 16 bit
                             //        6:read no dummy
                             //        8:invert gs
                             //        9:invert ss
                             //       10:mv axis
                             //       11:invert rgb
                             //       12:rev screen
                             //       13:filp vert	
			                 //       14:flip horiz

//The mode,width and heigth of supported LCD modules
lcd_info current_lcd_info[] = 
						 { 
							 0x9325,240,320,			
							 0x9328,240,320,
							 0x9341,240,320,
							 0x9090,320,480,
							 0x7575,240,320,
							 0x9595,240,320,
							 0x9486,320,480,
							 0x7735,128,160,
							 0x1283,130,130,
							 0x1106,128,64,
							 0x7735,128,128,
							 0x9488,320,480,
							 0x9488,320,480,
						 };

#if !defined(USE_HWSPI_ONLY)
// Constructor for software spi.
// if modules is unreadable or you don't know the width and height of modules,you can use this constructor.
LCDWIKI_SPI::LCDWIKI_SPI(uint16_t model,int8_t cs, int8_t cd, int8_t miso, int8_t mosi, int8_t reset, int8_t clk, int8_t led)
{
	_cs = cs;
	_cd = cd;
	_miso = miso;
	_mosi = mosi;
	_clk = clk;
	_reset = reset;
	_led = led;
	hw_spi = false; //software spi
	MODEL = model;
	spicsPort = portOutputRegister(digitalPinToPort(_cs));
	spicsPinSet = digitalPinToBitMask(_cs);
	spicsPinUnset = ~spicsPinSet;
		
	if(cd < 0)
	{
		spicdPort = 0;
		spicdPinSet = 0;
		spicdPinUnset = 0;
	}
	else
	{
		spicdPort = portOutputRegister(digitalPinToPort(_cd));
		spicdPinSet = digitalPinToBitMask(_cd);
		spicdPinUnset = ~spicdPinSet;	
	}
	if(miso < 0)
	{
		spimisoPort = 0;
		spimisoPinSet = 0;
		spimisoPinUnset = 0;
	}
	else
	{
		spimisoPort = portOutputRegister(digitalPinToPort(_miso));
		spimisoPinSet = digitalPinToBitMask(_miso);
		spimisoPinUnset = ~spimisoPinSet;
	}
	spimosiPort = portOutputRegister(digitalPinToPort(_mosi));
	spimosiPinSet = digitalPinToBitMask(_mosi);
	spimosiPinUnset = ~spimosiPinSet;

	spiclkPort = portOutputRegister(digitalPinToPort(_clk));
	spiclkPinSet = digitalPinToBitMask(_clk);
	spiclkPinUnset = ~spiclkPinSet;

	*spicsPort     |=  spicsPinSet; // Set all control bits to HIGH (idle)
	*spicdPort     |=  spicdPinSet; // Signals are ACTIVE LOW
	*spimisoPort   |=  spimisoPinSet;
	*spimosiPort   |=  spimosiPinSet;
	*spiclkPort    |=  spiclkPinSet;

	pinMode(cs, OUTPUT);	  // Enable outputs
	pinMode(cd, OUTPUT);
	pinMode(miso, INPUT);
	pinMode(mosi, OUTPUT);
	pinMode(clk, OUTPUT);
	if(reset >= 0) 
	{
		digitalWrite(reset, HIGH);
		pinMode(reset, OUTPUT);
	}
	if(led >= 0)
	{
		//digitalWrite(led, HIGH);
		pinMode(led, OUTPUT);
	}
	xoffset = 0;
	yoffset = 0;
	rotation = 0;
 	lcd_model = current_lcd_info[model].lcd_id;
	WIDTH = current_lcd_info[model].lcd_wid;
	HEIGHT = current_lcd_info[model].lcd_heg;
/*
	switch(lcd_model)
	{
		case 0x7735:
			WIDTH = 128;
			HEIGHT = 160;
		    break;
		case 0x9325:
		case 0x9328:
		case 0x9341:
		case 0x7575:
		case 0x9595:
			WIDTH = 240;
			HEIGHT = 320;
		    break;
		case 0x9486:
			WIDTH = 320;
			HEIGHT = 480;
            break;
		default:
			WIDTH = 0;
			HEIGHT = 0;
			break;
	}
*/
 	width = WIDTH;
	height = HEIGHT;
	setWriteDir();
}

// Constructor for software spi.
// if modules is readable or you know the width and height of modules,you can use this constructor.
LCDWIKI_SPI::LCDWIKI_SPI(int16_t wid,int16_t heg,int8_t cs, int8_t cd, int8_t miso, int8_t mosi, int8_t reset, int8_t clk, int8_t led)
{
	_cs = cs;
	_cd = cd;
	_miso = miso;
	_mosi = mosi;
	_clk = clk;
	_reset = reset;
	_led = led;
	hw_spi = false; //software spi

	spicsPort = portOutputRegister(digitalPinToPort(_cs));
	spicsPinSet = digitalPinToBitMask(_cs);
	spicsPinUnset = ~spicsPinSet;
		
	if(cd < 0)
	{
		spicdPort = 0;
		spicdPinSet = 0;
		spicdPinUnset = 0;
	}
	else
	{
		spicdPort = portOutputRegister(digitalPinToPort(_cd));
		spicdPinSet = digitalPinToBitMask(_cd);
		spicdPinUnset = ~spicdPinSet;	
	}
	if(miso < 0)
	{
		spimisoPort = 0;
		spimisoPinSet = 0;
		spimisoPinUnset = 0;
	}
	else
	{
		spimisoPort = portOutputRegister(digitalPinToPort(_miso));
		spimisoPinSet = digitalPinToBitMask(_miso);
		spimisoPinUnset = ~spimisoPinSet;
	}
	
	spimosiPort = portOutputRegister(digitalPinToPort(_mosi));
	spimosiPinSet = digitalPinToBitMask(_mosi);
	spimosiPinUnset = ~spimosiPinSet;

	spiclkPort = portOutputRegister(digitalPinToPort(_clk));
	spiclkPinSet = digitalPinToBitMask(_clk);
	spiclkPinUnset = ~spiclkPinSet;

	*spicsPort     |=  spicsPinSet; // Set all control bits to HIGH (idle)
	*spicdPort     |=  spicdPinSet; // Signals are ACTIVE LOW
	*spimisoPort   |=  spimisoPinSet;
	*spimosiPort   |=  spimosiPinSet;
	*spiclkPort    |=  spiclkPinSet;

	pinMode(cs, OUTPUT);	  // Enable outputs
	pinMode(cd, OUTPUT);
	pinMode(miso, INPUT);
	pinMode(mosi, OUTPUT);
	pinMode(clk, OUTPUT);
	if(reset >= 0) 
	{
		digitalWrite(reset, HIGH);
		pinMode(reset, OUTPUT);
	}
	if(led >= 0)
	{
		//digitalWrite(led, HIGH);
		pinMode(led, OUTPUT);
	}
	xoffset = 0;
	yoffset = 0;
	rotation = 0;
 	lcd_model = 0xFFFF;
    setWriteDir();
	WIDTH = wid;
	HEIGHT = heg;
 	width = WIDTH;
	height = HEIGHT;
}
#endif

// Constructor for hardware spi.
// if modules is unreadable or you don't know the width and height of modules,you can use this constructor.
LCDWIKI_SPI::LCDWIKI_SPI(uint16_t model,int8_t cs, int8_t cd, int8_t reset, int8_t led)
{
	_cs = cs;
	_cd = cd;
	_miso = -1;
	_mosi = -1;
	_clk = -1;
	_reset = reset;
	_led = led;
	hw_spi = true; //hardware spi
	MODEL = model;
#if defined(__AVR__)
	spicsPort = portOutputRegister(digitalPinToPort(_cs));
	spicsPinSet = digitalPinToBitMask(_cs);
	spicsPinUnset = ~spicsPinSet;
	if(cd < 0)
	{
		spicdPort = 0;
		spicdPinSet = 0;
		spicdPinUnset = 0;
	}
	else
	{
		spicdPort = portOutputRegister(digitalPinToPort(_cd));
		spicdPinSet = digitalPinToBitMask(_cd);
		spicdPinUnset = ~spicdPinSet;	
	}
	spimisoPort = 0;
	spimisoPinSet = 0;
	spimisoPinUnset = 0;
	spimosiPort = 0;
	spimosiPinSet = 0;
	spimosiPinUnset = 0;
	spiclkPort = 0;
	spiclkPinSet = 0;
	spiclkPinUnset = 0;
	
	*spicsPort     |=  spicsPinSet; // Set all control bits to HIGH (idle)
	*spicdPort     |=  spicdPinSet; // Signals are ACTIVE LOW
#elif defined(ARDUINO_ARCH_ESP8266)
	digitalWrite(_cs, HIGH);
	digitalWrite(_cd, HIGH);
#endif
	pinMode(cs, OUTPUT);	  // Enable outputs
	pinMode(cd, OUTPUT);

	if(reset >= 0) 
	{
		digitalWrite(reset, HIGH);
		pinMode(reset, OUTPUT);
	}
	if(led >= 0)
	{
		//digitalWrite(led, HIGH);
		pinMode(led, OUTPUT);
	}
	SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz (half speed)
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);

	xoffset = 0;
	yoffset = 0;
	rotation = 0;
 	lcd_model = current_lcd_info[model].lcd_id;
	WIDTH = current_lcd_info[model].lcd_wid;
	HEIGHT = current_lcd_info[model].lcd_heg;
/*
	switch(lcd_model)
	{
		case 0x7735:
			WIDTH = 128;
			HEIGHT = 160;
		    break;
		case 0x9325:
		case 0x9328:
		case 0x9341:
		case 0x7575:
		case 0x9595:
			WIDTH = 240;
			HEIGHT = 320;
		    break;
		case 0x9486:
			WIDTH = 320;
			HEIGHT = 480;
            break;
		default:
			WIDTH = 0;
			HEIGHT = 0;
			break;
	}
*/
 	width = WIDTH;
	height = HEIGHT;
	setWriteDir();
}

// Constructor for hardware spi.
// if modules is readable or you know the width and height of modules,you can use this constructor.
LCDWIKI_SPI::LCDWIKI_SPI(int16_t wid,int16_t heg,int8_t cs, int8_t cd, int8_t reset,int8_t led)
{
	_cs = cs;
	_cd = cd;
	_miso = -1;
	_mosi = -1;
	_clk = -1;
	_reset = reset;
	_led = led;
	hw_spi = true; //hardware spi
#if defined(__AVR__)
	spicsPort = portOutputRegister(digitalPinToPort(_cs));
	spicsPinSet = digitalPinToBitMask(_cs);
	spicsPinUnset = ~spicsPinSet;
	if(cd < 0)
	{
		spicdPort = 0;
		spicdPinSet = 0;
		spicdPinUnset = 0;
	}
	else
	{
		spicdPort = portOutputRegister(digitalPinToPort(_cd));
		spicdPinSet = digitalPinToBitMask(_cd);
		spicdPinUnset = ~spicdPinSet;	
	}
	spimisoPort = 0;
	spimisoPinSet = 0;
	spimisoPinUnset = 0;
	spimosiPort = 0;
	spimosiPinSet = 0;
	spimosiPinUnset = 0;
	spiclkPort = 0;
	spiclkPinSet = 0;
	spiclkPinUnset = 0;

	*spicsPort     |=  spicsPinSet; // Set all control bits to HIGH (idle)
	*spicdPort     |=  spicdPinSet; // Signals are ACTIVE LOW
#elif defined(ARDUINO_ARCH_ESP8266)
		digitalWrite(_cs, HIGH);
		digitalWrite(_cd, HIGH);
#endif

	pinMode(cs, OUTPUT);	  // Enable outputs
	pinMode(cd, OUTPUT);	

	if(reset >= 0) 
	{
		digitalWrite(reset, HIGH);
		pinMode(reset, OUTPUT);
	}
	if(led >= 0)
	{
		//digitalWrite(led, HIGH);
		pinMode(led, OUTPUT);
	}
	SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz (half speed)
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);

	xoffset = 0;
	yoffset = 0;
	rotation = 0;
 	lcd_model = 0xFFFF;
    setWriteDir();
	WIDTH = wid;
	HEIGHT = heg;
 	width = WIDTH;
	height = HEIGHT;
}

// Initialization lcd modules
void LCDWIKI_SPI::Init_LCD(void)
{
	reset();
	Led_control(true);
	if(lcd_model == 0xFFFF)
	{
		lcd_model = Read_ID(); 
	}
//	uint16_t ID = Read_ID(); 
	start(lcd_model);
//	Set_Rotation(0);
}

// Initialization common to both shield & breakout configs
void LCDWIKI_SPI::reset(void)
{
//	have_reset = 1;
//    setWriteDir();
    CS_IDLE;
    RD_IDLE;
    WR_IDLE;
  if(_reset >=0) 
  {
    digitalWrite(_reset, LOW);
    delay(2);
    digitalWrite(_reset, HIGH);
  }
  CS_ACTIVE;
  CD_COMMAND;
  write8(0x00);
  for(uint8_t i=0; i<3; i++)
  {
  	WR_STROBE; // Three extra 0x00s
  }
  CS_IDLE;
}

void LCDWIKI_SPI::Led_control(boolean i)
{
	if(_led >= 0)
	{
		if(i)
		{
			digitalWrite(_led, HIGH);
		}
		else
		{
			digitalWrite(_led, LOW);
		}
	}
}

//spi write for hardware and software
void LCDWIKI_SPI::Spi_Write(uint8_t data)
{
	if(hw_spi)
	{
		SPI.transfer(data);
	}
	else
	{
		uint8_t val = 0x80;
		while(val)
		{
			if(data&val)
			{
				MOSI_HIGH; 
			}
			else
			{
				MOSI_LOW;
			}
			CLK_LOW;
			CLK_HIGH;
			val >>= 1;
		}
	}
}

//spi read for hardware and software
uint8_t LCDWIKI_SPI::Spi_Read(void)
{
	if(hw_spi)
	{
		return SPI.transfer(0xFF);
	}
	else
	{
		uint8_t val,i,d;
		for(i = 0;i<8; i++)
		{
			CLK_LOW; 
			CLK_HIGH;
			val <<= 1;
			MISO_STATE(d);
			if(d)
			{
				val |= 0x01;
			}
		}
		return val;
	}
}

void LCDWIKI_SPI::Write_Cmd(uint16_t cmd)
{
	CS_ACTIVE;
	writeCmd16(cmd);
	CS_IDLE;
}

void LCDWIKI_SPI::Write_Data(uint16_t data)
{
	CS_ACTIVE;
	writeData16(data);
	CS_IDLE;
}

void LCDWIKI_SPI::Write_Cmd_Data(uint16_t cmd, uint16_t data)
{
	CS_ACTIVE;
	writeCmdData16(cmd,data);
	CS_IDLE;
}

//Write a command and N datas
void LCDWIKI_SPI::Push_Command(uint8_t cmd, uint8_t *block, int8_t N)
{
  	CS_ACTIVE;
	if(lcd_driver == ID_1106)
	{
    	writeCmd8(cmd);
	}
	else
	{
		writeCmd16(cmd);
	}
	while (N-- > 0) 
	{
        uint8_t u8 = *block++;
        writeData8(u8); 
		if(N && (lcd_driver == ID_7575))
		{
			cmd++;
			writeCmd16(cmd);
		}
    }
    CS_IDLE;
}

// Sets the LCD address window 
void LCDWIKI_SPI::Set_Addr_Window(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	CS_ACTIVE;
	if(lcd_driver == ID_932X) 
	{

	    // Values passed are in current (possibly rotated) coordinate
	    // system.  932X requires hardware-native coords regardless of
	    // MADCTL, so rotate inputs as needed.  The address counter is
	    // set to the top-left corner -- although fill operations can be
	    // done in any direction, the current screen rotation is applied
	    // because some users find it disconcerting when a fill does not
	    // occur top-to-bottom.
	    int x, y, t;
	    switch(rotation) 
		{
		     default:
			      x  = x1;
			      y  = y1;
			      break;
		     case 1:
			      t  = y1;
			      y1 = x1;
			      x1 = WIDTH  - 1 - y2;
			      y2 = x2;
			      x2 = WIDTH  - 1 - t;
			      x  = x2;
			      y  = y1;
		      	  break;
		     case 2:
			      t  = x1;
			      x1 = WIDTH  - 1 - x2;
			      x2 = WIDTH  - 1 - t;
			      t  = y1;
			      y1 = HEIGHT - 1 - y2;
			      y2 = HEIGHT - 1 - t;
			      x  = x2;
			      y  = y2;
			      break;
		     case 3:
			      t  = x1;
			      x1 = y1;
			      y1 = HEIGHT - 1 - x2;
			      x2 = y2;
			      y2 = HEIGHT - 1 - t;
			      x  = x1;
			      y  = y2;
			      break;
    	}
    writeCmdData16(ILI932X_HOR_START_AD, x1); // Set address window
    writeCmdData16(ILI932X_HOR_END_AD, x2);
    writeCmdData16(ILI932X_VER_START_AD, y1);
    writeCmdData16(ILI932X_VER_END_AD, y2);
    writeCmdData16(ILI932X_GRAM_HOR_AD, x ); // Set address counter to top left
    writeCmdData16(ILI932X_GRAM_VER_AD, y );
 	} 
	else if(lcd_driver == ID_7575)
	{
		writeCmdData8(HX8347G_COLADDRSTART_HI,x1>>8);
		writeCmdData8(HX8347G_COLADDRSTART_LO,x1);
		writeCmdData8(HX8347G_ROWADDRSTART_HI,y1>>8);
		writeCmdData8(HX8347G_ROWADDRSTART_LO,y1);
		writeCmdData8(HX8347G_COLADDREND_HI,x2>>8);
		writeCmdData8(HX8347G_COLADDREND_LO,x2);
		writeCmdData8(HX8347G_ROWADDREND_HI,y2>>8);
		writeCmdData8(HX8347G_ROWADDREND_LO,y2);
	}
	else if(lcd_driver == ID_1283A)
	{
		int16_t t1,t2;
		switch(rotation)
		{
			case 0:
			case 2:
				t1=x1;
				t2=x2;
				x1=y1+2;
				x2=y2+2;
				y1=t1+2;
				y2=t2+2;
				break;				
			case 1:
			case 3:			
				y1=y1+2;
				y2=y2+2;
				break;
		}
		writeCmd8(XC);
		writeData8(x2);
		writeData8(x1);
		writeCmd8(YC);
		writeData8(y2);
		writeData8(y1);
		writeCmd8(0x21);
		writeData8(x1);
		writeData8(y1);
		writeCmd8(CC);
	}
	else if(lcd_driver == ID_1106)
	{
		return;
	}
	else if(lcd_driver == ID_7735_128)
	{
		uint8_t x_buf[] = {(x1+xoffset)>>8,(x1+xoffset)&0xFF,(x2+xoffset)>>8,(x2+xoffset)&0xFF};
		uint8_t y_buf[] = {(y1+yoffset)>>8,(y1+yoffset)&0xFF,(y2+yoffset)>>8,(y2+yoffset)&0xFF};
		Push_Command(XC, x_buf, 4);
		Push_Command(YC, y_buf, 4);
	}
	else
	{
		uint8_t x_buf[] = {x1>>8,x1&0xFF,x2>>8,x2&0xFF};
		uint8_t y_buf[] = {y1>>8,y1&0xFF,y2>>8,y2&0xFF};
		Push_Command(XC, x_buf, 4);
		Push_Command(YC, y_buf, 4);
	}
	CS_IDLE;		
}

// Unlike the 932X drivers that set the address window to the full screen
// by default (using the address counter for drawPixel operations), the
// 7575 needs the address window set on all graphics operations.  In order
// to save a few register writes on each pixel drawn, the lower-right
// corner of the address window is reset after most fill operations, so
// that drawPixel only needs to change the upper left each time.
void LCDWIKI_SPI::Set_LR(void)
{
	CS_ACTIVE;
	writeCmdData8(HX8347G_COLADDREND_HI,(width -1)>>8);
	writeCmdData8(HX8347G_COLADDREND_LO,width -1);
	writeCmdData8(HX8347G_ROWADDREND_HI,(height -1)>>8);
	writeCmdData8(HX8347G_ROWADDREND_LO,height -1);
	CS_IDLE;
}

//push color table for 16bits
void LCDWIKI_SPI::Push_Any_Color(uint16_t * block, int16_t n, bool first, uint8_t flags)
{
	uint16_t color;
    uint8_t h, l;
	bool isconst = flags & 1;
//	bool isbigend = (flags & 2) != 0;
    CS_ACTIVE;
    if (first) 
	{  
		if(lcd_driver == ID_932X)
		{
			writeCmd8(ILI932X_START_OSC);
			
		}
		writeCmd8(CC);		
    }
    while (n-- > 0) 
	{
        if (isconst) 
		{
			color = pgm_read_word(block++);		
        } 
		else 
		{
			color = (*block++);			

		}
		if(MODEL == ILI9488_18)
		{
			writeData18(color);
		}
		else
		{
        	writeData16(color);
		}
	}
    CS_IDLE;
}

//push color table for 8bits
void LCDWIKI_SPI::Push_Any_Color(uint8_t * block, int16_t n, bool first, uint8_t flags)
{
	uint16_t color;
    uint8_t h, l;
	bool isconst = flags & 1;
	bool isbigend = (flags & 2) != 0;
    CS_ACTIVE;
    if (first) 
	{
		if(lcd_driver == ID_932X)
		{
			writeCmd8(ILI932X_START_OSC);
			
		}
		writeCmd8(CC);		
    }
    while (n-- > 0) 
	{
        if (isconst) 
		{
            h = pgm_read_byte(block++);
            l = pgm_read_byte(block++);
        } 
		else 
		{
		    h = (*block++);
            l = (*block++);
		}
        color = (isbigend) ? (h << 8 | l) :  (l << 8 | h);
		if(MODEL == ILI9488_18)
		{
			writeData18(color);
		}
		else
		{
        	writeData16(color);
		}
	}
    CS_IDLE;
}

//Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t LCDWIKI_SPI::Color_To_565(uint8_t r, uint8_t g, uint8_t b)
{
	return ((r& 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

//read value from lcd register 
uint16_t LCDWIKI_SPI::Read_Reg(uint16_t reg, int8_t index)
{
	uint16_t ret,high;
    uint8_t low;
//  if (!have_reset)
 // {
 //     reset();
//  }
	CS_ACTIVE;
    writeCmd16(reg);
    setReadDir();
    delay(1); 
	do 
	{ 
	//ead8(high);
	//ead8(low);
	//et = (high << 8) | lowc
		read16(ret);
	}while (--index >= 0);  
//    RD_IDLE;
    CS_IDLE;
    setWriteDir();
    return ret;
}

//read graph RAM data
int16_t LCDWIKI_SPI::Read_GRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h)
{
	uint16_t ret, dummy;
    int16_t n = w * h;
    uint8_t r, g, b, tmp;
    Set_Addr_Window(x, y, x + w - 1, y + h - 1);
    while (n > 0) 
	{
        CS_ACTIVE;
		writeCmd16(RC);
        setReadDir();
		if(lcd_driver == ID_932X)
		{
			while(n)
			{
				for(int i =0; i< 2; i++)
				{
					read8(r);
					read8(r);
					read8(r);
					read8(g);
				}
				*block++ = (r<<8 | g);
				n--;
			}
			Set_Addr_Window(0, 0, width - 1, height - 1);
		}
		else 
		{
			read8(r);
        	while (n) 
			{
				if(R24BIT == 1)
				{
        			read8(r);
         			read8(g);
        			read8(b);
            		ret = Color_To_565(r, g, b);
				}
				else if(R24BIT == 0)
				{
					read16(ret);
				}
            	*block++ = ret;
            	n--;
        	}
        }
//        RD_IDLE;
        CS_IDLE;
        setWriteDir();
    }
	return 0;
}

//read LCD controller chip ID
uint16_t LCDWIKI_SPI::Read_ID(void)
{
	uint16_t ret;
	if ((Read_Reg(0x04,0) == 0x00)&&(Read_Reg(0x04,1) == 0x8000))
	{
		uint8_t buf[] = {0xFF, 0x83, 0x57};
		Push_Command(HX8357D_SETC, buf, sizeof(buf));
		ret = (Read_Reg(0xD0,0) << 16) | Read_Reg(0xD0,1);
		if((ret == 0x990000) || (ret == 0x900000))
		{
			return 0x9090;
		}
	}
	ret = Read_Reg(0xD3,1);
	if(ret == 0x9341)
	{
		return 0x9341;
	}
	else if(ret == 0x9486)
	{
		return 0x9486;
	}
	else if(ret == 0x9488)
	{
		return 0x9488;
	}
	else
	{
		return Read_Reg(0, 0);
	}
}

//set x,y  coordinate and color to draw a pixel point 
void LCDWIKI_SPI::Draw_Pixe(int16_t x, int16_t y, uint16_t color)
{
	if((x < 0) || (y < 0) || (x > Get_Width()) || (y > Get_Height()))
	{
		return;
	}
	Set_Addr_Window(x, y, x, y);
	CS_ACTIVE;
	if(lcd_driver == ID_1283A)
	{
		writeData16(color);
	}
	else if(lcd_driver == ID_1106)
	{
		if(color)
		{
			SH1106_buffer[(y/8)*WIDTH+x]|= (1<<(y%8))&0xff;
		}
		else
		{
			SH1106_buffer[(y/8)*WIDTH+x]&= ~((1<<(y%8))&0xff);
		}
	}
	else
	{
		if(MODEL == ILI9488_18)
		{
			writeCmd8(CC);
			writeData18(color);
		}
		else
		{
			writeCmdData16(CC, color);
		}
	}
	CS_IDLE;
}

//fill area from x to x+w,y to y+h
void LCDWIKI_SPI::Fill_Rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	int16_t end;
	if (w < 0) 
	{
        w = -w;
        x -= w;
    }                           //+ve w
    end = x + w;
    if (x < 0)
    {
        x = 0;
    }
    if (end > Get_Width())
    {
        end = Get_Width();
    }
    w = end - x;
    if (h < 0) 
	{
        h = -h;
        y -= h;
    }                           //+ve h
    end = y + h;
    if (y < 0)
    {
        y = 0;
    }
    if (end > Get_Height())
    {
        end = Get_Height();
    }
    h = end - y;
    Set_Addr_Window(x, y, x + w - 1, y + h - 1);
	CS_ACTIVE;
	if(lcd_driver == ID_1106)
	{
		int16_t i,j;
		for(i=0;i<h;i++)
		{
			for(j=0;j<w;j++)
			{
				Draw_Pixe(x+j, y+i,color);
			}		
		}
		CS_IDLE;
		return;
	}
    else if(lcd_driver == ID_932X)
	{
		writeCmd8(ILI932X_START_OSC);		
	} 		
	writeCmd8(CC);		
	if (h > w) 
	{
        end = h;
        h = w;
        w = end;
    }
	while (h-- > 0) 
	{
		end = w;
		do 
		{
			if(MODEL == ILI9488_18)
			{
				writeData18(color);
			}
			else
			{
   				writeData16(color);
			}
		} while (--end != 0);
	}
	if(lcd_driver == ID_932X)
	{
		Set_Addr_Window(0, 0, width - 1, height - 1);
	}
	else if(lcd_driver == ID_7575)
	{
		Set_LR();
	}
	CS_IDLE;
}

//Scroll display 
void LCDWIKI_SPI::Vert_Scroll(int16_t top, int16_t scrollines, int16_t offset)
{
    int16_t bfa;
    int16_t vsp;
    int16_t sea = top;
	if(lcd_driver == ID_7735_128)
	{
		bfa = HEIGHT - top - scrollines+4; 
	}
	else
	{
		bfa = HEIGHT - top - scrollines; 
	}
    if (offset <= -scrollines || offset >= scrollines)
    {
		offset = 0; //valid scroll
    }
	vsp = top + offset; // vertical start position
    if (offset < 0)
    {
        vsp += scrollines;          //keep in unsigned range
    }
    sea = top + scrollines - 1;
	if(lcd_driver == ID_932X)
	{
		Write_Cmd_Data(SC1, (1 << 1) | 0x1);        //!NDL, VLE, REV
        Write_Cmd_Data(SC2, vsp);        //VL#
	}
	else if(lcd_driver == ID_1283A)
	{
		Write_Cmd_Data(SC1,vsp);
	}
	else if(lcd_driver == ID_1106)
	{
		return;
	}
	else
	{
  		uint8_t d[6];           // for multi-byte parameters
  		d[0] = top >> 8;        //TFA
  		d[1] = top;
  		d[2] = scrollines >> 8; //VSA
  		d[3] = scrollines;
  		d[4] = bfa >> 8;        //BFA
  		d[5] = bfa;
		Push_Command(SC1, d, 6);
		d[0] = vsp >> 8;        //VSP
  		d[1] = vsp;
		Push_Command(SC2, d, 2);
		if(lcd_driver == ID_7575)
		{
			d[0] = (offset != 0) ? 0x08:0;
			Push_Command(0x01, d, 1);
		}
		else if (offset == 0) 
		{
			Push_Command(0x13, NULL, 0);
		}
	}
}

//get lcd width
int16_t LCDWIKI_SPI::Get_Width(void) const
{
	return width;
}

//get lcd height
int16_t LCDWIKI_SPI::Get_Height(void) const
{
	return height;
}

//set clockwise rotation
void LCDWIKI_SPI::Set_Rotation(uint8_t r)
{
    rotation = r & 3;           // just perform the operation ourselves on the protected variables
    width = (rotation & 1) ? HEIGHT : WIDTH;
    height = (rotation & 1) ? WIDTH : HEIGHT;
	CS_ACTIVE;
	if(lcd_driver == ID_932X)
	{
		uint16_t val;
		switch(rotation) 
		{
			case 0: 
				val = 0x1030; //0 degree 
				break;
		 	case 1: 
				val = 0x1028; //90 degree 
				break;
		 	case 2: 
				val = 0x1000; //180 degree 
				break;
		 	case 3: 
				val = 0x1018; //270 degree 
				break;
		}
		writeCmdData16(MD, val); 
	}
	else if(lcd_driver == ID_7735)
	{
		uint8_t val;
		switch(rotation)
		{
			case 0: 
				val = 0xD0; //0 degree
				break;
		 	case 1: 
				val = 0xA0; //90 degree 
				break;
		 	case 2: 
				val = 0x00; //180 degree 
				break;
		 	case 3: 
				val = 0x60; //270 degree 
				break;			
		}
		writeCmdData8(MD, val);
	}
	else if(lcd_driver == ID_7735_128)
	{
		uint8_t val;
		switch(rotation)
		{
			case 0: 
				val = 0xD8; //0 degree
				xoffset = 2;
				yoffset = 3;
				break;
		 	case 1: 
				val = 0xA8; //90 degree 
				xoffset = 3;
				yoffset = 2;
				break;
		 	case 2: 
				val = 0x08; //180 degree 
				xoffset = 2;
				yoffset = 1;
				break;
		 	case 3: 
				val = 0x68; //270 degree 
				xoffset = 1;
				yoffset = 2;
				break;			
		}
		writeCmdData8(MD, val);
	}
	else if(lcd_driver == ID_1283A)
	{
		switch(rotation)
		{
			case 0:
			case 2:
				writeCmdData16(0x01, 0x2183);
				writeCmdData16(0x03, 0x6830);
				break; 				
			case 1:
			case 3:
				writeCmdData16(0x01, 0x2283);
				writeCmdData16(0x03, 0x6838);
			 	break;
		}
	}
	else if(lcd_driver == ID_1106)
	{
		return;
	}
	else if(lcd_driver == ID_9486)
	{
		uint8_t val;
		switch (rotation) 
		{
		   	case 0:
		     	val = ILI9341_MADCTL_BGR; //0 degree 
		     	break;
		   	case 1:
		     	val = ILI9341_MADCTL_MX | ILI9341_MADCTL_MV | ILI9341_MADCTL_ML | ILI9341_MADCTL_BGR ; //90 degree 
		     	break;
		 	case 2:
		    	val = ILI9341_MADCTL_MY | ILI9341_MADCTL_MX |ILI9341_MADCTL_BGR; //180 degree 
		    	break;
		   	case 3:
		     	val = ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR; //270 degree
		     	break;
		 }
		 writeCmdData8(MD, val); 
	}
	else if(lcd_driver == ID_9488)
	{
		uint8_t val;
		switch (rotation) 
		{			
			case 0:
		     	val = ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR ; //0 degree 
		     	break;
		   	case 1:
		     	val = ILI9341_MADCTL_MV | ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR ; //90 degree 
		     	break;
		 	case 2:
		    	val = ILI9341_MADCTL_ML | ILI9341_MADCTL_BGR; //180 degree 
		    	break;
		   	case 3:
		     	val = ILI9341_MADCTL_MX | ILI9341_MADCTL_ML | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR; //270 degree
		     	break;
		 }
		 writeCmdData8(MD, val); 
	}
	else
	{
		uint8_t val;
		switch (rotation) 
		{
		   	case 0:
		     	val = ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR; //0 degree 
				break;
		   	case 1:
		     	val = ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR; //90 degree 
		     	break;
		 	case 2:
		    	val = ILI9341_MADCTL_MY | ILI9341_MADCTL_ML |ILI9341_MADCTL_BGR; //180 degree 
		    	break;
		   	case 3:
		     	val = ILI9341_MADCTL_MX | ILI9341_MADCTL_MY| ILI9341_MADCTL_ML | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR; //270 degree 
		     	break;
		 }
		 writeCmdData8(MD, val); 
	}
 	Set_Addr_Window(0, 0, width - 1, height - 1);
	Vert_Scroll(0, HEIGHT, 0);
	CS_IDLE;
}

//get current rotation
//0  :  0 degree 
//1  :  90 degree
//2  :  180 degree
//3  :  270 degree
uint8_t LCDWIKI_SPI::Get_Rotation(void) const
{
	return rotation;
}

//Anti color display 
void LCDWIKI_SPI::Invert_Display(boolean i)
{
	CS_ACTIVE;
	uint8_t val = VL^i;
	if(lcd_driver == ID_932X)
	{
		writeCmdData8(0x61, val);
	}
	else if(lcd_driver == ID_7575)
	{
		writeCmdData8(0x01, val ? 8 : 10);
	}
	else if(lcd_driver == ID_1283A)
	{
		uint16_t reg;
		if((rotation == 0)||(rotation == 2))
		{
			if(val)
			{
				reg=0x2183;
			}
			else
			{
				reg=0x0183;
			}
		}
		else if((rotation == 1) || (rotation == 3))
		{
			if(val)
			{
				reg=0x2283;
			}
			else
			{
				reg=0x0283;
			}
		}
		writeCmdData16(0x01,reg);
	}
	else if(lcd_driver == ID_1106)
	{
		writeCmd8(val ? 0xA6 : 0xA7);
	}
	else
	{
		writeCmd8(val ? 0x21 : 0x20);
	}
	CS_IDLE;
}

void LCDWIKI_SPI::SH1106_Draw_Bitmap(uint8_t x,uint8_t y,uint8_t width, uint8_t height, uint8_t *BMP, uint8_t mode)
{
  uint8_t i,j,k;
  uint8_t tmp;
  for(i=0;i<(height+7)/8;i++)
  {
		for(j=0;j<width;j++)
		{
		    	if(mode)
				{
					tmp = pgm_read_byte(&BMP[i*width+j]);
				}
				else
				{
					tmp = ~(pgm_read_byte(&BMP[i*width+j]));
				}
				for(k=0;k<8;k++)
				{
					if(tmp&0x01)
					{
						Draw_Pixe(x+j, y+i*8+k,1);
					}
					else
					{
						Draw_Pixe(x+j, y+i*8+k,0);
					}
					tmp>>=1;
				}
		}
   } 
}

void LCDWIKI_SPI::SH1106_Display(void)
{
	u8 i,n;	
	CS_ACTIVE;
	for(i=0;i<8;i++)  
	{  
		writeCmd8(YC+i);    
		writeCmd8(0x02); 
		writeCmd8(XC); 
		for(n=0;n<WIDTH;n++)
		{
			writeData8(SH1106_buffer[i*WIDTH+n]); 
		}
	} 
	CS_IDLE;
}

void LCDWIKI_SPI:: init_table8(const void *table, int16_t size)
{
	uint8_t i;
    uint8_t *p = (uint8_t *) table, dat[MAX_REG_NUM];            //R61526 has GAMMA[22] 
    while (size > 0) 
	{
        uint8_t cmd = pgm_read_byte(p++);
        uint8_t len = pgm_read_byte(p++);
        if (cmd == TFTLCD_DELAY8) 
		{
            delay(len);
            len = 0;
        } 
		else 
		{
            for (i = 0; i < len; i++)
            {
                dat[i] = pgm_read_byte(p++);
            }
			Push_Command(cmd,dat,len);
        }
        size -= len + 2;
    }
}

void LCDWIKI_SPI:: init_table16(const void *table, int16_t size)
{
    uint16_t *p = (uint16_t *) table;
    while (size > 0) 
	{
        uint16_t cmd = pgm_read_word(p++);
        uint16_t d = pgm_read_word(p++);
        if (cmd == TFTLCD_DELAY16)
        {
            delay(d);
        }
        else 
		{
			Write_Cmd_Data(cmd, d);                      //static function
		}
        size -= 2 * sizeof(int16_t);
    }
}

void LCDWIKI_SPI::start(uint16_t ID)
{
	reset();
	delay(200);
	switch(ID)
	{
		case 0x9325:
		case 0x9328:
			lcd_driver = ID_932X;
			//WIDTH = 240,HEIGHT = 320;
			//width = WIDTH, height = HEIGHT;
			XC=0,YC=0,CC=ILI932X_RW_GRAM,RC=ILI932X_RW_GRAM,SC1=ILI932X_GATE_SCAN_CTRL2,SC2=ILI932X_GATE_SCAN_CTRL3,MD=0x0003,VL=1,R24BIT=0;
			static const uint16_t ILI932x_regValues[] PROGMEM = 
			{			
		  		ILI932X_START_OSC 	   , 0x0001, // Start oscillator
		  		TFTLCD_DELAY16			   , 50,	 // 50 millisecond delay
		  		ILI932X_DRIV_OUT_CTRL    , 0x0100,
		  		ILI932X_DRIV_WAV_CTRL    , 0x0700,
		  		ILI932X_ENTRY_MOD 	   , 0x1030,
		  		ILI932X_RESIZE_CTRL	   , 0x0000,
		  		ILI932X_DISP_CTRL2	   , 0x0202,
		  		ILI932X_DISP_CTRL3	   , 0x0000,
		  		ILI932X_DISP_CTRL4	   , 0x0000,
		  		ILI932X_RGB_DISP_IF_CTRL1, 0x0,
		  		ILI932X_FRM_MARKER_POS   , 0x0,
		  		ILI932X_RGB_DISP_IF_CTRL2, 0x0,
		  		ILI932X_POW_CTRL1 	   , 0x0000,
		  		ILI932X_POW_CTRL2 	   , 0x0007,
		  		ILI932X_POW_CTRL3 	   , 0x0000,
		  		ILI932X_POW_CTRL4 	   , 0x0000,
		  		TFTLCD_DELAY16			   , 200,
		  		ILI932X_POW_CTRL1 	   , 0x1690,
		  		ILI932X_POW_CTRL2 	   , 0x0227,
		  		TFTLCD_DELAY16			   , 50,
		  		ILI932X_POW_CTRL3 	   , 0x001A,
		  		TFTLCD_DELAY16			   , 50,
		  		ILI932X_POW_CTRL4 	   , 0x1800,
		  		ILI932X_POW_CTRL7 	   , 0x002A,
		  		TFTLCD_DELAY16			   , 50,
		  		ILI932X_GAMMA_CTRL1	   , 0x0000,
		  		ILI932X_GAMMA_CTRL2	   , 0x0000,
		  		ILI932X_GAMMA_CTRL3	   , 0x0000,
		  		ILI932X_GAMMA_CTRL4	   , 0x0206,
		  		ILI932X_GAMMA_CTRL5	   , 0x0808,
		  		ILI932X_GAMMA_CTRL6	   , 0x0007,
		  		ILI932X_GAMMA_CTRL7	   , 0x0201,
		  		ILI932X_GAMMA_CTRL8	   , 0x0000,
		  		ILI932X_GAMMA_CTRL9	   , 0x0000,
		 		ILI932X_GAMMA_CTRL10	   , 0x0000,
		  		ILI932X_GRAM_HOR_AD	   , 0x0000,
		  		ILI932X_GRAM_VER_AD	   , 0x0000,
		  		ILI932X_HOR_START_AD	   , 0x0000,
		  		ILI932X_HOR_END_AD	   , 0x00EF,
		  		ILI932X_VER_START_AD	   , 0X0000,
		  		ILI932X_VER_END_AD	   , 0x013F,
		  		ILI932X_GATE_SCAN_CTRL1  , 0xA700, // Driver Output Control (R60h)
		  		ILI932X_GATE_SCAN_CTRL2  , 0x0003, // Driver Output Control (R61h)
		  		ILI932X_GATE_SCAN_CTRL3  , 0x0000, // Driver Output Control (R62h)
		  		ILI932X_PANEL_IF_CTRL1   , 0X0010, // Panel Interface Control 1 (R90h)
		  		ILI932X_PANEL_IF_CTRL2   , 0X0000,
		  		ILI932X_PANEL_IF_CTRL3   , 0X0003,
		  		ILI932X_PANEL_IF_CTRL4   , 0X1100,
		  		ILI932X_PANEL_IF_CTRL5   , 0X0000,
		  		ILI932X_PANEL_IF_CTRL6   , 0X0000,
		  		ILI932X_DISP_CTRL1	   , 0x0133 // Main screen turn on		
			};
			init_table16(ILI932x_regValues, sizeof(ILI932x_regValues));
			break;
		case 0x9341:
			lcd_driver = ID_9341;
			//WIDTH = 240,HEIGHT = 320;
			//width = WIDTH, height = HEIGHT;
			XC=ILI9341_COLADDRSET,YC=ILI9341_PAGEADDRSET,CC=ILI9341_MEMORYWRITE,RC=HX8357_RAMRD,SC1=0x33,SC2=0x37,MD=ILI9341_MADCTL,VL=0,R24BIT=1;
			static const uint8_t ILI9341_regValues[] PROGMEM = 
			{        // BOE 2.4"
				ILI9341_SOFTRESET,0,                 //Soft Reset
				TFTLCD_DELAY8, 50, 
				ILI9341_DISPLAYOFF, 0,            //Display Off
            //	ILI9341_PIXELFORMAT, 1, 0x55,      //Pixel read=565, write=565.
            	ILI9341_INTERFACECONTROL, 3, 0x01, 0x01, 0x00,  //Interface Control needs EXTC=1 MV_EOR=0, TM=0, RIM=0
            	ILI9341_POWERCONTROLB, 3, 0x00, 0x81, 0x30,  //Power Control B [00 81 30]
            	ILI9341_POWERONSEQ, 4, 0x64, 0x03, 0x12, 0x81,    //Power On Seq [55 01 23 01]
            	ILI9341_DRIVERTIMINGA, 3, 0x85, 0x10, 0x78,  //Driver Timing A [04 11 7A]
            	ILI9341_POWERCONTROLA, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,      //Power Control A [39 2C 00 34 02]
            	ILI9341_RUMPRATIO, 1, 0x20,      //Pump Ratio [10]
            	ILI9341_DRIVERTIMINGB, 2, 0x00, 0x00,        //Driver Timing B [66 00]
            	ILI9341_RGBSIGNAL, 1, 0x00,      //RGB Signal [00] 
            //	ILI9341_FRAMECONTROL, 2, 0x00, 0x1B,        //Frame Control [00 1B]
            	//            0xB6, 2, 0x0A, 0xA2, 0x27, //Display Function [0A 82 27 XX]    .kbv SS=1  
            	ILI9341_INVERSIONCONRTOL, 1, 0x00,      //Inversion Control [02] .kbv NLA=1, NLB=1, NLC=1
            	ILI9341_POWERCONTROL1, 1, 0x21,      //Power Control 1 [26]
            	ILI9341_POWERCONTROL2, 1, 0x11,      //Power Control 2 [00]
            	ILI9341_VCOMCONTROL1, 2, 0x3F, 0x3C,        //VCOM 1 [31 3C]
            	ILI9341_VCOMCONTROL2, 1, 0xB5,      //VCOM 2 [C0]
            	ILI9341_MEMCONTROL, 1, ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR,
            	ILI9341_PIXELFORMAT, 1, 0x55,      //Pixel read=565, write=565.
            	ILI9341_FRAMECONTROL, 2, 0x00, 0x1B,        //Frame Control [00 1B]
            	ILI9341_MEMORYACCESS, 1, 0x48,      //Memory Access [00]
            	ILI9341_ENABLE3G, 1, 0x00,      //Enable 3G [02]
            	ILI9341_GAMMASET, 1, 0x01,      //Gamma Set [01]
            	ILI9341_UNDEFINE0, 15, 0x0f, 0x26, 0x24, 0x0b, 0x0e, 0x09, 0x54, 0xa8, 0x46, 0x0c, 0x17, 0x09, 0x0f, 0x07, 0x00,
            	ILI9341_UNDEFINE1, 15, 0x00, 0x19, 0x1b, 0x04, 0x10, 0x07, 0x2a, 0x47, 0x39, 0x03, 0x06, 0x06, 0x30, 0x38, 0x0f,
				ILI9341_ENTRYMODE, 1,0x07,
            	ILI9341_SLEEPOUT, 0,            //Sleep Out
            	TFTLCD_DELAY8, 150,
            	ILI9341_DISPLAYON, 0          //Display On
            };
			init_table8(ILI9341_regValues, sizeof(ILI9341_regValues));    
			break;
		case 0x9090:
			lcd_driver = ID_HX8357D;
			//WIDTH = 320,HEIGHT = 480;
			//width = WIDTH, height = HEIGHT;
			XC=ILI9341_COLADDRSET,YC=ILI9341_PAGEADDRSET,CC=HX8357_RAMWR,RC=HX8357_RAMRD,SC1=0x33,SC2=0x37,MD=HX8357_MADCTL,VL=1,R24BIT=1;
			static const uint8_t HX8357D_regValues[] PROGMEM = 
			{
  				HX8357_SWRESET, 0,
  				HX8357D_SETC, 3, 0xFF, 0x83, 0x57,
  				TFTLCD_DELAY8, 250,
				HX8357_SETRGB, 4, 0x00, 0x00, 0x06, 0x06,
				HX8357D_SETCOM, 1, 0x25,  // -1.52V
  				HX8357_SETOSC, 1, 0x68,  // Normal mode 70Hz, Idle mode 55 Hz
  				HX8357_SETPANEL, 1, 0x05,  // BGR, Gate direction swapped
  				HX8357_SETPWR1, 6, 0x00, 0x15, 0x1C, 0x1C, 0x83, 0xAA,
  				HX8357D_SETSTBA, 6, 0x50, 0x50, 0x01, 0x3C, 0x1E, 0x08,
  				// MEME GAMMA HERE
  				HX8357D_SETCYC, 7, 0x02, 0x40, 0x00, 0x2A, 0x2A, 0x0D, 0x78,
  				HX8357_COLMOD, 1, 0x55,
  				HX8357_MADCTL, 1, 0xC0,
  				HX8357_TEON, 1, 0x00,
  				HX8357_TEARLINE, 2, 0x00, 0x02,
  				HX8357_SLPOUT, 0,
  				TFTLCD_DELAY8, 150,
  				HX8357_DISPON, 0, 
  				TFTLCD_DELAY8, 50
			};
			init_table8(HX8357D_regValues, sizeof(HX8357D_regValues));
			break;
		case 0x7575:
		case 0x9595:
			lcd_driver = ID_7575;
			//WIDTH = 240,HEIGHT = 320;
			//width = WIDTH, height = HEIGHT;
			XC=0,YC=0,CC=0x22,RC=ILI932X_RW_GRAM,SC1=0x0E,SC2=0x14,MD=HX8347G_MEMACCESS,VL=1,R24BIT=1;
			static const uint8_t HX8347G_regValues[] PROGMEM = 
			{
				//  0xEA, 2, 0x00, 0x20,        //PTBA[15:0]
               //   0xEC, 2, 0x0C, 0xC4,   //
				  0x2E , 1 , 0x89,
        		  0x29 , 1 , 0x8F,
        		  0x2B , 1 , 0x02,
        		  0xE2 , 1 , 0x00,
        		  0xE4 , 1 , 0x01,
        		  0xE5 , 1 , 0x10,
        		  0xE6 , 1 , 0x01,
        		  0xE7 , 1 , 0x10,
        		  0xE8 , 1 , 0x70,   //0x70
        		  0xF2 , 1 , 0x00,   //0x00
        		  0xEA , 1 , 0x00,
        		  0xEB , 1 , 0x20,
        		  0xEC , 1 , 0x3C,
        		  0xED , 1 , 0xC8,
        		  0xE9 , 1 , 0x38, //0x38
        		  0xF1 , 1 , 0x01,

				 // 0x40, 13, 0x01, 0x00, 0x00, 0x10, 0x0E, 0x24, 0x04, 0x50, 0x02, 0x13, 0x19, 0x19, 0x16,  //
            	  //0x50, 14, 0x1B, 0x31, 0x2F, 0x3F, 0x3F, 0x3E, 0x2F, 0x7B, 0x09, 0x06, 0x06, 0x0C, 0x1D, 0xCC,  //
        
        		  // skip gamma, do later
        
        		  0x1B , 1 , 0x1A,  //0x1A
        		  0x1A , 1 , 0x01,  //0x01
        		  0x24 , 1 , 0x61,  //0x61
        		  0x25 , 1 , 0x5C,  //0x5C
        		  0x23 , 1 , 0x88,
        		  0x18 , 1 , 0x36,  //0x36
        		  0x19 , 1 , 0x01,
        		  0x1F , 1 , 0x88,
        		  TFTLCD_DELAY8 , 5   , // delay 5 ms
        		  0x1F , 1 , 0x80,
        		  TFTLCD_DELAY8   , 5   ,
        		  0x1F , 1 , 0x90,
        		  TFTLCD_DELAY8   , 5   ,
        		  0x1F , 1 , 0xD4,  //0xD4
        		  TFTLCD_DELAY8   , 5   ,
        		  0x17 , 1 , 0x05,
        
        		  0x36 , 1 , 0x00, //0x09
        		  0x28 , 1 , 0x38,
        		  TFTLCD_DELAY8   , 40  ,
        		  0x28 , 1 , 0x3C,  //0x3C
        		  0x02 , 1 , 0x00,
        		  0x03 , 1 , 0x00,
        		  0x04 , 1 , 0x00,
        		  0x05 , 1 , 0xEF,
        		  0x06 , 1 , 0x00,
        		  0x07 , 1 , 0x00,
        		  0x08 , 1 , 0x01,
        		  0x09 , 1 , 0x3F
			};
		    init_table8(HX8347G_regValues, sizeof(HX8347G_regValues));
			break;
		case 0x9486:
			lcd_driver = ID_9486;
			//WIDTH = 320,HEIGHT = 480;
			//width = WIDTH, height = HEIGHT;
			XC=ILI9341_COLADDRSET,YC=ILI9341_PAGEADDRSET,CC=ILI9341_MEMORYWRITE,RC=HX8357_RAMRD,SC1=0x33,SC2=0x37,MD=ILI9341_MADCTL,VL=0,R24BIT=0;
			static const uint8_t ILI9486_regValues[] PROGMEM = 
			{
				0xF7, 4, 0xA9,0x51,0x2C,0x82,
				0xC0, 2, 0x11,0x09,
				0xC1, 1, 0x41,
				0xC5, 3, 0x00,0x0A,0x80,
				0xB1, 2, 0xB0,0x11,
				0xB4, 1, 0x02,
				0xB6, 2, 0x02,0x22,
				0xB7, 1, 0xC6,
				0xBE, 2, 0x00,0x04,
				0xE9, 1, 0x00,
				0x36, 1, 0x08,
				0x3A, 1, 0x55,
				0xE0, 15, 0x00,0x07,0x10,0x09,0x17,0x0B,0x41,0x89,0x4B,0x0A,0x0C,0x0E,0x18,0x1B,0x0F,
				0xE1, 15, 0x00,0x17,0x1A,0x04,0x0E,0x06,0x2F,0x45,0x43,0x02,0x0A,0x09,0x32,0x36,0x0F,
				0x11, 0,            //Sleep Out
            	TFTLCD_DELAY8, 120,
            	0x29, 0         //Display On
				
			/*
				0x01, 0,            //Soft Reset
            	TFTLCD_DELAY8, 150,  // .kbv will power up with ONLY reset, sleep out, display on
            	0x28, 0,            //Display Off
            	0x3A, 1, 0x55,      //Pixel read=565, write=565.
            	0xC0, 2, 0x0d, 0x0d,        //Power Control 1 [0E 0E]
            	0xC1, 2, 0x43, 0x00,        //Power Control 2 [43 00]
            	0xC2, 1, 0x00,      //Power Control 3 [33]
            	0xC5, 4, 0x00, 0x48, 0x00, 0x48,    //VCOM  Control 1 [00 40 00 40]
            	0xB4, 1, 0x00,      //Inversion Control [00]
            	0xB6, 3, 0x02, 0x02, 0x3B,  // Display Function Control [02 02 3B] 
            	0xE0, 15,0x0F, 0x24, 0x1C, 0x0A, 0x0F, 0x08, 0x43, 0x88, 0x32, 0x0F, 0x10, 0x06, 0x0F, 0x07, 0x00,
            	0xE1, 15,0x0F, 0x38, 0x30, 0x09, 0x0F, 0x0F, 0x4E, 0x77, 0x3C, 0x07, 0x10, 0x05, 0x23, 0x1B, 0x00,
            	0x11, 0,            //Sleep Out
            	TFTLCD_DELAY8, 150,
            	0x29, 0         //Display On
            */
			};
			init_table8(ILI9486_regValues, sizeof(ILI9486_regValues));
			break;
		case 0x9488:
			lcd_driver = ID_9488;			
			if(MODEL == ILI9488_18)
			{
				static const uint8_t ILI9488_IPF[] PROGMEM ={0x3A,1,0x66};
				init_table8(ILI9488_IPF, sizeof(ILI9488_IPF));
			}
			else
			{
				static const uint8_t ILI9488_IPF[] PROGMEM ={0x3A,1,0x55};
				init_table8(ILI9488_IPF, sizeof(ILI9488_IPF));
			}
			//WIDTH = 320,HEIGHT = 480;
			//width = WIDTH, height = HEIGHT;
			XC=ILI9341_COLADDRSET,YC=ILI9341_PAGEADDRSET,CC=ILI9341_MEMORYWRITE,RC=HX8357_RAMRD,SC1=0x33,SC2=0x37,MD=ILI9341_MADCTL,VL=0,R24BIT=1;
			static const uint8_t ILI9488_regValues[] PROGMEM = 
			{
				0xF7, 4, 0xA9, 0x51, 0x2C, 0x82,
				0xC0, 2, 0x11, 0x09,
				0xC1, 1, 0x41,
				0xC5, 3, 0x00, 0x0A, 0x80,
				0xB1, 2, 0xB0, 0x11,
				0xB4, 1, 0x02,
				0xB6, 2, 0x02, 0x22,
				0xB7, 1, 0xC6,
				0xBE, 2, 0x00, 0x04,
				0xE9, 1, 0x00,
				0x36, 1, 0x08,
				0xE0, 15, 0x00, 0x07, 0x10, 0x09, 0x17, 0x0B, 0x41, 0x89, 0x4B, 0x0A, 0x0C, 0x0E, 0x18, 0x1B, 0x0F,
				0xE1, 15, 0x00, 0x17, 0x1A, 0x04, 0x0E, 0x06, 0x2F, 0x45, 0x43, 0x02, 0x0A, 0x09, 0x32, 0x36, 0x0F,
				0x11, 0,
				TFTLCD_DELAY8, 120,
				0x29, 0
			};
			init_table8(ILI9488_regValues, sizeof(ILI9488_regValues));
			break;
		case 0x7735:
			if(HEIGHT == 160)
			{
				lcd_driver = ID_7735;
			}
			else if(HEIGHT == 128)
			{
				lcd_driver = ID_7735_128;
			}
			//WIDTH = 128,HEIGHT = 160;
			//width = WIDTH, height = HEIGHT;
			XC=ILI9341_COLADDRSET,YC=ILI9341_PAGEADDRSET,CC=ILI9341_MEMORYWRITE,RC=HX8357_RAMRD,SC1=0x33,SC2=0x37,MD=ILI9341_MADCTL,VL=0,R24BIT=0;
			static const uint8_t ST7735S_regValues[] PROGMEM = 
			{
				0x11, 0,            
            	TFTLCD_DELAY8, 120,  
            	0xB1, 3, 0x05, 0x3C, 0x3C,          
            	0xB2, 3, 0x05, 0x3C, 0x3C,      
            	0xB3, 6, 0x05, 0x3C, 0x3C, 0x05, 0x3C, 0x3C,     
            	0xB4, 1, 0x03,        
            	0xC0, 3, 0x28, 0x08, 0x04,     
            	0xC1, 1, 0xC0,     
            	0xC2, 2, 0x0D, 0x00,      
            	0xC3, 2, 0x8D, 0x2A,   
            	0xC4, 2, 0x8D, 0xEE,
            	0xC5, 1, 0x1A,
            	0x17 , 1 , 0x05,
            	0x36, 1, 0x08,
            	0xE0, 16,0x03, 0x22, 0x07, 0x0A, 0x2E, 0x30, 0x25, 0x2A, 0x28, 0x26, 0x2E, 0x3A, 0x00, 0x01, 0x03, 0x13,
            	0xE1, 16,0x04, 0x16, 0x06, 0x0D, 0x2D, 0x26, 0x23, 0x27, 0x27, 0x25, 0x2D, 0x3B, 0x00, 0x01, 0x04, 0x13,         
            	//TFTLCD_DELAY8, 150,
            	0x3A, 1, 0x05,  
      //      	0x13, 0,
            	0x29, 0         
			};
			init_table8(ST7735S_regValues, sizeof(ST7735S_regValues));
			break;
		 case 0x1283:
		 	lcd_driver = ID_1283A;
			XC=0x45,YC=0x44,CC=0x22,RC=HX8357_RAMRD,SC1=0x41,SC2=0x42,MD=0x03,VL=1,R24BIT=0;
		 	static const uint16_t SSD1283A_regValues[] PROGMEM = 
			{
				0x10, 0x2F8E,            
            	0x11, 0x000C,  
            	0x07, 0x0021,           
            	0x28, 0x0006,      
            	0x28, 0x0005,     
            	0x27, 0x057F,        
            	0x29, 0x89A1,     
            	0x00, 0x0001,     
            	TFTLCD_DELAY16, 100,       
            	0x29, 0x80B0,   
            	TFTLCD_DELAY16, 30, 
            	0x29, 0xFFFE,
            	0x07, 0x0223,
            	TFTLCD_DELAY16, 30, 
            	0x07, 0x0233,
            	0x01, 0x2183,
            	0x03, 0x6830,
            	0x2F, 0xFFFF,
            	0x2C, 0x8000,
            	0x27, 0x0570,
            	0x02, 0x0300,
            	0x0B, 0x580C,
            	0x12, 0x0609,
            	0x13, 0x3100, 
			};
			init_table16(SSD1283A_regValues, sizeof(SSD1283A_regValues));
			break;
		case 0x1106:
		 	lcd_driver = ID_1106;
			XC=0x10,YC=0xB0,CC=0,RC=0,SC1=0,SC2=0,MD=0,VL=1,R24BIT=0;
			static const uint8_t SH1106_regValues[] PROGMEM = 
			{
				0x8D, 0,            
            	0x10, 0,  
            	0xAE, 0,           
            	0x02, 0,      
            	0x10, 0,     
            	0x40, 0,        
            	0x81, 0,     
            	0xCF, 0,         
            	0xA1, 0,   
            	0xC8, 0,
            	0xA6, 0,
            	0xA8, 0,
            	0x3F, 0,
            	0xD3, 0,
            	0x00, 0,
            	0xD5, 0,
            	0x80, 0,
            	0xD9, 0,
            	0xF1, 0,
            	0xDA, 0,
            	0x12, 0,
            	0xDB, 0,
            	0x40, 0,
            	0x20, 0,
            	0x02, 0,
            	0x8D, 0,
            	0x14, 0,
            	0xA4, 0,
            	0xA6, 0,
            	0xAF, 0,
			};
			init_table8(SH1106_regValues, sizeof(SH1106_regValues));
			break;
		default:
			lcd_driver = ID_UNKNOWN;
			break;		
	}
	Set_Rotation(rotation); 
	Invert_Display(false);
}

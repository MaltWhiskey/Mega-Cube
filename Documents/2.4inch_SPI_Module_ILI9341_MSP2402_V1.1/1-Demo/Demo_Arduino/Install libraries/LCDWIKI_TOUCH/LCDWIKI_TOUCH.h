// IMPORTANT: LIBRARY MUST BE SPECIFICALLY CONFIGURED FOR EITHER TFT SHIELD
// OR BREAKOUT BOARD USAGE.

// Lcdwiki TOUCH library
// MIT license


#ifndef _LCDWIKI_TOUCH_H_
#define _LCDWIKI_TOUCH_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define CMD_RDX 0XD0
#define CMD_RDY 0X90

#define TP_PRES_DOWN 0x80  
#define TP_CATH_PRES 0x40  


class LCDWIKI_TOUCH
{
  public:
  	LCDWIKI_TOUCH(int8_t tcs, int8_t tclk, int8_t tout, int8_t tdin, int8_t tirq);//sofware spi
	void TP_Init(uint8_t r,uint16_t w, uint16_t h);
	uint8_t TP_Scan(uint8_t mode);
	void TP_Write_Byte(uint8_t data);
	uint16_t TP_Read_ADC(uint8_t cmd);
	uint16_t TP_Read_XY(uint8_t xy);
	uint8_t TP_Read_Coordinate(uint16_t *x, uint16_t *y);
	uint8_t TP_Read_Coordinate2(uint16_t *x, uint16_t *y);
	void TP_Set_Rotation(uint8_t val);
	uint8_t TP_Get_Rotation(void) const;	
	void LCD_Set_Rotation(uint8_t val);
	uint8_t LCD_Get_Rotation(void) const;	
	void TP_Set_State(uint8_t val);
	uint8_t TP_Get_State(void) const;	
	
	uint16_t x0,y0,x,y,wid,heig;
	uint8_t touch_statue,touch_rotation,lcd_rotation;	
  private:
 	#ifdef __AVR__
		volatile uint8_t *tcsPort, *tclkPort, *tdoutPort, *tdinPort, *tirqPort;
		uint8_t  tcsPinSet, tclkPinSet,	tdoutPinSet, tdinPinSet, tirqPinSet;
	#endif
};

#endif

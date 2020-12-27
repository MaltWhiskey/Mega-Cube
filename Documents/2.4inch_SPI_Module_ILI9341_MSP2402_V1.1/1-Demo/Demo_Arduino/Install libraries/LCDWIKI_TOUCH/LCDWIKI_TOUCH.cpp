// IMPORTANT: LIBRARY MUST BE SPECIFICALLY CONFIGURED FOR EITHER TFT SHIELD
// OR BREAKOUT BOARD USAGE.

// Lcdwiki TOUCH library
// MIT license


#include "LCDWIKI_TOUCH.h"
#include "mcu_touch_magic.h"
#include "cali_para.h"

static void delay_us(u32 us)
{
    u32 time=100*us/7;    
    while(--time);   
}
 	 			    			
LCDWIKI_TOUCH::LCDWIKI_TOUCH(int8_t tcs, int8_t tclk, int8_t tdout, int8_t tdin, int8_t tirq)
{
	// Convert pin numbers to registers and bitmasks
#ifdef __AVR__
	tcsPort	 = portOutputRegister(digitalPinToPort(tcs));
	tclkPort = portOutputRegister(digitalPinToPort(tclk));
	tdoutPort = portInputRegister(digitalPinToPort(tdout));
	tdinPort = portOutputRegister(digitalPinToPort(tdin));
	tirqPort = portInputRegister(digitalPinToPort(tirq));

	tcsPinSet  = digitalPinToBitMask(tcs);
	tclkPinSet = digitalPinToBitMask(tclk);
	tdoutPinSet = digitalPinToBitMask(tdout);
	tdinPinSet = digitalPinToBitMask(tdin);
	tirqPinSet = digitalPinToBitMask(tirq);
	
	pinMode(tcs, OUTPUT);	  // Enable outputs
	pinMode(tclk, OUTPUT);
	pinMode(tdout, INPUT);
	pinMode(tdin, OUTPUT);
	pinMode(tirq, INPUT);	
#endif
	touch_statue = 0 ;
	touch_rotation = 0;	
	lcd_rotation = 0;
	wid = 0;
	heig = 0;
	x0=0;
	y0=0;
	x=0xffff;
	y=0xffff;
}

void LCDWIKI_TOUCH::TP_Init(uint8_t r,uint16_t w, uint16_t h)
{
	TDIN_HIGH;
	TCLK_HIGH; 
	TCS_HIGH;
	wid = w;
	heig = h;	
	LCD_Set_Rotation(r);
}

void LCDWIKI_TOUCH::TP_Write_Byte(uint8_t data)
{
	uint8_t val = 0x80;
	while(val)
	{
		if(data&val)
		{
			TDIN_HIGH; 
		}
		else
		{
			TDIN_LOW;
		}
		TCLK_LOW;
		TCLK_HIGH;
		val >>= 1;
	}
}

uint16_t LCDWIKI_TOUCH::TP_Read_ADC(uint8_t cmd)
{
	uint16_t num=0; 
	uint8_t count=0; 
	TCS_LOW; 
	TCLK_LOW;           
	TDIN_LOW;
	TP_Write_Byte(cmd);
	//delay_us(6);              
	TCLK_LOW; 
	delay_us(1); 
	TCLK_HIGH;
	TCLK_LOW;
	for(count=0;count<16;count++)  
	{   
	    num<<=1;          
	    TCLK_LOW;                         
	    TCLK_HIGH;
	    if(TDOUT_STATE)
	    {
	         num += 1;                  
	    } 
		else
		{
			num += 0; 
		}
	}
	num>>=4;   // the high 12 bits is valid
    TCS_HIGH;   
   	return num; 
}

#define READ_TIMES 5 //the times for read
#define LOST_VAL 1          //
uint16_t LCDWIKI_TOUCH::TP_Read_XY(uint8_t xy)
{
 	uint16_t i, j;
 	uint16_t buf[READ_TIMES];
 	uint16_t sum=0;
 	uint16_t temp;    
 	for(i=0;i<READ_TIMES;i++)
 	{                                 
  		buf[i]=TP_Read_ADC(xy);            
    }                                    
	for(i=0;i<READ_TIMES-1; i++)//Sort in ascending order
 	{
  		for(j=i+1;j<READ_TIMES;j++)
       	{
        	if(buf[i]>buf[j])
            {
             	temp=buf[i];
             	buf[i]=buf[j];
             	buf[j]=temp;
          	}
      	}
  	}          
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++) //Remove maximum and minimum values
	{
		sum+=buf[i];
	}
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 

uint8_t LCDWIKI_TOUCH::TP_Read_Coordinate(uint16_t *x,uint16_t *y)
{
        uint16_t xtemp,ytemp;                                                    
        xtemp=TP_Read_XY(CMD_RDX);
        ytemp=TP_Read_XY(CMD_RDY);
      //  if(xtemp<100||ytemp<100)return 0;//Reading failed
        *x=xtemp;
        *y=ytemp;
        return 1;//Reading success
}        

#define ERR_RANGE 50 //error range
uint8_t LCDWIKI_TOUCH::TP_Read_Coordinate2(uint16_t *x,uint16_t *y) 
{
  	uint16_t x1,y1;
  	uint16_t x2,y2;
  	uint8_t flag;    
    flag=TP_Read_Coordinate(&x1,&y1);  
    if(flag==0)
    {
		return 0;
    }
	flag=TP_Read_Coordinate(&x2,&y2);
    if(flag==0)
    {
		return 0;   
    }
	if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//Before and after the two samples are within +- ERR_RANGE.
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }
	else
	{
      	return 0;          
  	}        
} 

uint8_t LCDWIKI_TOUCH::TP_Scan(uint8_t mode)
{
	if(TIRQ_STATE==0) 
	{
		if(mode) //Physical coordinates
		{
			TP_Read_Coordinate2(&x,&y);
		}
		else if(TP_Read_Coordinate2(&x,&y)) //screen coordinate
		{
		/*
			long temp;
			temp = (long)XFAC*x/10000;
			x=temp+XOFFSET;
			temp = (long)YFAC*y/10000;
		    y=temp+YOFFSET;
	   */
			x=((long)XFAC*x)/10000+XOFFSET;
		    y=((long)YFAC*y)/10000+YOFFSET;
			switch(touch_rotation)
			{
				case 0:
					if(lcd_rotation == 0)
					{
						break;
					}
					else if(lcd_rotation == 1)
					{
						uint16_t tmp;
						tmp = x;
						x=y;
						y=tmp;
						y = heig - y;
					}
					else if(lcd_rotation == 2)
					{
						x = wid-x;
						y = heig - y;
					}
					else if(lcd_rotation == 3)
					{
						uint16_t tmp;
						tmp = x;
						x=y;
						y=tmp;
						x = wid-x;
					}
					break;
				case 1:
					if(lcd_rotation == 0)
					{
						x = wid-x;
					}
					else if(lcd_rotation == 1)
					{
						uint16_t tmp;
						tmp = x;
						x=y;
						y=tmp;
					}
					else if(lcd_rotation == 2)
					{
						y = heig - y;
					}
					else if(lcd_rotation == 3)
					{
						uint16_t tmp;
						tmp = x;
						x=y;
						y=tmp;
						x = wid-x;
						y = heig - y;
					}
					break;
				case 2:
					if(lcd_rotation == 0)
					{
						x = wid-x;
						y = heig - y;
					}
					else if(lcd_rotation == 1)
					{
						uint16_t tmp;
						tmp = x;
						x=y;
						y=tmp;
						x = wid-x;
					}
					else if(lcd_rotation == 2)
					{
						break;
					}
					else if(lcd_rotation == 3)
					{
						uint16_t tmp;
						tmp = x;
						x=y;
						y=tmp;
						y = heig - y;
					}
					break;
				case 3:
					if(lcd_rotation == 0)
					{
						y = heig - y;
					}
					else if(lcd_rotation == 1)
					{
						uint16_t tmp;
						tmp = x;
						x=y;
						y=tmp;
						x = wid-x;
						y = heig - y;
					}
					else if(lcd_rotation == 2)
					{
						x = wid-x;;
					}
					else if(lcd_rotation == 3)
					{
						uint16_t tmp;
						tmp = x;
						x=y;
						y=tmp;
					}
					break;
				default:
					break;
			}
	 	}
		if((touch_statue&TP_PRES_DOWN)==0)
		{		 
			touch_statue=TP_PRES_DOWN|TP_CATH_PRES; 
			x0=x;
			y0=y;  	   			 
		}			   
	}
	else
	{
		if(touch_statue&TP_PRES_DOWN)
		{
			touch_statue&=~(1<<7);
		}
		else
		{
			x0=0;
			y0=0;
			x=0xffff;
			y=0xffff;
		}	    
	}
	return touch_statue&TP_PRES_DOWN;
}

void LCDWIKI_TOUCH::TP_Set_Rotation(uint8_t val)
{
	touch_rotation = val;
}

uint8_t LCDWIKI_TOUCH::TP_Get_Rotation(void) const
{
	return touch_rotation;
}

void LCDWIKI_TOUCH::LCD_Set_Rotation(uint8_t val)
{
	lcd_rotation = val;
}

uint8_t LCDWIKI_TOUCH::LCD_Get_Rotation(void) const
{
	return lcd_rotation;
}

void LCDWIKI_TOUCH::TP_Set_State(uint8_t val)
{
	touch_statue = val;
}

uint8_t LCDWIKI_TOUCH::TP_Get_State(void) const
{
	return touch_statue;
}


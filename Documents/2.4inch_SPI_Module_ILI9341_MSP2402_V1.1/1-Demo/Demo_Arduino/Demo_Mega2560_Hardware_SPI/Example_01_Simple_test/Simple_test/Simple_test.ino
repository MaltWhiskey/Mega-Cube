//This application does not rely on any libraries and it is for ILI9341

//This program is a demo of clearing screen to display black,white,red,green,blue.

//when using the BREAKOUT BOARD only and using these hardware spi lines to the LCD,
//the SDA pin and SCK pin is defined by the system and can't be modified.
//if you don't need to control the LED pin,you can set it to 3.3V and set the pin definition to -1.
//other pins can be defined by youself,for example
//pin usage as follow:
//                  CS  DC/RS  RESET  SDI/MOSI  SCK   LED    VCC     GND    
//Arduino Mega2560  A5   A3     A4       51     52    A0   5V/3.3V   GND

//Remember to set the pins to suit your display module!
/********************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE 
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**********************************************************************************/
#include <SPI.h>

#define LED   A0    
#define CS    A5        
#define RS    A3       
#define RESET A4

void Lcd_Writ_Bus(unsigned char d)
{
  SPI.transfer(d);
}

void Lcd_Write_Com(unsigned char VH)  
{   
  *(portOutputRegister(digitalPinToPort(RS))) &=  ~digitalPinToBitMask(RS);//LCD_RS=0;
  Lcd_Writ_Bus(VH);
}

void Lcd_Write_Data(unsigned char VH)
{
  *(portOutputRegister(digitalPinToPort(RS)))|=  digitalPinToBitMask(RS);//LCD_RS=1;
  Lcd_Writ_Bus(VH);
}

void Lcd_Write_Com_Data(unsigned char com,unsigned char dat)
{
  Lcd_Write_Com(com);
  Lcd_Write_Data(dat);
}

void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{
        Lcd_Write_Com(0x2a);
	Lcd_Write_Data(x1>>8);
	Lcd_Write_Data(x1);
	Lcd_Write_Data(x2>>8);
	Lcd_Write_Data(x2);
        Lcd_Write_Com(0x2b);
	Lcd_Write_Data(y1>>8);
	Lcd_Write_Data(y1);
	Lcd_Write_Data(y2>>8);
	Lcd_Write_Data(y2);
	Lcd_Write_Com(0x2c); 							 
}

void SPI_Init(void)
{
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz (half speed)
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);  
}

void Lcd_Init(void)
{
  digitalWrite(RESET,HIGH);
  delay(5); 
  digitalWrite(RESET,LOW);
  delay(15);
  digitalWrite(RESET,HIGH);
  delay(15);

  digitalWrite(CS,LOW);  //CS

    Lcd_Write_Com(0xCB);  
    Lcd_Write_Data(0x39); 
    Lcd_Write_Data(0x2C); 
    Lcd_Write_Data(0x00); 
    Lcd_Write_Data(0x34); 
    Lcd_Write_Data(0x02); 

    Lcd_Write_Com(0xCF);  
    Lcd_Write_Data(0x00); 
    Lcd_Write_Data(0XC1); 
    Lcd_Write_Data(0X30); 

    Lcd_Write_Com(0xE8);  
    Lcd_Write_Data(0x85); 
    Lcd_Write_Data(0x00); 
    Lcd_Write_Data(0x78); 

    Lcd_Write_Com(0xEA);  
    Lcd_Write_Data(0x00); 
    Lcd_Write_Data(0x00); 
 
    Lcd_Write_Com(0xED);  
    Lcd_Write_Data(0x64); 
    Lcd_Write_Data(0x03); 
    Lcd_Write_Data(0X12); 
    Lcd_Write_Data(0X81); 

    Lcd_Write_Com(0xF7);  
    Lcd_Write_Data(0x20); 
  
    Lcd_Write_Com(0xC0);    //Power control 
    Lcd_Write_Data(0x23);   //VRH[5:0] 
 
    Lcd_Write_Com(0xC1);    //Power control 
    Lcd_Write_Data(0x10);   //SAP[2:0];BT[3:0] 

    Lcd_Write_Com(0xC5);    //VCM control 
    Lcd_Write_Data(0x3e);   //Contrast
    Lcd_Write_Data(0x28); 
 
    Lcd_Write_Com(0xC7);    //VCM control2 
    Lcd_Write_Data(0x86);   //--
 
    Lcd_Write_Com(0x36);    // Memory Access Control 
    Lcd_Write_Data(0x48);   

    Lcd_Write_Com(0x3A);    
    Lcd_Write_Data(0x55); 

    Lcd_Write_Com(0xB1);    
    Lcd_Write_Data(0x00);  
    Lcd_Write_Data(0x18); 
 
    Lcd_Write_Com(0xB6);    // Display Function Control 
    Lcd_Write_Data(0x08); 
    Lcd_Write_Data(0x82);
    Lcd_Write_Data(0x27);  

    Lcd_Write_Com(0x11);    //Exit Sleep 
    delay(120); 
				
    Lcd_Write_Com(0x29);    //Display on 
    Lcd_Write_Com(0x2c); 
    digitalWrite(CS,HIGH);
}

void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c)                   
{	
  unsigned int i,j;
  digitalWrite(CS,LOW);
  Lcd_Write_Com(0x02c); //write_memory_start
  //digitalWrite(RS,HIGH);
  l=l+x;
  Address_set(x,y,l,y);
  j=l*2;
  for(i=1;i<=j;i++)
  {
    Lcd_Write_Data(c);
  }
  digitalWrite(CS,HIGH);   
}

void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c)                   
{	
  unsigned int i,j;
  digitalWrite(CS,LOW);
  Lcd_Write_Com(0x02c); //write_memory_start
  //digitalWrite(RS,HIGH);
  l=l+y;
  Address_set(x,y,x,l);
  j=l*2;
  for(i=1;i<=j;i++)
  { 
    Lcd_Write_Data(c);
  }
  digitalWrite(CS,HIGH);   
}

void Rect(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)
{
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}

void Rectf(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)
{
  unsigned int i;
  for(i=0;i<h;i++)
  {
    H_line(x  , y  , w, c);
    H_line(x  , y+i, w, c);
  }
}

int RGB(int r,int g,int b)
{
  return r << 16 | g << 8 | b;
}

void LCD_Clear(unsigned int j)                   
{	
  unsigned int i,m;
  digitalWrite(CS,LOW);
  Address_set(0,0,240,320);
  for(i=0;i<240;i++)
    for(m=0;m<320;m++)
    {
      Lcd_Write_Data(j>>8);
      Lcd_Write_Data(j);
    }
  digitalWrite(CS,HIGH);   
}

void setup()
{
  SPI_Init();
  pinMode(A0,OUTPUT);
  pinMode(A3,OUTPUT);
  pinMode(A4,OUTPUT);
  pinMode(A5,OUTPUT);

  digitalWrite(A0, HIGH);
  digitalWrite(A3, HIGH);
  digitalWrite(A4, HIGH);
  digitalWrite(A5, HIGH);

  Lcd_Init();
  
}

void loop()
{  
   LCD_Clear(0xf800);
   LCD_Clear(0x07E0);
   LCD_Clear(0x001F);
   LCD_Clear(0x0); 
  for(int i=0;i<500;i++)
  {
    Rect(random(300),random(300),random(300),random(300),random(65535)); // rectangle at x, y, with, hight, color
  }
  
//  LCD_Clear(0xf800);
}

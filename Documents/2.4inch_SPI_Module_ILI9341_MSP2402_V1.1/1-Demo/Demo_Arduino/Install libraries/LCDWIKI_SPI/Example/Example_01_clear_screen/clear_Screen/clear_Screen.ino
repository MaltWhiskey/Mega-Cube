// IMPORTANT: LCDWIKI_SPI LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.

//This program is a demo of clearing screen to display black,white,red,green,blue.

//Set the pins to the correct ones for your development shield or breakout board.
//when using the BREAKOUT BOARD only and using these software spi lines to the LCD,
//there is no MISO pin and You can use any free pin to define the pins,for example
//pin usage as follow:
//             CS  CD  RST  MOSI  MISO  CLK  LED 
//Arduino Uno  A5  A3  A4   A2    NONE  A1   A3
//Arduino Mega A5  A3  A4   A2    NONE  A1   A3

//when using the BREAKOUT BOARD only and using these hardware spi lines to the LCD,
//there is no MISO pin 
//the MOSI pin and CLK pin is defined by the system and can't be modified.
//other pins can be defined by youself,for example
//pin usage as follow:
//             CS  CD  RST  MOSI  MISO  CLK  LED  
//Arduino Uno  10  9   8    11    NONE  13   A3
//Arduino Mega 10  9   8    51    NONE  52   A3

//Remember to set the pins to suit your display module!

#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_SPI.h> //Hardware-specific library

//the definiens of software spi mode as follow:
//if the IC model is known or the modules is unreadable,you can use this constructed function
//LCDWIKI_SPI mylcd(ST7735S,A5,A3,-1,A2,A4,A1,A3);//software spi,model,cs,cd,miso,mosi,reset,clk,led
//LCDWIKI_SPI mylcd(SSD1283A,A5,A3,-1,A2,A4,A1,A3);//software spi,model,cs,cd,miso,mosi,reset,clk,led

//if the IC model is not known and the modules is readable,you can use this constructed function
//LCDWIKI_SPI mylcd(160,128,A5,A3,-1,A2,A4,A1,A3);//software spi,model,cs,cd,miso,mosi,reset,clk
//LCDWIKI_SPI mylcd(130,130,A5,A3,-1,A2,A4,A1,A3);//software spi,model,cs,cd,miso,mosi,reset,clk

//the definiens of hardware spi mode as follow:
//if the IC model is known or the modules is unreadable,you can use this constructed function
//LCDWIKI_SPI mylcd(ST7735S,10,9,8,A3); //hardware spi,cs,cd,reset
//LCDWIKI_SPI mylcd(SSD1283A,10,9,8,A3); //hardware spi,cs,cd,reset

//if the IC model is not known and the modules is readable,you can use this constructed function
//LCDWIKI_SPI mylcd(160,128,10,9,8,A3); //hardware spi,cs,cd,reset
//LCDWIKI_SPI mylcd(130,130,10,9,8,A3); //hardware spi,cs,cd,reset

void setup() 
{
    mylcd.Init_LCD();
    mylcd.Fill_Screen(0x0000);
    mylcd.Fill_Screen(0xFFFF);
}

void loop() 
{    
    mylcd.Fill_Screen(0,0,0);
    mylcd.Fill_Screen(255,255,255);
    mylcd.Fill_Screen(255,0,0); 
    mylcd.Fill_Screen(0,255,0);
    mylcd.Fill_Screen(0,0,255);
    delay(3000);
    mylcd.Fill_Screen(0,0,0);
    delay(1000);
    mylcd.Fill_Screen(255,255,255);
    delay(1000);
    mylcd.Fill_Screen(0xF800);
    delay(1000);
    mylcd.Fill_Screen(0x07E0);
   delay(1000);
   mylcd.Fill_Screen(0x001F);
   delay(3000);
}

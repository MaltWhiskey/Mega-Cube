// IMPORTANT: LCDWIKI_SPI LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.

//This program is a demo of clearing screen to display black,white,red,green,blue.

//when using the BREAKOUT BOARD only and using these hardware spi lines to the LCD,
//the SDA pin and SCK pin is defined by the system and can't be modified.
//if you don't need to control the LED pin,you can set it to 3.3V and set the pin definition to -1.
//other pins can be defined by youself,for example
//pin usage as follow:
//             CS  DC/RS  RESET  SDI/MOSI  SDO/MISO  SCK  LED    VCC     GND    
//Arduino Uno  A5   A3     A4      11        12      13   A0   5V/3.3V   GND

//Remember to set the pins to suit your display module!
/***********************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE 
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**********************************************************************************/
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_SPI.h> //Hardware-specific library

//paramters define
#define MODEL ILI9341
#define CS    A5    
#define CD    A3
#define RST   A4
#define MOSI  11
#define MISO  12
#define SCK   13
#define LED   A0   //if you don't need to control the LED pin,you should set it to -1 and set it to 3.3V

//the definiens of software spi mode as follow:
//if the IC model is known or the modules is unreadable,you can use this constructed function
LCDWIKI_SPI mylcd(MODEL,CS,CD,MISO,MOSI,RST,SCK,LED); //model,cs,dc,miso,mosi,reset,sck,led
//if the IC model is not known and the modules is readable,you can use this constructed function
//LCDWIKI_SPI mylcd(240,320,CS,CD,MISO,MOSI,RST,SCK,LED); //width,height,cs,dc,miso,mosi,reset,sck,led

void setup() 
{
    mylcd.Init_LCD(); //initialize lcd
    mylcd.Fill_Screen(0xFFFF); //display white
}

void loop() 
{ 
    //Sequential display black,white,red,green,blue
    mylcd.Fill_Screen(0,0,0);  
    mylcd.Fill_Screen(255,255,255); 
    mylcd.Fill_Screen(255,0,0); 
    mylcd.Fill_Screen(0,255,0);
    mylcd.Fill_Screen(0,0,255);
    delay(3000);
    mylcd.Fill_Screen(0x0000);
    delay(1000);
    mylcd.Fill_Screen(0xFFFF);
    delay(1000);
    mylcd.Fill_Screen(0xF800);
    delay(1000);
    mylcd.Fill_Screen(0x07E0);
   delay(1000);
   mylcd.Fill_Screen(0x001F);
   delay(1000);
}

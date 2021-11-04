// IMPORTANT: LCDWIKI_SPI LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.

//This program is a demo of how to read color data from speciallied point

//when using the BREAKOUT BOARD only and using these hardware spi lines to the LCD,
//the SDA pin and SCK pin is defined by the system and can't be modified.
//if you don't need to control the LED pin,you can set it to 3.3V and set the pin definition to -1.
//other pins can be defined by youself,for example
//pin usage as follow:
//                  CS  DC/RS  RESET  SDI/MOSI  SDO/MISO  SCK  LED    VCC     GND    
//Arduino Mega2560  A5   A3     A4      51        50      52   A0   5V/3.3V   GND

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
#define MOSI  51
#define MISO  50
#define SCK   52
#define LED   A0   //if you don't need to control the LED pin,you should set it to -1 and set it to 3.3V

//the definiens of software spi mode as follow:
//if the IC model is known or the modules is unreadable,you can use this constructed function
LCDWIKI_SPI my_lcd(MODEL,CS,CD,MISO,MOSI,RST,SCK,LED); //model,cs,dc,miso,mosi,reset,sck,led
//if the IC model is not known and the modules is readable,you can use this constructed function
//LCDWIKI_SPI my_lcd(240,320,CS,CD,MISO,MOSI,RST,SCK,LED); //width,height,cs,dc,miso,mosi,reset,sck,led

#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

//get the color data from speciallied point
void color_dump(uint16_t x,uint16_t y)
{
    uint8_t buf[30] = {0},pbuf[10] = {0};
    uint8_t wd = (my_lcd.Get_Display_Width() - 9 * 6)/ (5 * 6);
    uint8_t hi = (my_lcd.Get_Display_Height() / 8) - 1;
    uint16_t pixel = 0;
    //set white
    my_lcd.Set_Text_colour(WHITE);
    //set text size 1
    my_lcd.Set_Text_Size(1);
    for(int j = 0;j< hi;j++)
    {
      sprintf(buf,"%3d,%3d:",x,y+j);
      my_lcd.Print_String(buf, 0, 8*(j+1)*my_lcd.Get_Text_Size());
      for(int i=0;i<wd;i++)
      {          
//          my_lcd.Print_String(buf, 0, line+8);
        //read pixel
          pixel = my_lcd.Read_Pixel(x+i,y+j);

        // if white set green
          if(WHITE == pixel)
          {
             my_lcd.Set_Text_colour(GREEN); 
           }
           sprintf(pbuf,"%04X ",pixel); 
           my_lcd.Print_String(pbuf,(strlen(buf)+strlen(pbuf)*i)*6*my_lcd.Get_Text_Size(),8*(j+1)*my_lcd.Get_Text_Size());
        //set white
           my_lcd.Set_Text_colour(WHITE);
        } 
     }
}   

uint8_t aspect;
char *aspectname[] = {"PORTRAIT", "LANDSCAPE", "PORTRAIT_REV", "LANDSCAPE_REV"};
uint16_t colors[] = {BLACK, BLUE};

void setup()
{
  my_lcd.Init_LCD();
//my_lcd.Set_Text_Back_colour(BLACK);
}

void loop() 
{
  uint16_t iter, color;
    char buf[80];
    aspect = (aspect + 1) & 3;
    my_lcd.Set_Rotation(aspect);
 //color_dump(36,0);
 
    for (iter = 0; iter < sizeof(colors) / sizeof(uint16_t); iter++) 
    {
        color = colors[iter];
        my_lcd.Fill_Screen(color);
        my_lcd.Set_Text_Back_colour(color);
        my_lcd.Set_Text_colour(WHITE);
        my_lcd.Set_Text_Size(1);
     sprintf(buf, " ID=0x%04X Background=%04X %s", my_lcd.Read_ID(), color, aspectname[aspect]);
      my_lcd.Print_String(buf,0,0);
        color_dump(36,0);
        delay(3000);
       
    } 

}

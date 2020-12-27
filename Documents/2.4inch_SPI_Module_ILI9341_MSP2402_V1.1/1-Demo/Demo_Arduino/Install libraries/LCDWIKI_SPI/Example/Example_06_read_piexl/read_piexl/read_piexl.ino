// IMPORTANT: LCDWIKI_SPI LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.

//This program is a demo of how to read color value from GRAM.

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

#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

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

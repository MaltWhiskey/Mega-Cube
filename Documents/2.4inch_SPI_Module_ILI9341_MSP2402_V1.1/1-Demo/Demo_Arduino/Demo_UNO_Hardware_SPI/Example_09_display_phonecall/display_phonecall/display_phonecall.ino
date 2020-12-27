// IMPORTANT: LCDWIKI_SPI LIBRARY AND LCDWIKI_TOUCH LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.

//This program is a demo of display phonecall UI

//when using the BREAKOUT BOARD only and using these hardware spi lines to the LCD,
//the SDA pin and SCK pin is defined by the system and can't be modified.
//if you don't need to control the LED pin,you can set it to 3.3V and set the pin definition to -1.
//other pins can be defined by youself,for example
//pin usage as follow:
//             CS  DC/RS  RESET  SDI/MOSI  SDO/MISO  SCK  LED    VCC     GND    
//Arduino Uno  A5   A3     A4      11        12      13   A0   5V/3.3V   GND
//            T_IRQ  T_DO  T_DIN  T_CS  T_CLK
//Arduino Uno  6      4      5     2      3

//Remember to set the pins to suit your display module!

/*********************************************************************************
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
#include <LCDWIKI_TOUCH.h> //touch screen library

//paramters define
#define MODEL ILI9341
#define CS   A5    
#define CD   A3
#define RST  A4
#define LED  A0   //if you don't need to control the LED pin,you should set it to -1 and set it to 3.3V

//touch screen paramters define
#define TCS   2
#define TCLK  3
#define TDOUT 4
#define TDIN  5
#define TIRQ  6

//the definiens of hardware spi mode as follow:
//if the IC model is known or the modules is unreadable,you can use this constructed function
LCDWIKI_SPI my_lcd(MODEL,CS,CD,RST,LED); //model,cs,dc,reset,led
//if the IC model is not known and the modules is readable,you can use this constructed function
//LCDWIKI_SPI my_lcd(240,320,CS,CD,RST,LED); //model,cs,dc,reset,led

//the definiens of touch mode as follow:
LCDWIKI_TOUCH my_touch(TCS,TCLK,TDOUT,TDIN,TIRQ); //tcs,tclk,tdout,tdin,tirq

                             /*  r     g    b */
#define BLACK        0x0000  /*   0,   0,   0 */
#define BLUE         0x001F  /*   0,   0, 255 */
#define RED          0xF800  /* 255,   0,   0 */
#define GREEN        0x07E0  /*   0, 255,   0 */
#define CYAN         0x07FF  /*   0, 255, 255 */
#define MAGENTA      0xF81F  /* 255,   0, 255 */
#define YELLOW       0xFFE0  /* 255, 255,   0 */
#define WHITE        0xFFFF  /* 255, 255, 255 */
#define NAVY         0x000F  /*   0,   0, 128 */
#define DARKGREEN    0x03E0  /*   0, 128,   0 */
#define DARKCYAN     0x03EF  /*   0, 128, 128 */
#define MAROON       0x7800  /* 128,   0,   0 */
#define PURPLE       0x780F  /* 128,   0, 128 */
#define OLIVE        0x7BE0  /* 128, 128,   0 */
#define LIGHTGREY    0xC618  /* 192, 192, 192 */
#define DARKGREY     0x7BEF  /* 128, 128, 128 */
#define ORANGE       0xFD20  /* 255, 165,   0 */
#define GREENYELLOW  0xAFE5  /* 173, 255,  47 */
#define PINK         0xF81F  /* 255,   0, 255 */

/******************* UI details */
#define BUTTON_R 25
#define BUTTON_SPACING_X 25
#define BUTTON_SPACING_Y 5
#define EDG_Y 5
#define EDG_X 20

uint16_t px,py;


// We have a status line for like, is FONA working
#define STATUS_X 10
#define STATUS_Y 65


typedef struct _button_info
{
     uint8_t button_name[10];
     uint8_t button_name_size;
     uint16_t button_name_colour;
     uint16_t button_colour;
     uint16_t button_x;
     uint16_t button_y;     
 }button_info;

button_info phone_button[15] = 
{
  "1",3,BLACK,CYAN,EDG_X+BUTTON_R-1,my_lcd.Get_Display_Height()-EDG_Y-4*BUTTON_SPACING_Y-9*BUTTON_R-1,
  "2",3,BLACK,CYAN,EDG_X+3*BUTTON_R+BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-4*BUTTON_SPACING_Y-9*BUTTON_R-1,
  "3",3,BLACK,CYAN,EDG_X+5*BUTTON_R+2*BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-4*BUTTON_SPACING_Y-9*BUTTON_R-1,
  "4",3,BLACK,CYAN,EDG_X+BUTTON_R-1,my_lcd.Get_Display_Height()-EDG_Y-3*BUTTON_SPACING_Y-7*BUTTON_R-1, 
  "5",3,BLACK,CYAN,EDG_X+3*BUTTON_R+BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-3*BUTTON_SPACING_Y-7*BUTTON_R-1,
  "6",3,BLACK,CYAN,EDG_X+5*BUTTON_R+2*BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-3*BUTTON_SPACING_Y-7*BUTTON_R-1,
  "7",3,BLACK,CYAN,EDG_X+BUTTON_R-1,my_lcd.Get_Display_Height()-EDG_Y-2*BUTTON_SPACING_Y-5*BUTTON_R-1,
  "8",3,BLACK,CYAN,EDG_X+3*BUTTON_R+BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-2*BUTTON_SPACING_Y-5*BUTTON_R-1,
  "9",3,BLACK,CYAN,EDG_X+5*BUTTON_R+2*BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-2*BUTTON_SPACING_Y-5*BUTTON_R-1,
  "*",3,BLACK,PINK,EDG_X+BUTTON_R-1,my_lcd.Get_Display_Height()-EDG_Y-BUTTON_SPACING_Y-3*BUTTON_R-1,
  "0",3,BLACK,CYAN,EDG_X+3*BUTTON_R+BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-BUTTON_SPACING_Y-3*BUTTON_R-1,
  "#",3,BLACK,PINK,EDG_X+5*BUTTON_R+2*BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-BUTTON_SPACING_Y-3*BUTTON_R-1,
  "end",2,BLACK,RED,EDG_X+BUTTON_R-1,my_lcd.Get_Display_Height()-EDG_Y-BUTTON_R-1,
  "call",2,BLACK,GREEN,EDG_X+3*BUTTON_R+BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-BUTTON_R-1,
  "dele",2,BLACK,LIGHTGREY,EDG_X+5*BUTTON_R+2*BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-BUTTON_R-1,
};

uint16_t text_x=7,text_y=6,text_x_add = 6*phone_button[0].button_name_size,text_y_add = 8*phone_button[0].button_name_size;
uint16_t n=0;

void show_string(uint8_t *str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
{
    my_lcd.Set_Text_Mode(mode);
    my_lcd.Set_Text_Size(csize);
    my_lcd.Set_Text_colour(fc);
    my_lcd.Set_Text_Back_colour(bc);
    my_lcd.Print_String(str,x,y);
}

boolean is_pressed(int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t px,int16_t py)
{
    if((px > x1 && px < x2) && (py > y1 && py < y2))
    {
        return true;  
    } 
    else
    {
        return false;  
    }
 }

void show_menu(void)
{
    uint16_t i;
   for(i = 0;i < sizeof(phone_button)/sizeof(button_info);i++)
   {
      my_lcd.Set_Draw_color(phone_button[i].button_colour);
      my_lcd.Fill_Circle(phone_button[i].button_x, phone_button[i].button_y, BUTTON_R);
      show_string(phone_button[i].button_name,phone_button[i].button_x-strlen(phone_button[i].button_name)*phone_button[i].button_name_size*6/2+phone_button[i].button_name_size/2+1,phone_button[i].button_y-phone_button[i].button_name_size*8/2+phone_button[i].button_name_size/2+1,phone_button[i].button_name_size,phone_button[i].button_name_colour,BLACK,1);
   }
   my_lcd.Set_Draw_color(BLACK);
   my_lcd.Fill_Rectangle(1, 1, my_lcd.Get_Display_Width()-2, 3);
   my_lcd.Fill_Rectangle(1, 29, my_lcd.Get_Display_Width()-2, 31);
   my_lcd.Fill_Rectangle(1, 1, 3, 31);
   my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()-4, 1, my_lcd.Get_Display_Width()-2, 31);
}
  
void setup(void) 
{
   my_lcd.Init_LCD();
   my_lcd.Set_Rotation(0); 
   my_touch.TP_Set_Rotation(2);
   my_touch.TP_Init(my_lcd.Get_Rotation(),my_lcd.Get_Display_Width(),my_lcd.Get_Display_Height()); 
   my_lcd.Fill_Screen(BLUE); 
   show_menu();
}

void loop(void)
{
  uint16_t i;
  px = 0;
  py = 0;
  my_touch.TP_Scan(0);
  if (my_touch.TP_Get_State()&TP_PRES_DOWN) 
  {
    px = my_touch.x;
    py = my_touch.y;
  } 
  for(i=0;i<sizeof(phone_button)/sizeof(button_info);i++)
  {
       if(is_pressed(phone_button[i].button_x-BUTTON_R,phone_button[i].button_y-BUTTON_R,phone_button[i].button_x+BUTTON_R,phone_button[i].button_y+BUTTON_R,px,py))
       {
            my_lcd.Set_Draw_color(DARKGREY);
            my_lcd.Fill_Circle(phone_button[i].button_x, phone_button[i].button_y, BUTTON_R);
            show_string(phone_button[i].button_name,phone_button[i].button_x-strlen(phone_button[i].button_name)*phone_button[i].button_name_size*6/2+phone_button[i].button_name_size/2+1,phone_button[i].button_y-phone_button[i].button_name_size*8/2+phone_button[i].button_name_size/2+1,phone_button[i].button_name_size,WHITE,BLACK,1);
            delay(100);
            my_lcd.Set_Draw_color(phone_button[i].button_colour);
            my_lcd.Fill_Circle(phone_button[i].button_x, phone_button[i].button_y, BUTTON_R);
            show_string(phone_button[i].button_name,phone_button[i].button_x-strlen(phone_button[i].button_name)*phone_button[i].button_name_size*6/2+phone_button[i].button_name_size/2+1,phone_button[i].button_y-phone_button[i].button_name_size*8/2+phone_button[i].button_name_size/2+1,phone_button[i].button_name_size,phone_button[i].button_name_colour,BLACK,1);  
            if(i < 12)
            {
                if(n < 13)
                {
                  show_string(phone_button[i].button_name,text_x,text_y,phone_button[i].button_name_size,GREENYELLOW, BLACK,1);
                  text_x += text_x_add-1;
                  n++;
                }
            }
            else if(12 == i)
            {
                my_lcd.Set_Draw_color(BLUE);
                my_lcd.Fill_Rectangle(0, 33, my_lcd.Get_Display_Width()-1, 42);
                show_string("Calling ended",CENTER,33,1,RED, BLACK,1);  
            } 
            else if(13 == i)
            {
                my_lcd.Set_Draw_color(BLUE);
                my_lcd.Fill_Rectangle(0, 33, my_lcd.Get_Display_Width()-1, 42);
                show_string("Calling...",CENTER,33,1,GREEN, BLACK,1);  
            }
            else if(14 == i)
            {
                if(n > 0)
                {
                    my_lcd.Set_Draw_color(BLUE);
                    text_x -= (text_x_add-1);  
                    my_lcd.Fill_Rectangle(text_x, text_y, text_x+text_x_add-1, text_y+text_y_add-2);
                    n--; 
                }
            }
       }      
  }
}

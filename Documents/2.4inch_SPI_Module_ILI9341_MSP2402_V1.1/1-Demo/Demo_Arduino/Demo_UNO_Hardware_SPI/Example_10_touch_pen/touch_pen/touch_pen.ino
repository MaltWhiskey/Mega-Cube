// IMPORTANT: LCDWIKI_SPI LIBRARY AND LCDWIKI_TOUCH LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.

//This program is a demo of drawing

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

#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

uint16_t color_mask[] = {0xF800,0xFFE0,0x07E0,0x07FF,0x001F,0xF81F}; //color select

#define COLORBOXSIZE my_lcd.Get_Display_Width()/6
#define PENBOXSIZE my_lcd.Get_Display_Width()/4

int16_t old_color, current_color,flag_colour;
int16_t old_pen,current_pen,flag_pen;
boolean show_flag = true;

void show_string(uint8_t *str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
{
    my_lcd.Set_Text_Mode(mode);
    my_lcd.Set_Text_Size(csize);
    my_lcd.Set_Text_colour(fc);
    my_lcd.Set_Text_Back_colour(bc);
    my_lcd.Print_String(str,x,y);
}

//show color select menu
void show_color_select_menu(void)
{
   uint16_t i;
   for(i = 0;i<6;i++)
   {
       my_lcd.Set_Draw_color(color_mask[i]);
       my_lcd.Fill_Rectangle(i*COLORBOXSIZE, 0, (i+1)*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
   }  
   my_lcd.Set_Draw_color(GREEN);
   my_lcd.Fill_Round_Rectangle((my_lcd.Get_Display_Width()-20)/3+10, COLORBOXSIZE/2+2, (my_lcd.Get_Display_Width()-20)/3*2+10,COLORBOXSIZE/2+20, 5);
   show_string("OK",CENTER,COLORBOXSIZE/2+4,2,RED, BLACK,1);
}

//show pen size select menu
void show_pen_size_select_menu(void)
{
  uint16_t i;
  my_lcd.Set_Text_Mode(1);
  my_lcd.Set_Text_Size(2);
  my_lcd.Set_Text_colour(GREEN);
  my_lcd.Set_Text_Back_colour(BLACK);
  for(i = 0;i<4;i++)
  {
      my_lcd.Print_Number_Int(i+1, 5+PENBOXSIZE*i, (COLORBOXSIZE/2-16)/2, 0, ' ',10);
      my_lcd.Set_Draw_color(RED);
      my_lcd.Fill_Rectangle(25+PENBOXSIZE*i, COLORBOXSIZE/2/2-i, PENBOXSIZE*(i+1)-10, COLORBOXSIZE/2/2+i);
  }
   my_lcd.Set_Draw_color(GREEN);
   my_lcd.Fill_Round_Rectangle((my_lcd.Get_Display_Width()-20)/3+10, COLORBOXSIZE/2+2, (my_lcd.Get_Display_Width()-20)/3*2+10,COLORBOXSIZE/2+20, 5);
   show_string("OK",CENTER,COLORBOXSIZE/2+4,2,RED, BLACK,1);
}

//show main menu
void show_main_menu(void)
{
   my_lcd.Set_Draw_color(YELLOW);
   my_lcd.Fill_Round_Rectangle(5, 0, (my_lcd.Get_Display_Width()-20)/3+5,COLORBOXSIZE/2+20, 5);
   my_lcd.Fill_Round_Rectangle((my_lcd.Get_Display_Width()-20)/3*2+15, 0, (my_lcd.Get_Display_Width()-20)/3*3+15,COLORBOXSIZE/2+20, 5);
   my_lcd.Set_Draw_color(MAGENTA);
   my_lcd.Fill_Round_Rectangle((my_lcd.Get_Display_Width()-20)/3+10, 0, (my_lcd.Get_Display_Width()-20)/3*2+10,COLORBOXSIZE/2+20, 5);
   show_string("COLOUR",8+((my_lcd.Get_Display_Width()-20)/3-72)/2-1,((COLORBOXSIZE/2+20)-16)/2,2,BLUE, BLACK,1);
   show_string("CLEAR",(my_lcd.Get_Display_Width()-20)/3+10+((my_lcd.Get_Display_Width()-20)/3-60)/2-1,((COLORBOXSIZE/2+20)-16)/2,2,WHITE, BLACK,1);
   show_string("PEN",(my_lcd.Get_Display_Width()-20)/3*2+15+((my_lcd.Get_Display_Width()-20)/3-36)/2-1,((COLORBOXSIZE/2+20)-16)/2,2,BLUE, BLACK,1);
 }

void setup(void) 
{
  my_lcd.Init_LCD();
  my_lcd.Set_Rotation(0); 
  my_touch.TP_Set_Rotation(2);
  my_touch.TP_Init(my_lcd.Get_Rotation(),my_lcd.Get_Display_Width(),my_lcd.Get_Display_Height()); 
  my_lcd.Fill_Screen(BLACK);
  show_main_menu();
  current_color = RED;
  current_pen = 0;
}

void loop()
{
comme:
  my_touch.TP_Scan(0);
  if (my_touch.TP_Get_State()&TP_PRES_DOWN) 
  {
         if(my_touch.y< COLORBOXSIZE/2+20) 
         {
              if(((my_touch.x>5)&&(my_touch.x < ((my_lcd.Get_Display_Width()-20)/3+5)))&&!flag_pen) //select color
              {
                flag_colour = 1;
                if(show_flag)
                {
                    my_lcd.Set_Draw_color(BLACK);
                    my_lcd.Fill_Rectangle(0,0,my_lcd.Get_Display_Width()-1,COLORBOXSIZE/2+20);
                    show_color_select_menu();
                }
                show_flag = false;
                switch(current_color)
                {
                  case RED:
                  {
                    my_lcd.Set_Draw_color(WHITE);
                    my_lcd.Draw_Rectangle(0, 0, COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                    break;  
                  }
                  case YELLOW:
                  {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(COLORBOXSIZE, 0, 2*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                       break; 
                  }
                  case GREEN:
                  {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(2*COLORBOXSIZE, 0, 3*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                       break; 
                   }
                  case CYAN:
                  {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(3*COLORBOXSIZE, 0, 4*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                       break;                 
                  }
                  case BLUE:
                  {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(4*COLORBOXSIZE, 0, 5*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                       break;   
                  }
                  case MAGENTA:  
                  {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(5*COLORBOXSIZE, 0, 6*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                       break;   
                  }
                  default:
                      break;
                }
             }
             if(flag_colour)
             {
                 if(my_touch.y < COLORBOXSIZE/2)
                 {
                    old_color = current_color;
                    if (my_touch.x < COLORBOXSIZE) 
                    { 
                        current_color = RED; 
                        my_lcd.Set_Draw_color(WHITE);
                        my_lcd.Draw_Rectangle(0, 0, COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                    } 
                    else if (my_touch.x < COLORBOXSIZE*2) 
                    {
                        current_color = YELLOW;
                        my_lcd.Set_Draw_color(WHITE);
                        my_lcd.Draw_Rectangle(COLORBOXSIZE, 0, 2*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                    } 
                    else if (my_touch.x < COLORBOXSIZE*3) 
                    {
                        current_color = GREEN;
                        my_lcd.Set_Draw_color(WHITE);
                        my_lcd.Draw_Rectangle(2*COLORBOXSIZE, 0, 3*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                    } 
                    else if (my_touch.x < COLORBOXSIZE*4) 
                    {
                        current_color = CYAN;
                        my_lcd.Set_Draw_color(WHITE);
                        my_lcd.Draw_Rectangle(3*COLORBOXSIZE, 0, 4*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                    } 
                    else if (my_touch.x < COLORBOXSIZE*5) 
                    {
                        current_color = BLUE;
                        my_lcd.Set_Draw_color(WHITE);
                        my_lcd.Draw_Rectangle(4*COLORBOXSIZE, 0, 5*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                    } 
                    else if (my_touch.x < COLORBOXSIZE*6) 
                    {
                        current_color = MAGENTA;
                        my_lcd.Set_Draw_color(WHITE);
                        my_lcd.Draw_Rectangle(5*COLORBOXSIZE, 0, 6*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                    }
                    if(old_color != current_color)
                    {
                        switch(old_color)
                        {
                            case RED:
                            {
                              my_lcd.Set_Draw_color(RED);
                              my_lcd.Draw_Rectangle(0, 0, COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                              break;  
                            }
                            case YELLOW:
                            {
                                 my_lcd.Set_Draw_color(YELLOW);
                                 my_lcd.Draw_Rectangle(COLORBOXSIZE, 0, 2*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                                 break; 
                            }
                            case GREEN:
                            {
                                 my_lcd.Set_Draw_color(GREEN);
                                 my_lcd.Draw_Rectangle(2*COLORBOXSIZE, 0, 3*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                                 break; 
                             }
                            case CYAN:
                            {
                                 my_lcd.Set_Draw_color(CYAN);
                                 my_lcd.Draw_Rectangle(3*COLORBOXSIZE, 0, 4*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                                 break;                 
                            }
                            case BLUE:
                            {
                                 my_lcd.Set_Draw_color(BLUE);
                                 my_lcd.Draw_Rectangle(4*COLORBOXSIZE, 0, 5*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                                 break;   
                            }
                            case MAGENTA:  
                            {
                                 my_lcd.Set_Draw_color(MAGENTA);
                                 my_lcd.Draw_Rectangle(5*COLORBOXSIZE, 0, 6*COLORBOXSIZE-1, COLORBOXSIZE/2-1);
                                 break;   
                            }
                            default:
                                break;                        
                        }
                    }
                }
                else if(my_touch.y< COLORBOXSIZE/2+20)
                {
                     if((my_touch.x>(my_lcd.Get_Display_Width()-20)/3+10)&&(my_touch.x<(my_lcd.Get_Display_Width()-20)/3*2+10))
                     {
                         my_lcd.Set_Draw_color(BLACK);
                         my_lcd.Fill_Rectangle(0,0,my_lcd.Get_Display_Width()-1,COLORBOXSIZE/2+20);
                         show_main_menu();
                         flag_colour = 0;
                         show_flag = true;
                         goto comme;
                     }
                }
            }
            if((my_touch.x>((my_lcd.Get_Display_Width()-20)/3*2+15))&&(my_touch.x < (((my_lcd.Get_Display_Width()-20)/3*3+15)))&&!flag_colour) //select pen size
            {
                flag_pen = 1;
                if(show_flag)
                {
                    my_lcd.Set_Draw_color(BLACK);
                    my_lcd.Fill_Rectangle(0,0,my_lcd.Get_Display_Width()-1,COLORBOXSIZE/2+20);
                    show_pen_size_select_menu();
                }
                show_flag = false;
                switch(current_pen)
                {
                   case 0:
                   {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(0, 0, PENBOXSIZE-1, COLORBOXSIZE/2-1);
                       break;
                   }
                   case 1:
                   {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(PENBOXSIZE, 0, 2*PENBOXSIZE-1, COLORBOXSIZE/2-1);
                       break; 
                   }
                   case 2:
                   {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(2*PENBOXSIZE, 0, 3*PENBOXSIZE-1, COLORBOXSIZE/2-1);
                       break;   
                   }
                   case 3:  
                   {
                       my_lcd.Set_Draw_color(WHITE);
                       my_lcd.Draw_Rectangle(3*PENBOXSIZE, 0, 4*PENBOXSIZE-1, COLORBOXSIZE/2-1);
                       break;                
                   }
                   default:
                       break;
                  }              
              }
              if(flag_pen)
              {
                   if(my_touch.y < COLORBOXSIZE/2)
                   {
                      old_pen = current_pen;
                      if(my_touch.x < PENBOXSIZE)
                      {
                          current_pen = 0;
                          my_lcd.Set_Draw_color(WHITE);
                          my_lcd.Draw_Rectangle(0, 0, PENBOXSIZE-1, COLORBOXSIZE/2-1);
                      }
                      else if(my_touch.x < 2*PENBOXSIZE)
                      {
                          current_pen = 1;
                          my_lcd.Set_Draw_color(WHITE);
                          my_lcd.Draw_Rectangle(PENBOXSIZE, 0, 2*PENBOXSIZE-1, COLORBOXSIZE/2-1);
                       }
                       else if(my_touch.x < 3*PENBOXSIZE) 
                       {
                           current_pen = 2;
                           my_lcd.Set_Draw_color(WHITE);
                           my_lcd.Draw_Rectangle(2*PENBOXSIZE, 0, 3*PENBOXSIZE-1, COLORBOXSIZE/2-1);
                       }
                       else if(my_touch.x < 4*PENBOXSIZE)
                       {
                           current_pen = 3;
                           my_lcd.Set_Draw_color(WHITE);
                           my_lcd.Draw_Rectangle(3*PENBOXSIZE, 0, 4*PENBOXSIZE-1, COLORBOXSIZE/2-1);               
                       }
                       if(old_pen != current_pen)
                       {
                           switch(old_pen)
                           {
                                 case 0:
                                 {
                                     my_lcd.Set_Draw_color(BLACK);
                                     my_lcd.Draw_Rectangle(0, 0, PENBOXSIZE-1, COLORBOXSIZE/2-1);
                                     break;
                                 }
                                 case 1:
                                 {
                                     my_lcd.Set_Draw_color(BLACK);
                                     my_lcd.Draw_Rectangle(PENBOXSIZE, 0, 2*PENBOXSIZE-1, COLORBOXSIZE/2-1);
                                     break; 
                                 }
                                 case 2:
                                 {
                                     my_lcd.Set_Draw_color(BLACK);
                                     my_lcd.Draw_Rectangle(2*PENBOXSIZE, 0, 3*PENBOXSIZE-1, COLORBOXSIZE/2-1);
                                     break;   
                                 }
                                 case 3:  
                                 {
                                     my_lcd.Set_Draw_color(BLACK);
                                     my_lcd.Draw_Rectangle(3*PENBOXSIZE, 0, 4*PENBOXSIZE-1, COLORBOXSIZE/2-1);
                                     break;                
                                 }
                                 default:
                                     break;           
                           }      
                       }
                   }
                   else if(my_touch.y < COLORBOXSIZE/2+20)
                   {
                       if((my_touch.x>(my_lcd.Get_Display_Width()-20)/3+10)&&(my_touch.x<(my_lcd.Get_Display_Width()-20)/3*2+10))
                       {
                          my_lcd.Set_Draw_color(BLACK);
                          my_lcd.Fill_Rectangle(0,0,my_lcd.Get_Display_Width()-1,COLORBOXSIZE/2+20);
                          show_main_menu();
                          flag_pen = 0;
                          show_flag = true;
                          goto comme;
                       }  
                   }
              }
              if(((my_touch.x>((my_lcd.Get_Display_Width()-20)/3+10))&&(my_touch.x < ((my_lcd.Get_Display_Width()-20)/3*2+10)))&&!flag_colour&&!flag_pen) 
              {
                  my_lcd.Set_Draw_color(BLACK);  
                  my_lcd.Fill_Rectangle(0,COLORBOXSIZE/2+21,my_lcd.Get_Display_Width()-1,my_lcd.Get_Display_Height()-1);
              }
         }
      if (((my_touch.y-current_pen) > COLORBOXSIZE/2+20) && ((my_touch.y+current_pen) < my_lcd.Get_Display_Height()))  //drawing
      {
        my_lcd.Set_Draw_color(current_color);
        my_lcd.Fill_Circle(my_touch.x,  my_touch.y,current_pen);
    }
  }
}

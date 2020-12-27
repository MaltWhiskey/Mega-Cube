// IMPORTANT: LCDWIKI_SPI LIBRARY AND LCDWIKI_TOUCH LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.

//This program is a demo of how to calibration touch screen

//if you adjust the touch screen successfully,you should put the calibration parameter
//to the file of cali_para.h in the library.

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

#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

LCDWIKI_SPI my_lcd(ILI9341,A5,A3,12,11,A4,13,A0); //model,cs,dc,miso,mosi,reset,sck,led
LCDWIKI_TOUCH my_touch(2,3,4,5,6); //tcs,tclk,tdout,tdin,tirq

void show_string(uint8_t *str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
{
    my_lcd.Set_Text_Mode(mode);
    my_lcd.Set_Text_Size(csize);
    my_lcd.Set_Text_colour(fc);
    my_lcd.Set_Text_Back_colour(bc);
    my_lcd.Print_String(str,x,y);
}

void show_number(long num,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode,int16_t sys)
{
    my_lcd.Set_Text_Mode(mode);
    my_lcd.Set_Text_Size(csize);
    my_lcd.Set_Text_colour(fc);
    my_lcd.Set_Text_Back_colour(bc);
    my_lcd.Print_Number_Int(num, x, y, 0, ' ',10);
}

void draw_touch_point(int16_t x, int16_t y, uint16_t color)
{
    my_lcd.Set_Draw_color(color); 
    my_lcd.Draw_Fast_HLine(x-12,y,26);
    my_lcd.Draw_Fast_VLine(x,y-12,26);
    my_lcd.Draw_Pixel(x+1, y+1);
    my_lcd.Draw_Pixel(x-1, y+1);
    my_lcd.Draw_Pixel(x+1, y-1);
    my_lcd.Draw_Pixel(x-1, y-1);
    my_lcd.Draw_Circle(x,y,6);
}

void show_cali_info(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t fac)
{
    my_lcd.Set_Draw_color(WHITE);
    my_lcd.Fill_Rectangle(0,140,my_lcd.Get_Display_Width()-1,my_lcd.Get_Display_Height()-1);
    show_string("x1:",40,140,2,RED,WHITE,1);
    show_string("y1:",40+90,140,2,RED,WHITE,1);
    show_string("x2:",40,160,2,RED,WHITE,1);
    show_string("y2:",40+90,160,2,RED,WHITE,1);
    show_string("x3:",40,180,2,RED,WHITE,1);
    show_string("y3:",40+90,180,2,RED,WHITE,1);
    show_string("x4:",40,200,2,RED,WHITE,1);
    show_string("y4:",40+90,200,2,RED,WHITE,1);
    show_string("fac is:",40,220,2,RED,WHITE,1);
    show_number(x0,40+36,140,2,RED,WHITE,1,10); 
    show_number(y0,40+36+90,140,2,RED,WHITE,1,10); 
    show_number(x1,40+36,160,2,RED,WHITE,1,10); 
    show_number(y1,40+36+90,160,2,RED,WHITE,1,10); 
    show_number(x2,40+36,180,2,RED,WHITE,1,10); 
    show_number(y2,40+36+90,180,2,RED,WHITE,1,10); 
    show_number(x3,40+36,200,2,RED,WHITE,1,10); 
    show_number(y3,40+36+90,200,2,RED,WHITE,1,10); 
    show_number(fac,40+84,220,2,RED,WHITE,1,10); 
}
void touch_screen_calibration(void)
{
  int16_t pos_temp[4][2],xoffset,yoffset;
  uint8_t cnt = 0;
  uint16_t d1,d2;
  uint32_t temp1,temp2;
  float fac,xfac,yfac;
  bool flag = false;
  my_lcd.Fill_Screen(WHITE);
  //Display prompt information
  show_string("Please use the stylus click",10,40,1,RED, BLACK,1); 
  show_string("the cross on the screen.",10,56,1,RED, BLACK,1); 
  show_string("The cross will always move",10,72,1,RED, BLACK,1); 
  show_string("until the screen adjustment",10,88,1,RED, BLACK,1); 
  show_string("is completed.",10,104,1,RED, BLACK,1); 

  //draw the first point
  draw_touch_point(20, 20, RED);
  //Eliminate trigger signal
  my_touch.TP_Set_State(0);
  while(1)
  {
     my_touch.TP_Scan(1);//Scanning physical coordinates  
     if((my_touch.TP_Get_State()&0xC0) == TP_CATH_PRES) //Press the button once and release it
     {
        my_touch.TP_Set_State(my_touch.TP_Get_State()&(~(1<<6)));
        pos_temp[cnt][0] = my_touch.x;
        pos_temp[cnt][1] = my_touch.y;
        cnt++;
        switch(cnt)
        {
            case 1:
               draw_touch_point(20, 20, WHITE);
               draw_touch_point(my_lcd.Get_Display_Width()-20, 20, RED);
               break;
            case 2:
               draw_touch_point(my_lcd.Get_Display_Width()-20, 20, WHITE);
               draw_touch_point(20, my_lcd.Get_Display_Height()-20, RED);
               break;               
            case 3:
               draw_touch_point(20, my_lcd.Get_Display_Height()-20, WHITE);
               draw_touch_point(my_lcd.Get_Display_Width()-20, my_lcd.Get_Display_Height()-20, RED);
               break;
            case 4:
                temp1=abs(pos_temp[0][0]-pos_temp[1][0]);
                temp2=abs(pos_temp[0][1]-pos_temp[1][1]);
                temp1*=temp1;
                temp2*=temp2;
                d1 = sqrt(temp1+temp2);
                temp1=abs(pos_temp[2][0]-pos_temp[3][0]);
                temp2=abs(pos_temp[2][1]-pos_temp[3][1]);
                temp1*=temp1;
                temp2*=temp2;
                d2 = sqrt(temp1+temp2); 
                fac=(float)d1/d2;
                if(fac<0.95||fac>1.05||d1==0||d2==0)
                {
                  cnt=0;
                  draw_touch_point(my_lcd.Get_Display_Width()-20, my_lcd.Get_Display_Height()-20, WHITE);
                  draw_touch_point(20, 20, RED); 
                  show_cali_info(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100); 
                  continue;
                }  
                temp1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
                temp2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
                temp1*=temp1;
                temp2*=temp2;
                d1=sqrt(temp1+temp2);//µÃµ½1,3µÄ¾àÀë 
                temp1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
                temp2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
                temp1*=temp1;
                temp2*=temp2;
                d2=sqrt(temp1+temp2);//µÃµ½2,4µÄ¾àÀë
                fac=(float)d1/d2;  
                if(fac<0.95||fac>1.05)//²»ºÏ¸ñ
                {
                   cnt=0; 
                   draw_touch_point(my_lcd.Get_Display_Width()-20, my_lcd.Get_Display_Height()-20, WHITE);
                   draw_touch_point(20, 20, RED); 
                   show_cali_info(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);
                   continue;
                }  
                temp1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
                temp2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
                temp1*=temp1;
                temp2*=temp2;
                d1=sqrt(temp1+temp2);//µÃµ½1,4µÄ¾àÀë
                temp1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
                temp2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
                temp1*=temp1;
                temp2*=temp2;
                d2=sqrt(temp1+temp2);//
                fac=(float)d1/d2;
                if(fac<0.95||fac>1.05)//
                {
                  cnt=0;
                  draw_touch_point(my_lcd.Get_Display_Width()-20, my_lcd.Get_Display_Height()-20, WHITE);
                  draw_touch_point(20, 20, RED); 
                  show_cali_info(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);
                  continue;
                }//ÕýÈ·ÁË  
                flag = true;
                if(my_touch.LCD_Get_Rotation()==0||my_touch.LCD_Get_Rotation()==2)
                {     
                    xfac=(float)(my_lcd.Get_Display_Width()-40)/(abs(pos_temp[1][0]-pos_temp[0][0]));//µÃµ½xfac  
                    xoffset=(my_lcd.Get_Display_Width()-xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//µÃµ½xoff   
                    yfac=(float)(my_lcd.Get_Display_Height()-40)/(abs(pos_temp[2][1]-pos_temp[0][1]));//µÃµ½yfac
                    yoffset=(my_lcd.Get_Display_Height()-yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//µÃµ½yoff   
                }
                else if(my_touch.LCD_Get_Rotation()==1||my_touch.LCD_Get_Rotation()==3)
                {
                    yfac=(float)(my_lcd.Get_Display_Width()-40)/(abs(pos_temp[1][1]-pos_temp[0][1]));//µÃµ½xfac    
                    yoffset=(my_lcd.Get_Display_Width()-yfac*(pos_temp[1][1]+pos_temp[0][1]))/2;//µÃµ½xoff   
                    xfac=(float)(my_lcd.Get_Display_Height()-40)/(abs(pos_temp[2][0]-pos_temp[0][0]));//µÃµ½yfac
                    xoffset=(my_lcd.Get_Display_Height()-xfac*(pos_temp[2][0]+pos_temp[0][0]))/2;//µÃµ½yoff   
                }
                my_lcd.Fill_Screen(WHITE);
                show_string("Touch Screen Adjust OK!",35,110,2,BLUE, WHITE,1);
                show_string("xfac:",35,130,2,BLUE, WHITE,1);
                show_string("xoffset:",35,150,2,BLUE, WHITE,1);
                show_string("yfac:",35,170,2,BLUE, WHITE,1);
                show_string("yoffset:",35,190,2,BLUE, WHITE,1);
                show_number((long)(xfac*10000),35+60,130,2,BLUE,WHITE,1,10); 
                show_number(xoffset,35+96,150,2,BLUE,WHITE,1,10); 
                show_number((long)(yfac*10000),35+60,170,2,BLUE,WHITE,1,10); 
                show_number(yoffset,35+96,190,2,BLUE,WHITE,1,10); 
                break;
        }   
     }
     if(flag)
     {
        break; 
     }   
   }  
}

void setup() 
{
  my_lcd.Init_LCD();
  my_touch.TP_Init(my_lcd.Get_Rotation(),my_lcd.Get_Display_Width(),my_lcd.Get_Display_Height()); 
  touch_screen_calibration(); 
}

void loop() 
{
}

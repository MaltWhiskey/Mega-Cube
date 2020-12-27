// IMPORTANT: LCDWIKI_SPI LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.

//This program is a demo of how to use most of the functions 
//of the library with a supported display modules.

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
#define CS   A5    
#define CD   A3
#define RST  A4
#define LED  A0   //if you don't need to control the LED pin,you should set it to -1 and set it to 3.3V

//the definiens of hardware spi mode as follow:
//if the IC model is known or the modules is unreadable,you can use this constructed function
LCDWIKI_SPI my_lcd(MODEL,CS,CD,RST,LED); //model,cs,dc,reset,led
//if the IC model is not known and the modules is readable,you can use this constructed function
//LCDWIKI_SPI my_lcd(240,320,CS,CD,RST,LED); //model,cs,dc,reset,led

void show_string(uint8_t *str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
{
    my_lcd.Set_Text_Mode(mode);
    my_lcd.Set_Text_Size(csize);
    my_lcd.Set_Text_colour(fc);
    my_lcd.Set_Text_Back_colour(bc);
    my_lcd.Print_String(str,x,y);
}

//display main surface
unsigned long show_text(void)
{
    unsigned long time_start = micros();
    my_lcd.Set_Draw_color(32, 0,255);
    my_lcd.Fill_Rectangle(0, 0, my_lcd.Get_Display_Width()-1, 14);
    show_string("* Universal Color TFT Display Library *",CENTER,3,1,0x07E0, 0,1);

    my_lcd.Set_Draw_color(128, 128, 128);
    my_lcd.Fill_Rectangle(0, my_lcd.Get_Display_Height()-15, my_lcd.Get_Display_Width()-1, my_lcd.Get_Display_Height()-1);
    show_string("<http://www.lcdwiki.com>",CENTER,my_lcd.Get_Display_Height()-11,1,0xFFFF, 0,1);
    
    my_lcd.Set_Draw_color(255, 0, 0); 
    my_lcd.Draw_Rectangle(0, 15, my_lcd.Get_Display_Width()-1, my_lcd.Get_Display_Height()-16);   
    return micros() - time_start;
}

//display triangle functions
unsigned long show_triangle_function(void)
{
     uint16_t i;
     unsigned long time_start = micros();
     // Draw crosshairs
     my_lcd.Set_Draw_color(0, 0, 255); 
     my_lcd.Draw_Fast_VLine(my_lcd.Get_Display_Width()/2-1, 16, my_lcd.Get_Display_Height()- 32);
     my_lcd.Draw_Fast_HLine(1, my_lcd.Get_Display_Height()/2-1, my_lcd.Get_Display_Width()-2);
     for(i = 1;i <= (my_lcd.Get_Display_Height()- 32)/2/10;i++)
     {
         my_lcd.Draw_Fast_HLine(my_lcd.Get_Display_Width()/2-1-2, my_lcd.Get_Display_Height()/2-1-i*10, 5);
         my_lcd.Draw_Fast_HLine(my_lcd.Get_Display_Width()/2-1-2, my_lcd.Get_Display_Height()/2-1+i*10, 5);
     }
     for(i = 1;i <= (my_lcd.Get_Display_Width()-2)/2/10;i++)
     {
         my_lcd.Draw_Fast_VLine(my_lcd.Get_Display_Width()/2-1-i*10, my_lcd.Get_Display_Height()/2-1-2, 5);
         my_lcd.Draw_Fast_VLine(my_lcd.Get_Display_Width()/2-1+i*10, my_lcd.Get_Display_Height()/2-1-2, 5);
     }
     
     // Draw sin lines
     show_string("sin",5,17,1,0x07FF, 0,0);
     my_lcd.Set_Draw_color(0, 255, 255); 
     for (i=1; i<my_lcd.Get_Display_Width()-2; i++)
     {
        my_lcd.Draw_Pixel(i,my_lcd.Get_Display_Height()/2-1+(sin(((i*1.13)*3.14)/180)*95));
     }

     // Draw cos lines
     show_string("cos",5,25,1,0x07E0, 0,0);
     my_lcd.Set_Draw_color(0, 255, 0);
     for (i=1; i<my_lcd.Get_Display_Width()-2; i++)
     {
        my_lcd.Draw_Pixel(i,my_lcd.Get_Display_Height()/2-1+(cos(((i*1.13)*3.14)/180)*95));
     }

     // Draw tan lines
     show_string("tan",5,33,1,0xFFE0, 0,0);
     my_lcd.Set_Draw_color(255, 255, 0);
     for (i=1; i<my_lcd.Get_Display_Width()-2; i++)
     {
        my_lcd.Draw_Pixel(i,my_lcd.Get_Display_Height()/2-1+(tan(((i*1.13)*3.14)/180)*10));
     }

     // Draw cot lines
     show_string("cot",5,41,1,0xF800, 0,0);
     my_lcd.Set_Draw_color(255, 0, 0);
     for (i=1; i<my_lcd.Get_Display_Width()-2; i++)
     {
        my_lcd.Draw_Pixel(i,my_lcd.Get_Display_Height()/2-1+1/(tan(((i*1.13)*3.14)/180)*0.1));
     }
     return micros()-time_start;
}

// Draw a moving sinewave
unsigned long show_sinewave(void)
{
   uint16_t x = 1,i,y;
   uint8_t buf[my_lcd.Get_Display_Width()-2];
   unsigned long time_start = micros();
   int16_t t = 20;
   float k = 1.1;
   my_lcd.Set_Draw_color(0, 0, 255); 
   my_lcd.Draw_Fast_VLine(my_lcd.Get_Display_Width()/2-1, 16, my_lcd.Get_Display_Height()- 32);
   my_lcd.Draw_Fast_HLine(1, my_lcd.Get_Display_Height()/2-1, my_lcd.Get_Display_Width()-2);
   for (i=1; i<((my_lcd.Get_Display_Width()-2)*t); i++) 
   {
      x++;
      if (x==my_lcd.Get_Display_Width()-1)
      {
          x=1;
      }
      if (i>my_lcd.Get_Display_Width()-1)
      {
          if ((x==my_lcd.Get_Display_Width()/2-1)||(buf[x-1]==my_lcd.Get_Display_Height()/2-1))
          {
              my_lcd.Set_Draw_color(0, 0, 255); 
          }
          else
          {
             my_lcd.Set_Draw_color(0, 0, 0); 
          }
          my_lcd.Draw_Pixel(x,buf[x-1]);
    }
    my_lcd.Set_Draw_color(255, 64, 255);
    y=my_lcd.Get_Display_Height()/2-1+(sin(((i*k)*3.14)/180)*(90-(i/100)));
    my_lcd.Draw_Pixel(x,y);
    buf[x-1]=y;
  }
  return micros()- time_start;   
}

// Draw some filled rectangles
unsigned long show_fill_rectangle(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    uint16_t side_len = (my_lcd.Get_Display_Height()-40)/5;
    uint16_t x_spec = (my_lcd.Get_Display_Width()-5*side_len)/2;
    uint16_t y_spec = (my_lcd.Get_Display_Height()-5*side_len)/2;
    for(i = 0;i<5;i++)
    {
          switch (i)
          {
            case 0:
              my_lcd.Set_Draw_color(255,0,255);
              break;
            case 1:
              my_lcd.Set_Draw_color(255,0,0);
              break;
            case 2:
              my_lcd.Set_Draw_color(0,255,0);
              break;
            case 3:
              my_lcd.Set_Draw_color(0,0,255);
              break;
            case 4:
              my_lcd.Set_Draw_color(255,255,0);
              break;
            default:
              break;
          }   
          my_lcd.Fill_Rectangle(x_spec+i*side_len-1, y_spec+i*side_len-1, x_spec+(i+1)*side_len-1, y_spec+(i+1)*side_len-1);
          my_lcd.Fill_Rectangle(x_spec+i*side_len-1, y_spec+(5-i)*side_len-1, x_spec+(i+1)*side_len-1, y_spec+(4-i)*side_len-1); 
     }
     return micros()- time_start;   
}

// Draw some filled round rectangles
unsigned long show_fill_round_rectangle(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    uint16_t side_len = (my_lcd.Get_Display_Height()-40)/5;
    uint16_t x_spec = (my_lcd.Get_Display_Width()-5*side_len)/2;
    uint16_t y_spec = (my_lcd.Get_Display_Height()-5*side_len)/2;
    for(i = 0;i<5;i++)
    {
          switch (i)
          {
            case 0:
              my_lcd.Set_Draw_color(255,0,255);
              break;
            case 1:
              my_lcd.Set_Draw_color(255,0,0);
              break;
            case 2:
              my_lcd.Set_Draw_color(0,255,0);
              break;
            case 3:
              my_lcd.Set_Draw_color(0,0,255);
              break;
            case 4:
              my_lcd.Set_Draw_color(255,255,0);
              break;
            default:
              break;
          }   
          my_lcd.Fill_Round_Rectangle(x_spec+i*side_len-1, y_spec+i*side_len-1, x_spec+(i+1)*side_len-1, y_spec+(i+1)*side_len-1,10);
          my_lcd.Fill_Round_Rectangle(x_spec+i*side_len-1, y_spec+(5-i)*side_len-1, x_spec+(i+1)*side_len-1, y_spec+(4-i)*side_len-1,10); 
     }
     return micros()- time_start;   
}

// Draw some filled circles
unsigned long show_fill_circle(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    uint16_t r_len = (my_lcd.Get_Display_Height()-40)/5/2;
    uint16_t x_spec = (my_lcd.Get_Display_Width()-5*r_len*2)/2;
    uint16_t y_spec = (my_lcd.Get_Display_Height()-5*r_len*2)/2;
    for(i = 0;i<5;i++)
    {
          switch (i)
          {
            case 0:
              my_lcd.Set_Draw_color(255,0,255);
              break;
            case 1:
              my_lcd.Set_Draw_color(255,0,0);
              break;
            case 2:
              my_lcd.Set_Draw_color(0,255,0);
              break;
            case 3:
              my_lcd.Set_Draw_color(0,0,255);
              break;
            case 4:
              my_lcd.Set_Draw_color(255,255,0);
              break;
            default:
              break;
          }   
          my_lcd.Fill_Circle(x_spec+r_len+i*r_len*2-1, y_spec+r_len+i*r_len*2-1,r_len);
          my_lcd.Fill_Circle(x_spec+r_len+i*r_len*2-1, y_spec+(5-i)*r_len*2-r_len-1,r_len); 
     }
     return micros()- time_start;   
 }

// Draw some filled triangles
unsigned long show_fill_triangle(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    uint16_t h_len = (my_lcd.Get_Display_Height()-40)/5;
    uint16_t side_len = (h_len*115)/100;
    uint16_t x_spec = (my_lcd.Get_Display_Width()-5*side_len)/2;
    uint16_t y_spec = (my_lcd.Get_Display_Height()-5*h_len)/2;
    for(i = 0;i<5;i++)
    {
          switch (i)
          {
            case 0:
              my_lcd.Set_Draw_color(255,0,255);
              break;
            case 1:
              my_lcd.Set_Draw_color(255,0,0);
              break;
            case 2:
              my_lcd.Set_Draw_color(0,255,0);
              break;
            case 3:
              my_lcd.Set_Draw_color(0,0,255);
              break;
            case 4:
              my_lcd.Set_Draw_color(255,255,0);
              break;
            default:
              break;
          } 
          my_lcd.Fill_Triangle(x_spec+i*side_len-1,y_spec+(i+1)*h_len-1,x_spec+side_len/2+i*side_len-1,y_spec+i*h_len-1,x_spec+(i+1)*side_len-1,y_spec+(i+1)*h_len-1); 
          my_lcd.Fill_Triangle(x_spec+i*side_len-1,y_spec+(5-i)*h_len-1,x_spec+side_len/2+i*side_len-1,y_spec+(4-i)*h_len-1,x_spec+(i+1)*side_len-1,y_spec+(5-i)*h_len-1);  
     }
     return micros()- time_start;   
}

// Draw some lines in a pattern
unsigned long show_grid_lines(void)
{
    uint16_t i;
    unsigned long time_start = micros();
   float k=1.44;
    my_lcd.Set_Draw_color(255,0,0);
    for (i=16; i<my_lcd.Get_Display_Height()-17; i+=5)
    {
       my_lcd.Draw_Line(1, i, (i*k)-10, my_lcd.Get_Display_Height()-17);
     }
     my_lcd.Set_Draw_color(255,0,0);
     for (i=my_lcd.Get_Display_Height()-17; i>16; i-=5)
    {
      my_lcd.Draw_Line(my_lcd.Get_Display_Width()-2, i, (i*k)-11, 16);
    }
    my_lcd.Set_Draw_color(0,255,255);
    for (i=my_lcd.Get_Display_Height()-16; i>16; i-=5)
    {
        my_lcd.Draw_Line(1, i, (my_lcd.Get_Display_Height()-17)*k+10-(i*k), 16);
    }
    my_lcd.Set_Draw_color(0,255,255);
    for (int i=15; i<my_lcd.Get_Display_Height()-17; i+=5)
    {
        my_lcd.Draw_Line(my_lcd.Get_Display_Width()-2, i, (my_lcd.Get_Display_Height()-17)*k+10-(i*k), my_lcd.Get_Display_Height()-17);
    }
    return micros()- time_start;   
}

// Draw some random pixels
unsigned long show_random_pixels(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 5000;i++)
    {
       my_lcd.Set_Draw_color(random(255),random(255),random(255));
       my_lcd.Draw_Pixel(2+random(my_lcd.Get_Display_Width()-4),17+random(my_lcd.Get_Display_Height()-34));
    }
    return micros()- time_start; 
}

// Draw some random lines
unsigned long show_random_lines(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 300;i++)
    {
       my_lcd.Set_Draw_color(random(255),random(255),random(255));
       my_lcd.Draw_Line(2+random(my_lcd.Get_Display_Width()-4),17+random(my_lcd.Get_Display_Height()-34),2+random(my_lcd.Get_Display_Width()-4),17+random(my_lcd.Get_Display_Height()-34));
    }
    return micros()- time_start; 
}

// Draw some random rectangles
unsigned long show_random_rectangles(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 150;i++)
    {
       my_lcd.Set_Draw_color(random(255),random(255),random(255));
       my_lcd.Draw_Rectangle(2+random(my_lcd.Get_Display_Width()-4),17+random(my_lcd.Get_Display_Height()-34),2+random(my_lcd.Get_Display_Width()-4),17+random(my_lcd.Get_Display_Height()-34));
    }
    return micros()- time_start; 
}

// Draw some random round rectangles
unsigned long show_random_round_rectangles(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 150;i++)
    {
       my_lcd.Set_Draw_color(random(255),random(255),random(255));
       my_lcd.Draw_Round_Rectangle(2+random(my_lcd.Get_Display_Width()-4),17+random(my_lcd.Get_Display_Height()-34),2+random(my_lcd.Get_Display_Width()-4),17+random(my_lcd.Get_Display_Height()-34),5);
    }
    return micros()- time_start; 
}

// Draw some random circles
unsigned long show_random_circles(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 150;i++)
    {
       my_lcd.Set_Draw_color(random(255),random(255),random(255));
       my_lcd.Draw_Circle(41+random(my_lcd.Get_Display_Width()-82), 56+random(my_lcd.Get_Display_Height()-112), random(40));
    }
    return micros()- time_start; 
}

// Draw some random triangles
unsigned long show_random_triangles(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 150;i++)
    {
       my_lcd.Set_Draw_color(random(255),random(255),random(255));
       my_lcd.Draw_Triangle(2+random(my_lcd.Get_Display_Width()-4),17+random(my_lcd.Get_Display_Height()-34),2+random(my_lcd.Get_Display_Width()-4),17+random(my_lcd.Get_Display_Height()-34),2+random(my_lcd.Get_Display_Width()-4),17+random(my_lcd.Get_Display_Height()-34));
    }
    return micros()- time_start; 
}

// Draw some random bit maps
unsigned long show_random_bit_map(void)
{
    uint16_t buf[48],i;
    unsigned long time_start = micros();
    int16_t len = (my_lcd.Get_Display_Height()*3/4)/6;
    for(i = 0;i< 48; i++)
    {
       my_lcd.Set_Draw_color(random(255), random(255), random(255));
       buf[i] = my_lcd.Get_Draw_color();
    }
    for(i = 1;i<=6;i++)
    {
        my_lcd.Draw_Bit_Map(my_lcd.Get_Display_Width()/2-1-((len/2)*4/3)*i, my_lcd.Get_Display_Height()/2-1-(len/2)*i, 8, 6, buf, i*(len/6));
        delay(100);
    }
    return micros()- time_start; 
}

//Clear the screen
void clear_screen(void)
{
   delay(2000);  
   my_lcd.Set_Draw_color(0, 0, 0);
   my_lcd.Fill_Rectangle(1, 16, my_lcd.Get_Display_Width()-2, my_lcd.Get_Display_Height()-17);
}

unsigned long (*show_function[])(void) = 
 {
                                          show_text,
                                          show_triangle_function,
                                          show_sinewave,
                                          show_fill_rectangle,
                                          show_fill_round_rectangle,
                                          show_fill_circle,
                                          show_fill_triangle,
                                          show_grid_lines,
                                          show_random_pixels,
                                          show_random_lines,
                                          show_random_rectangles,
                                          show_random_round_rectangles,
                                          show_random_circles,
                                          show_random_triangles,
                                          show_random_bit_map,
                                          };

 static uint8_t* show_str[] =
 {
                       "TEST_TX  :", 
                       "TEST_TF  :",
                       "TEST_SW  :",
                       "TEST_FR  :",
                       "TEST_FRR :",
                       "TEST_FC  :",
                       "TEST_FT  :",
                       "TEST_GL  :",
                       "TEST_RP  :",
                       "TEST_RL  :",
                       "TEST_RR  :",
                       "TEST_RRR :",
                       "TEST_RC  :",
                       "TEST_RT  :",
                       "TEST_RBM :"
                     };
//display the running time of programs 
unsigned long show_total_time(void)
{
     uint16_t i;
     unsigned long buf[15];
     unsigned long time_start = micros();
     for(i = 0;i< 15;i++)
     {
        buf[i] = show_function[i](); 
        clear_screen();  
     }
     for(i = 0;i<15; i++)
     {
         my_lcd.Set_Text_colour(255, 165, 0);   
         my_lcd.Set_Text_Size(1);
         my_lcd.Set_Text_Mode(1);
         my_lcd.Print_String(show_str[i], (my_lcd.Get_Display_Width()-260)/2-1, (my_lcd.Get_Display_Height()-150)/2+i*10-1);
         my_lcd.Set_Text_colour(0, 255, 0); 
         my_lcd.Print_Number_Int(buf[i], (my_lcd.Get_Display_Width()-260)/2-1+200, (my_lcd.Get_Display_Height()-150)/2+i*10-1, 0, ' ', 10);
     }
     delay(5000);
     return micros()- time_start; 
}

//display ending and total running time
void show_end(unsigned long run_time)
{
    my_lcd.Fill_Screen(0, 255, 255);
    my_lcd.Set_Draw_color(255, 0, 0);
    my_lcd.Fill_Round_Rectangle(my_lcd.Get_Display_Width()/2-1-120+1, my_lcd.Get_Display_Height()/2-1-60+1, my_lcd.Get_Display_Width()/2-1+120-1, my_lcd.Get_Display_Height()/2-1+60-1,5);
    show_string("Running over!",CENTER,my_lcd.Get_Display_Height()/2-1-40,1,0x07FF, 0,1);
    show_string("That's ok!",CENTER,my_lcd.Get_Display_Height()/2-1-30,1,0x07FF, 0,1);
    show_string("After a few seconds,",CENTER,my_lcd.Get_Display_Height()/2-1-20,1,0x07FF, 0,1);
    show_string("it will restart.",CENTER,my_lcd.Get_Display_Height()/2-1-10,1,0x07FF, 0,1);
    show_string("Please wait ...",CENTER,my_lcd.Get_Display_Height()/2-1,1,0x07FF, 0,1);
    show_string("Total runtime(us):  ",my_lcd.Get_Display_Width()/2-1-90,my_lcd.Get_Display_Height()/2-1+40,1,0xFFE0, 0,1);
    my_lcd.Set_Text_colour(0, 255, 0);
    my_lcd.Print_Number_Int(run_time, my_lcd.Get_Display_Width()/2-1+30, my_lcd.Get_Display_Height()/2-1+40, 0, ' ', 10);  
    delay(10000);   
}

void setup() 
{
  my_lcd.Init_LCD();
  my_lcd.Fill_Screen(0x0);  
  my_lcd.Set_Rotation(1);  
}

void loop() 
{
    unsigned long total_time;
    my_lcd.Fill_Screen(0x0); 
    total_time = show_total_time();
    show_end(total_time);    
}

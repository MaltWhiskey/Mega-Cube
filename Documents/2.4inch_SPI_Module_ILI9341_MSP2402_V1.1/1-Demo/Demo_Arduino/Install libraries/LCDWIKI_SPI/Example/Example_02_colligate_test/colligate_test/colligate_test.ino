// IMPORTANT: LCDWIKI_SPI LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.

//This program is a demo of how to use most of the functions 
//of the library with a supported display modules.

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


unsigned long show_text(void)
{
    unsigned long time_start = micros();
    my_lcd.Set_Draw_color(32, 0,255);
    my_lcd.Fill_Rectangle(0, 0, my_lcd.Get_Display_Width()-1, 10);
    my_lcd.Set_Text_colour(0, 255, 0);   
    my_lcd.Set_Text_Size(1);
    my_lcd.Set_Text_Mode(1);
    my_lcd.Print_String("* Color TFT Display *", CENTER, 2);

    my_lcd.Set_Draw_color(128, 128, 128);
    my_lcd.Fill_Rectangle(0, my_lcd.Get_Display_Height()-1-10, my_lcd.Get_Display_Width()-1, my_lcd.Get_Display_Height()-1);
    my_lcd.Set_Text_colour(255, 255, 255);   
    my_lcd.Set_Text_Size(1);
    my_lcd.Set_Text_Mode(1);
    my_lcd.Print_String("<http://abcde12345.com>", CENTER, my_lcd.Get_Display_Height()-9);

    my_lcd.Set_Draw_color(255,255,0); 
    my_lcd.Draw_Rectangle(0, 11, my_lcd.Get_Display_Width()-1, my_lcd.Get_Display_Height()-1-11);   
    return micros() - time_start;
}

unsigned long show_triangle_function(void)
{
     uint16_t i;
     unsigned long time_start = micros();
     my_lcd.Set_Draw_color(0, 0, 255); 
     my_lcd.Draw_Fast_VLine(my_lcd.Get_Display_Width()/2-1, 12, my_lcd.Get_Display_Height()- 24);
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
     my_lcd.Set_Text_colour(0, 255, 255);
     my_lcd.Set_Text_Back_colour(0,0,0);   
     my_lcd.Set_Text_Size(1);
     my_lcd.Set_Text_Mode(0);
     my_lcd.Print_String("sin",5,17);
     my_lcd.Set_Draw_color(0, 255, 255); 
     for (i=1; i<my_lcd.Get_Display_Width()-2; i++)
     {
        my_lcd.Draw_Pixel(i,my_lcd.Get_Display_Height()/2-1+(sin(((i*2.27)*3.14)/180)*40));
     }
     
     my_lcd.Set_Text_colour(0, 255, 0);
     my_lcd.Set_Text_Back_colour(0,0,0);   
     my_lcd.Set_Text_Size(1);
     my_lcd.Set_Text_Mode(0);
     my_lcd.Print_String("cos",5,25);
     my_lcd.Set_Draw_color(0, 255, 0);
     for (i=1; i<my_lcd.Get_Display_Width()-2; i++)
     {
        my_lcd.Draw_Pixel(i,my_lcd.Get_Display_Height()/2-1+(cos(((i*2.27)*3.14)/180)*40));
     }
      
     my_lcd.Set_Text_colour(255, 255, 0); 
     my_lcd.Set_Text_Back_colour(0,0,0);  
     my_lcd.Set_Text_Size(1);
     my_lcd.Set_Text_Mode(0);
     my_lcd.Print_String("tan",5,33);
     my_lcd.Set_Draw_color(255, 255, 0);
     for (i=1; i<my_lcd.Get_Display_Width()-2; i++)
     {
        my_lcd.Draw_Pixel(i,my_lcd.Get_Display_Height()/2-1+(tan(((i*2.27)*3.14)/180)*5));
     }

     my_lcd.Set_Text_colour(255, 0, 0); 
     my_lcd.Set_Text_Back_colour(0,0,0);  
     my_lcd.Set_Text_Size(1);
     my_lcd.Set_Text_Mode(0);
     my_lcd.Print_String("cot",5,41);
     my_lcd.Set_Draw_color(255, 0, 0);
     for (i=1; i<my_lcd.Get_Display_Width()-2; i++)
     {
        my_lcd.Draw_Pixel(i,my_lcd.Get_Display_Height()/2-1+1/(tan(((i*2.27)*3.14)/180)*0.2));
     }
     return micros()-time_start;
}

unsigned long show_sinewave(void)
{
   uint16_t buf[my_lcd.Get_Display_Width()-2],x = 1,i,y;
   unsigned long time_start = micros();
   int16_t t = 20, b = 40;
   float k = 2.5;
   
   my_lcd.Set_Draw_color(0, 0, 255); 
   my_lcd.Draw_Fast_VLine(my_lcd.Get_Display_Width()/2-1, 12, my_lcd.Get_Display_Height()- 24);
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
    y=my_lcd.Get_Display_Height()/2-1+(sin(((i*k)*3.14)/180)*(b-(i/100)));
    my_lcd.Draw_Pixel(x,y);
    buf[x-1]=y;
  }
  return micros()- time_start;   
}

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
          my_lcd.Fill_Round_Rectangle(x_spec+i*side_len-1, y_spec+i*side_len-1, x_spec+(i+1)*side_len-1, y_spec+(i+1)*side_len-1,5);
          my_lcd.Fill_Round_Rectangle(x_spec+i*side_len-1, y_spec+(5-i)*side_len-1, x_spec+(i+1)*side_len-1, y_spec+(4-i)*side_len-1,5); 
     }
     return micros()- time_start;   
}

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

unsigned long show_grid_lines(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    int16_t wid = my_lcd.Get_Display_Width();
   float k = 1.44;

    my_lcd.Set_Draw_color(255,0,0);
    for (i=12; i<my_lcd.Get_Display_Height()-1-12; i+=5)
    {
       my_lcd.Draw_Line(1, i, (i*k)-10, my_lcd.Get_Display_Height()-2-12);
     }
     my_lcd.Set_Draw_color(255,0,0);
     for (i=my_lcd.Get_Display_Height()-2-12; i>13; i-=5)
    {
      my_lcd.Draw_Line(my_lcd.Get_Display_Width()-2, i, (i*k)-10, 12);
    }
    my_lcd.Set_Draw_color(0,255,255);
    for (i=my_lcd.Get_Display_Height()-2-12; i>13; i-=5)
    {
        my_lcd.Draw_Line(1, i, (my_lcd.Get_Display_Height()-1-12)*k+10-(i*k), 12);
    }
    my_lcd.Set_Draw_color(0,255,255);
    for (int i=13; i<my_lcd.Get_Display_Height()-2-12; i+=5)
    {
        my_lcd.Draw_Line(my_lcd.Get_Display_Width()-2, i, (my_lcd.Get_Display_Height()-1-12)*k+10-(i*k), my_lcd.Get_Display_Height()-2-12);
    }
    return micros()- time_start;   
}

unsigned long show_random_pixels(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 10000;i++)
    {
       my_lcd.Set_Draw_color(random(255),random(255),random(255));
       my_lcd.Draw_Pixel(2+random(my_lcd.Get_Display_Width()-4),12+random(my_lcd.Get_Display_Height()-24));
    }
    return micros()- time_start; 
}

unsigned long show_random_lines(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 300;i++)
    {
       my_lcd.Set_Draw_color(random(255),random(255),random(255));
       my_lcd.Draw_Line(2+random(my_lcd.Get_Display_Width()-4),12+random(my_lcd.Get_Display_Height()-24),2+random(my_lcd.Get_Display_Width()-4),12+random(my_lcd.Get_Display_Height()-24));
    }
    return micros()- time_start; 
}

unsigned long show_random_rectangles(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 150;i++)
    {
       my_lcd.Set_Draw_color(random(255),random(255),random(255));
       my_lcd.Draw_Rectangle(2+random(my_lcd.Get_Display_Width()-4),12+random(my_lcd.Get_Display_Height()-24),2+random(my_lcd.Get_Display_Width()-4),12+random(my_lcd.Get_Display_Height()-24));
    }
    return micros()- time_start; 
}

unsigned long show_random_round_rectangles(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 150;i++)
    {
       my_lcd.Set_Draw_color(random(255),random(255),random(255));
       my_lcd.Draw_Round_Rectangle(2+random(my_lcd.Get_Display_Width()-4),12+random(my_lcd.Get_Display_Height()-24),2+random(my_lcd.Get_Display_Width()-4),12+random(my_lcd.Get_Display_Height()-24),5);
    }
    return micros()- time_start; 
}

unsigned long show_random_circles(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 150;i++)
    {
       my_lcd.Set_Draw_color(random(255),random(255),random(255));
       my_lcd.Draw_Circle(41+random(my_lcd.Get_Display_Width()-82), 52+random(my_lcd.Get_Display_Height()-104), random(40));
    }
    return micros()- time_start; 
}

unsigned long show_random_triangles(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 150;i++)
    {
       my_lcd.Set_Draw_color(random(255),random(255),random(255));
       my_lcd.Draw_Triangle(2+random(my_lcd.Get_Display_Width()-4),12+random(my_lcd.Get_Display_Height()-24),2+random(my_lcd.Get_Display_Width()-4),12+random(my_lcd.Get_Display_Height()-24),2+random(my_lcd.Get_Display_Width()-4),12+random(my_lcd.Get_Display_Height()-24));
    }
    return micros()- time_start; 
}

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
        my_lcd.Draw_Bit_Map(my_lcd.Get_Display_Width()/2-1-((len/2)*4/3+1)*i, my_lcd.Get_Display_Height()/2-1-(len/2)*i, 8, 6, buf, i*(len/6));
        delay(100);
    }
    return micros()- time_start; 
}

void clear_screen(void)
{
   delay(2000);  
   my_lcd.Set_Draw_color(0, 0, 0);
   my_lcd.Fill_Rectangle(1, 12, my_lcd.Get_Display_Width()-2, my_lcd.Get_Display_Height()-1-12);
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
uint8_t *show_str[]=
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
     for(i = 0;i<12; i++)
     {
         my_lcd.Set_Text_colour(255, 165, 0);   
         my_lcd.Set_Text_Size(1);
         my_lcd.Set_Text_Mode(1);
         my_lcd.Print_String(show_str[i], (my_lcd.Get_Display_Width()-130)/2-1, (my_lcd.Get_Display_Height()-96)/2+i*8-1);
         my_lcd.Set_Text_colour(0, 255, 0); 
         my_lcd.Print_Number_Int(buf[i], (my_lcd.Get_Display_Width()-130)/2-1+70, (my_lcd.Get_Display_Height()-96)/2+i*8-1, 0, ' ', 10);
     }
     delay(5000);
     return micros()- time_start; 
}

void show_end(unsigned long run_time)
{
    my_lcd.Fill_Screen(0, 255, 255);
    my_lcd.Set_Draw_color(255, 0, 0);
    my_lcd.Fill_Round_Rectangle(my_lcd.Get_Display_Width()/2-1-70+1, my_lcd.Get_Display_Height()/2-1-40+1, my_lcd.Get_Display_Width()/2-1+70-1, my_lcd.Get_Display_Height()/2-1+40-1,5);
    my_lcd.Set_Text_colour(0, 255, 255);   
    my_lcd.Set_Text_Size(1);
    my_lcd.Set_Text_Mode(1);
    my_lcd.Print_String("Running over!", CENTER, my_lcd.Get_Display_Height()/2-1-30);
    my_lcd.Print_String("That's ok!", CENTER, my_lcd.Get_Display_Height()/2-1-20);
    my_lcd.Print_String("it will restart.", CENTER, my_lcd.Get_Display_Height()/2-1-10);
    my_lcd.Print_String("Please wait ...", CENTER, my_lcd.Get_Display_Height()/2-1);
    my_lcd.Set_Text_colour(255, 255, 0); 
    my_lcd.Print_String("runtime(us): ", my_lcd.Get_Display_Width()/2-1-65, my_lcd.Get_Display_Height()/2-1+25);
    my_lcd.Set_Text_colour(0, 255, 0);
    my_lcd.Print_Number_Int(run_time, my_lcd.Get_Display_Width()/2-1+12, my_lcd.Get_Display_Height()/2-1+25, 0, ' ', 10);  
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

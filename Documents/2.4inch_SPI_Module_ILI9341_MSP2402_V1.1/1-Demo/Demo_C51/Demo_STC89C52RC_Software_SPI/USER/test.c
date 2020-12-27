//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//测试硬件：单片机STC89C52RC,晶振30M  单片机工作电压3.3V
//QDtech-TFT液晶驱动 for C51
//xiao冯@ShenZhen QDtech co.,LTD
//公司网站:www.qdtft.com
//淘宝网站：http://qdtech.taobao.com
//wiki技术网站：http://www.lcdwiki.com
//我司提供技术支持，任何技术问题欢迎随时交流学习
//固话(传真) :+86 0755-23594567 
//手机:15989313508（冯工） 
//邮箱:lcdwiki01@gmail.com    support@lcdwiki.com    goodtft@163.com
//技术支持QQ:3002773612  3002778157
//技术交流QQ群:324828016
//创建日期:2018/7/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 深圳市全动电子技术有限公司 2018-2028
//All rights reserved
//********************************************************************************
//=========================================电源接线================================================//
//5V接DC 5V电源
//GND接地
//=======================================液晶屏数据线接线==========================================//
//本模块默认数据总线类型为SPI
//液晶屏模块            单片机
// SDI(MOSI)     接       P15        //SPI写信号
// SDO(MISO)     接       P16        //SPI读信号，如果不需要读功能，此管脚可不接
//=======================================液晶屏控制线接线==========================================//
//液晶屏模块            单片机
//  CS           接       P13       //片选控制信号
//  RESET        接       P33       //复位信号
//  DC           接       P12       //数据/命令选择控制信号
//  SCK          接       P17       //SPI时钟信号
//  LED          接       P32       //背光控制信号，如果使用STC89C52RC单片机或者不需要控制，接3.3V
//=========================================触摸屏接线=========================================//
//STC89C52RC单片机ROM太小，无法运行带触摸功能的程序，所以触摸屏不需要接线
//不使用触摸或者模块本身不带触摸，则可不连接
//触摸屏使用的数据总线类型为SPI
//触摸屏模块            单片机
//  T_CLK        接       P36       //触摸屏SPI时钟信号
//  T_CS         接       P37       //触摸屏片选控制信号
//  T_DIN        接       P34       //触摸屏SPI写信号
//  T_DO         接       P35       //触摸屏SPI读信号
//  T_IRQ        接       P40       //触摸屏响应检查信号，如单片机无P4组，请自行更改其他可用IO并修改代码定义
//**************************************************************************************************/	
#include "lcd.h"
#include "string.h"
#include "sys.h"
#include "gui.h"
#include "test.h"
#include "touch.h"
#include "pic.h"


//========================variable==========================//
u16 ColorTab[5]={BRED,YELLOW,RED,GREEN,BLUE};//定义颜色数组
//=====================end of variable======================//

/*****************************************************************************
 * @name       :void DrawTestPage(u8 *str)
 * @date       :2018-08-09 
 * @function   :Drawing test interface
 * @parameters :str:the start address of the Chinese and English strings
 * @retvalue   :None
******************************************************************************/ 
void DrawTestPage(u8 *str)
{
//绘制固定栏up
LCD_Fill(0,0,lcddev.width,20,BLUE);
//绘制固定栏down
LCD_Fill(0,lcddev.height-20,lcddev.width,lcddev.height,BLUE);
POINT_COLOR=WHITE;
Gui_StrCenter(0,2,WHITE,BLUE,str,16,1);//居中显示
Gui_StrCenter(0,lcddev.height-18,WHITE,BLUE,"QDtech版权所有",16,1);//居中显示
//绘制测试区域
LCD_Fill(0,20,lcddev.width,lcddev.height-20,BLACK);
}

/*****************************************************************************
 * @name       :void main_test(void)
 * @date       :2018-08-09 
 * @function   :Drawing the main Interface of the Comprehensive Test Program
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void main_test(void)
{
	DrawTestPage("全动电子综合测试程序");
	
	Gui_StrCenter(0,30,RED,BLUE,"全动电子",16,1);//居中显示
	Gui_StrCenter(0,60,RED,BLUE,"综合测试程序",16,1);//居中显示	
	Gui_StrCenter(0,90,GREEN,BLUE,"2.4' ILI9341 240X320",16,1);//居中显示
	Gui_StrCenter(0,120,BLUE,BLUE,"xiaoFeng@QDtech 2018-08-20",16,1);//居中显示
	delay_ms(1500);		
	delay_ms(1500);
}

/*****************************************************************************
 * @name       :void Test_Color(void)
 * @date       :2018-08-09 
 * @function   :Color fill test(white,black,red,green,blue)
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void Test_Color(void)
{
	DrawTestPage("测试1:纯色填充测试");
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
	Show_Str(lcddev.width-50,30,BLUE,YELLOW,"White",16,1);delay_ms(500);
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,BLACK);
	Show_Str(lcddev.width-50,30,BLUE,YELLOW,"Black",16,1);delay_ms(500);
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,RED);
	Show_Str(lcddev.width-50,30,BLUE,YELLOW,"Red",16,1); delay_ms(500);
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,GREEN);
	Show_Str(lcddev.width-50,30,BLUE,YELLOW,"Green",16,1);delay_ms(500);
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,BLUE);
	Show_Str(lcddev.width-50,30,WHITE,YELLOW,"Blue",16,1);delay_ms(500);

}

/*****************************************************************************
 * @name       :void Test_FillRec(void)
 * @date       :2018-08-09 
 * @function   :Rectangular display and fill test
								Display red,green,blue,yellow,pink rectangular boxes in turn,
								1500 milliseconds later,
								Fill the rectangle in red,green,blue,yellow and pink in turn
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void Test_FillRec(void)
{
	u8 i=0;
	DrawTestPage("测试2:GUI矩形填充测试");
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
	for (i=0; i<5; i++) 
	{
		LCD_DrawRectangle(lcddev.width/2-80+(i*15),lcddev.height/2-80+(i*15),lcddev.width/2-80+(i*15)+60,lcddev.height/2-80+(i*15)+60); 
		POINT_COLOR=ColorTab[i];
	}
	delay_ms(1500);	
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE); 
	for (i=0; i<5; i++) 
	{
		LCD_DrawFillRectangle(lcddev.width/2-80+(i*15),lcddev.height/2-80+(i*15),lcddev.width/2-80+(i*15)+60,lcddev.height/2-80+(i*15)+60); 
		POINT_COLOR=ColorTab[i];
	}
	delay_ms(1500);
}

/*****************************************************************************
 * @name       :void Test_Circle(void)
 * @date       :2018-08-09 
 * @function   :circular display and fill test
								Display red,green,blue,yellow,pink circular boxes in turn,
								1500 milliseconds later,
								Fill the circular in red,green,blue,yellow and pink in turn
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void Test_Circle(void)
{
	u8 i=0;
	DrawTestPage("测试3:GUI画圆填充测试");
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
	for (i=0; i<5; i++)  
		gui_circle(lcddev.width/2-80+(i*25),lcddev.height/2-50+(i*25),ColorTab[i],30,0);
	delay_ms(1500);	
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE); 
	for (i=0; i<5; i++) 
	  	gui_circle(lcddev.width/2-80+(i*25),lcddev.height/2-50+(i*25),ColorTab[i],30,1);
	delay_ms(1500);
}

/*****************************************************************************
 * @name       :void English_Font_test(void)
 * @date       :2018-08-09 
 * @function   :English display test
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void English_Font_test(void)
{
	DrawTestPage("测试4:英文显示测试");
	POINT_COLOR=RED;
	BACK_COLOR=BLUE;
	LCD_ShowString(10,30,12,"6X12:abcdefghijklmnopqrstuvwxyz",0);
	LCD_ShowString(10,45,12,"6X12:ABCDEFGHIJKLMNOP0123456789",1);
	LCD_ShowString(10,60,12,"6X12:~!@#$%^&*()_+{}:<>?/|-+.",0);
	LCD_ShowString(10,80,16,"8X16:abcdefghijklmnopqrstuvw",0);
	LCD_ShowString(10,100,16,"8X16:ABCDEFGHIJKLMN01234567",1);
	LCD_ShowString(10,120,16,"8X16:~!@#$%^&*()_+{}:<>?/|",0); 
	delay_ms(1200);
}

/*****************************************************************************
 * @name       :void Chinese_Font_test(void)
 * @date       :2018-08-09 
 * @function   :chinese display test
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void Chinese_Font_test(void)
{	
	DrawTestPage("测试5:中文显示测试");
	Show_Str(10,30,BLUE,YELLOW,"16X16:全动电子技术有限公司欢迎您",16,0);
	Show_Str(10,50,BLUE,YELLOW,"16X16:Welcome全动电子",16,1);
	Show_Str(10,70,BLUE,YELLOW,"24X24:深圳市中文测试",24,1);
	Show_Str(10,100,BLUE,YELLOW,"32X32:字体测试",32,1);
	delay_ms(1200);
}

/*****************************************************************************
 * @name       :void Pic_test(void)
 * @date       :2018-08-09 
 * @function   :picture display test
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void Pic_test(void)
{
	DrawTestPage("测试6:图片显示测试");
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
	Gui_Drawbmp16(30,30,gImage_qq);
	Show_Str(30+12,75,BLUE,YELLOW,"QQ",16,1);
	Gui_Drawbmp16(90,30,gImage_qq);
	Show_Str(90+12,75,BLUE,YELLOW,"QQ",16,1);
	Gui_Drawbmp16(150,30,gImage_qq);
	Show_Str(150+12,75,BLUE,YELLOW,"QQ",16,1);
	delay_ms(1200);
}

/*****************************************************************************
 * @name       :void Touch_Test(void)
 * @date       :2018-08-09 
 * @function   :touch test
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void Touch_Test(void)
{ 
	u8 i=0;
	u16 j=0;
	u16 colorTemp=0;		
	DrawTestPage("测试7:Touch测试");
	LCD_ShowString(lcddev.width-24,0,16,"RST",1);//显示清屏区域
	LCD_Fill(lcddev.width-52,2,lcddev.width-50+20,18,RED); 
	POINT_COLOR=RED;
	while(1)
	{	 	 
		if(Convert_Pos())			//触摸屏被按下
		{	
		 	if(tp_pixlcd.x<lcddev.width&&tp_pixlcd.y<lcddev.height)
			{	
				if(tp_pixlcd.x>(lcddev.width-24)&&tp_pixlcd.y<16)
				{
					DrawTestPage("测试7:Touch测试");//清除
					POINT_COLOR=colorTemp;
					LCD_ShowString(lcddev.width-24,0,16,"RST",1);//显示清屏区域
					LCD_Fill(lcddev.width-52,2,lcddev.width-50+20,18,POINT_COLOR); 
				}
				else if((tp_pixlcd.x>(lcddev.width-60)&&tp_pixlcd.x<(lcddev.width-50+20))&&tp_pixlcd.y<20)
				{
				LCD_Fill(lcddev.width-52,2,lcddev.width-50+20,18,ColorTab[j%5]); 
				POINT_COLOR=ColorTab[(j++)%5];
				colorTemp=POINT_COLOR;
				delay_ms(10);
				}

				else 
				TP_Draw_Big_Point(tp_pixlcd.x,tp_pixlcd.y,POINT_COLOR);    			   
			}
		}else delay_ms(10);	//没有按键按下的时候 	    				
	}   
}
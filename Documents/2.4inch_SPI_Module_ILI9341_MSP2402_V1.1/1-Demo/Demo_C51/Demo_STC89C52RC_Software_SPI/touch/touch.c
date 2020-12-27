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
#include "sys.h"
#include "touch.h"
#include "lcd.h"
#include "gui.h"

//***因触摸屏批次不同等原因，默认的校准参数值可能会引起触摸识别不准，建议校准后再使用，不建议使用固定的默认校准参数
u16 vx=15621,vy=11221;  //比例因子，此值除以1000之后表示多少个AD值代表一个像素点 
u16 chx=3890,chy=340;//默认像素点坐标为0时的AD起始值 
//***因触摸屏批次不同等原因，默认的校准参数值可能会引起触摸识别不准，建议校准后再使用，不建议使用固定的默认校准参数

struct tp_pix_  tp_pixad,tp_pixlcd;	 //当前触控坐标的AD值,前触控坐标的像素值   

//返回触摸按下的状态
u8 tpstate(void)
{
	return 	Penirq;
}

//SPI开始
void spistar(void)                                     
{
	TCS=1;
	DCLK=1;
	DIN=1;
	DCLK=1;
}
//SPI写数据
void WriteCharTo7843(unsigned char num)          
{
	unsigned char count=0;
	DCLK=0;
	for(count=0;count<8;count++)
	{
		num<<=1;
		DIN=CY;
		DCLK=0; _nop_();_nop_();_nop_();                //上升沿有效
		DCLK=1; _nop_();_nop_();_nop_();
	}
}

//SPI 读数据
u16 ReadFromCharFrom7843()             
{
	u8 count=0;
	u16 Num=0;
	for(count=0;count<12;count++)
	{
		Num<<=1;		
		DCLK=1; _nop_();_nop_();_nop_();   //下降沿有效
		DCLK=0; _nop_();_nop_();_nop_();
		if(DOUT)
		{
			Num|=1;
		}
		
	}

return(Num);
}	
//从7846/7843/XPT2046/UH7843/UH7846读取adc值	  
//0x90=y   0xd0-x
u16 ADS_Read_AD(unsigned char CMD)          
{
	u16 l;
	TCS=0;
	WriteCharTo7843(CMD);        //送控制字即用差分方式读X坐标 详细请见有关资料
	DCLK=1; _nop_();_nop_();_nop_();_nop_();
	DCLK=0; _nop_();_nop_();_nop_();_nop_();
	l=ReadFromCharFrom7843();
	TCS=1;
	return l;
}		   
//读取一个坐标值
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值 
#define READ_TIMES 15 //读取次数
#define LOST_VAL 5	  //丢弃值
u16 ADS_Read_XY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)
	{				 
		buf[i]=ADS_Read_AD(xy);	    
	}				    
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
//带滤波的坐标读取
//最小值不能少于100.
u8 Read_ADS(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=ADS_Read_XY(CMD_RDX);
	ytemp=ADS_Read_XY(CMD_RDY);	 									   
	if(xtemp<100||ytemp<100)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;//读数成功
}
//2次读取ADS7846,连续读取2次有效的AD值,且这两次的偏差不能超过
//50,满足条件,则认为读数正确,否则读数错误.	   
//该函数能大大提高准确度
#define ERR_RANGE 20 //误差范围 
u8 Read_ADS2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=Read_ADS(&x1,&y1);   
    if(flag==0)return(0);
    flag=Read_ADS(&x2,&y2);	
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-ERR_RANGE内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)>>1;
        *y=(y1+y2)>>1;		
        return 1;
    }else return 0;	  
} 
//精确读取一次坐标,校准的时候用的	   
u8 Read_TP_Once(void)
{
	u8 re=0;
	u16 x1,y1;
	while(re==0)
	{
		while(!Read_ADS2(&tp_pixad.x,&tp_pixad.y));
		delay_ms(10);
		while(!Read_ADS2(&x1,&y1));
		if(tp_pixad.x==x1&&tp_pixad.y==y1)
		{
			re=1; 
		}
	} 
	return re;
}
//画一个校准用的辅助触摸区域
void Drow_Touch_Point(u16 x,u16 y)
{
	POINT_COLOR=RED;
	LCD_DrawLine(x-12,y,x+13,y);//横线
	LCD_DrawLine(x,y-12,x,y+13);//竖线
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	gui_circle(x,y,POINT_COLOR,6,0);//画中心圈
}	  
//画一个大点(2*2的点)		   
//x,y:坐标
//color:颜色
void TP_Draw_Big_Point(u16 x,u16 y,u16 color)
{	   
	POINT_COLOR=color;
	
	LCD_DrawPoint(x,y);//中心点 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);	 	  	
}	  
//转换结果
//根据触摸屏的校准参数来决定转换后的结果,保存在tp_pixlcd.x,tp_pixlcd.y中
u8 Convert_Pos(void)
{	
	#if USE_HORIZONTAL==1
	u16 temp; 
	#endif 	 
	u8 l=0;
	
	if(Read_ADS2(&tp_pixad.x,&tp_pixad.y))
	{
		l=1;
		tp_pixlcd.x=tp_pixad.x>chx?((u32)tp_pixad.x-(u32)chx)*1000/vx:((u32)chx-(u32)tp_pixad.x)*1000/vx;
		tp_pixlcd.y=tp_pixad.y>chy?((u32)tp_pixad.y-(u32)chy)*1000/vy:((u32)chy-(u32)tp_pixad.y)*1000/vy;
	}
	#if USE_HORIZONTAL==1
	temp=tp_pixlcd.x;
	tp_pixlcd.x=tp_pixlcd.y;
	tp_pixlcd.y=temp;
	tp_pixlcd.x=lcddev.width-tp_pixlcd.x;
	
	tp_pixlcd.x=lcddev.width-tp_pixlcd.x;
	tp_pixlcd.y=lcddev.height-tp_pixlcd.y;  
	#endif 
	return l;
}	    
//触摸屏校准代码
//得到四个校准参数
#define tp_pianyi 50   //校准坐标偏移量	
#define tp_xiaozhun 1000   //校准精度
void Touch_Adjust(void)
{	
	float vx1,vx2,vy1,vy2;  //比例因子，此值除以1000之后表示多少个AD值代表一个像素点
	u16 chx1,chx2,chy1,chy2;//默认像素点坐标为0时的AD起始值
	u16 lx,ly;				 
	struct tp_pixu32_ p[4];
	u8  cnt=0;	 
	cnt=0;				
	POINT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	LCD_Clear(WHITE);//清屏   
	POINT_COLOR=RED;//红色 
	LCD_Clear(WHITE);//清屏 
	Drow_Touch_Point(tp_pianyi,tp_pianyi);//画点1 
	while(1)
	{
		if(Penirq==0)//按键按下了
		{
			if(Read_TP_Once())//得到单次按键值
			{  								   
				p[cnt].x=tp_pixad.x;
				p[cnt].y=tp_pixad.y;
				cnt++; 
			}			 
			switch(cnt)
			{			   
				case 1:
					LCD_Clear(WHITE);//清屏 
					while(!Penirq)  //等待松手
					{
					}
					Drow_Touch_Point(lcddev.width-tp_pianyi-1,tp_pianyi);//画点2
					break;
				case 2:
					LCD_Clear(WHITE);//清屏 
					while(!Penirq)  //等待松手
					{
					}
					Drow_Touch_Point(tp_pianyi,lcddev.height-tp_pianyi-1);//画点3
					break;
				case 3:
					LCD_Clear(WHITE);//清屏 
					while(!Penirq)  //等待松手
					{
					}
					Drow_Touch_Point(lcddev.width-tp_pianyi-1,lcddev.height-tp_pianyi-1);//画点4
					break;
				case 4:	 //全部四个点已经得到
	    		   	LCD_Clear(WHITE);//清屏 
				   	while(!Penirq)  //等待松手
					{
					}
			   		vx1=p[1].x>p[0].x?(p[1].x-p[0].x+1)*1000/(lcddev.width-tp_pianyi-tp_pianyi):(p[0].x-p[1].x-1)*1000/(lcddev.width-tp_pianyi-tp_pianyi);
				 	chx1=p[1].x>p[0].x?p[0].x-(vx1*tp_pianyi)/1000:p[0].x+(vx1*tp_pianyi)/1000;
				   	vy1=p[2].y>p[0].y?(p[2].y-p[0].y-1)*1000/(lcddev.height-tp_pianyi-tp_pianyi):(p[0].y-p[2].y-1)*1000/(lcddev.height-tp_pianyi-tp_pianyi);
					chy1=p[2].y>p[0].y?p[0].y-(vy1*tp_pianyi)/1000:p[0].y+(vy1*tp_pianyi)/1000; 
					
					vx2=p[3].x>p[2].x?(p[3].x-p[2].x+1)*1000/(lcddev.width-tp_pianyi-tp_pianyi):(p[2].x-p[3].x-1)*1000/(lcddev.width-tp_pianyi-tp_pianyi);
					chx2=p[3].x>p[2].x?p[2].x-(vx2*tp_pianyi)/1000:p[2].x+(vx2*tp_pianyi)/1000;
				   	vy2=p[3].y>p[1].y?(p[3].y-p[1].y-1)*1000/(lcddev.height-tp_pianyi-tp_pianyi):(p[1].y-p[3].y-1)*1000/(lcddev.height-tp_pianyi-tp_pianyi);
					chy2=p[3].y>p[1].y?p[1].y-(vy2*tp_pianyi)/1000:p[1].y+(vy2*tp_pianyi)/1000; 


					if((vx1>vx2&&vx1>vx2+tp_xiaozhun)||(vx1<vx2&&vx1<vx2-tp_xiaozhun)||(vy1>vy2&&vy1>vy2+tp_xiaozhun)||(vy1<vy2&&vy1<vy2-tp_xiaozhun))
					{
						cnt=0;
						LCD_Clear(WHITE);//清屏 
						Drow_Touch_Point(tp_pianyi,tp_pianyi);//画点1 
						continue;
					}
					vx=(vx1+vx2)/2;vy=(vy1+vy2)/2;
					chx=(chx1+chx2)/2;chy=(chy1+chy2)/2;	
																
					//显示校准信息
					LCD_Clear(WHITE);//清屏 
					POINT_COLOR=BLACK;
					BACK_COLOR=BLUE;	
					/*
					lx=0;ly=50;			
					LCD_ShowString(lx,ly,"VX1:");lx+=40;LCD_ShowNum(lx,ly,vx1,4);					
					lx=0;ly+=20;
					LCD_ShowString(lx,ly,"Vy1:");lx+=40;LCD_ShowNum(lx,ly,vy1,4);					
					lx=0;ly+=20; 
					LCD_ShowString(lx,ly,"CHX1:");lx+=40;LCD_ShowNum(lx,ly,chx1,4);					
				    lx=0;ly+=20; 
					LCD_ShowString(lx,ly,"CHY1:");lx+=40;LCD_ShowNum(lx,ly,chy1,4);

					lx=100;ly=50;			
					LCD_ShowString(lx,ly,"VX2:");lx+=40;LCD_ShowNum(lx,ly,vx2,4);					
					lx=100;ly+=20;
					LCD_ShowString(lx,ly,"Vy2:");lx+=40;LCD_ShowNum(lx,ly,vy2,4);					
					lx=100;ly+=20; 
					LCD_ShowString(lx,ly,"CHX2:");lx+=40;LCD_ShowNum(lx,ly,chx2,4);					
				    lx=100;ly+=20; 
					LCD_ShowString(lx,ly,"CHY2:");lx+=40;LCD_ShowNum(lx,ly,chy2,4);
					*/
					lx=50;ly=150;			
					LCD_ShowString(lx,ly,16,"VX:",1);lx+=40;LCD_ShowNum(lx,ly,vx,6,16);					
					lx=50;ly+=20;
					LCD_ShowString(lx,ly,16,"Vy:",1);lx+=40;LCD_ShowNum(lx,ly,vy,6,16);					
					lx=50;ly+=20; 
					LCD_ShowString(lx,ly,16,"CHX:",1);lx+=40;LCD_ShowNum(lx,ly,chx,6,16);					
				    lx=50;ly+=20; 
					LCD_ShowString(lx,ly,16,"CHY:",1);lx+=40;LCD_ShowNum(lx,ly,chy,6,16);

					lx=30;ly+=30;
					LCD_ShowString(lx,ly,16,"Adjust OK!",1);	
					lx=30;ly+=30;
					LCD_ShowString(lx,ly,16,"Touch Anywhere To Continue",1);										  
					Read_TP_Once(); //等待任意键后继续

					LCD_Clear(WHITE);//清屏
					return;//校正完成				 
			}
		}
	} 
}
	    	

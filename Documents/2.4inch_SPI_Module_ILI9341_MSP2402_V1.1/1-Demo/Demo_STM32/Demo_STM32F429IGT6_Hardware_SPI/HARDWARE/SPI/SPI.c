//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//测试硬件：单片机STM32F429IGT6,正点原子Apollo STM32F4/F7开发板,主频180MHZ，晶振12MHZ
//QDtech-TFT液晶驱动 for STM32 IO模拟
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
//创建日期:2018/08/09
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 深圳市全动电子技术有限公司 2018-2028
//All rights reserved
/****************************************************************************************************
//=========================================电源接线================================================//
//     LCD模块                STM32单片机
//      VCC          接        DC5V/3.3V      //电源
//      GND          接          GND          //电源地
//=======================================液晶屏数据线接线==========================================//
//本模块默认数据总线类型为SPI总线
//     LCD模块                STM32单片机    
//    SDI(MOSI)      接          PF9          //液晶屏SPI总线数据写信号
//    SDO(MISO)      接          PF8          //液晶屏SPI总线数据读信号，如果不需要读，可以不接线
//=======================================液晶屏控制线接线==========================================//
//     LCD模块 					      STM32单片机 
//       LED         接          PD6          //液晶屏背光控制信号，如果不需要控制，接5V或3.3V
//       SCK         接          PF7          //液晶屏SPI总线时钟信号
//      DC/RS        接          PD5          //液晶屏数据/命令控制信号
//       RST         接          PD12         //液晶屏复位控制信号
//       CS          接          PD11         //液晶屏片选控制信号
//=========================================触摸屏触接线=========================================//
//如果模块不带触摸功能或者带有触摸功能，但是不需要触摸功能，则不需要进行触摸屏接线
//	   LCD模块                STM32单片机 
//      T_IRQ        接          PH11         //触摸屏触摸中断信号
//      T_DO         接          PG3          //触摸屏SPI总线读信号
//      T_DIN        接          PI3          //触摸屏SPI总线写信号
//      T_CS         接          PI8          //触摸屏片选控制信号
//      T_CLK        接          PH6          //触摸屏SPI总线时钟信号
**************************************************************************************************/		
 /* @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**************************************************************************************************/	
#include "spi.h"

SPI_HandleTypeDef SPI5_Handler;  //SPI句柄

/*****************************************************************************
 * @name       :u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte)
 * @date       :2018-08-09 
 * @function   :Write a byte of data using STM32's hardware SPI
 * @parameters :TxData:Data to be written
 * @retvalue   :Data received by the bus
******************************************************************************/
u8 SPI_WriteByte(u8 TxData)
{
    u8 Rxdata;
    HAL_SPI_TransmitReceive(&SPI5_Handler,&TxData,&Rxdata,1, 1000);       
 	return Rxdata;           	     //返回收到的数据			
} 

/*****************************************************************************
 * @name       :void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet)
 * @date       :2018-08-09 
 * @function   :Set hardware SPI Speed
 * @parameters :SPI_BaudRatePrescaler:SPI speed
 * @retvalue   :None
******************************************************************************/
void SPI_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
    __HAL_SPI_DISABLE(&SPI5_Handler);            //关闭SPI
    SPI5_Handler.Instance->CR1&=0XFFC7;          //位3-5清零，用来设置波特率
    SPI5_Handler.Instance->CR1|=SPI_BaudRatePrescaler;//设置SPI速度
    __HAL_SPI_ENABLE(&SPI5_Handler);   
} 


/*****************************************************************************
 * @name       :void SPI5_Init(void)	
 * @date       :2018-08-09 
 * @function   :Initialize the STM32 hardware SPI5
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void SPI5_Init(void)
{
    SPI5_Handler.Instance=SPI5;                         //SP5
    SPI5_Handler.Init.Mode=SPI_MODE_MASTER;             //设置SPI工作模式，设置为主模式
    SPI5_Handler.Init.Direction=SPI_DIRECTION_2LINES;   //设置SPI单向或者双向的数据模式:SPI设置为双线模式
    SPI5_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI5_Handler.Init.CLKPolarity=SPI_POLARITY_LOW;    //串行同步时钟的空闲状态为高电平
    SPI5_Handler.Init.CLKPhase=SPI_PHASE_1EDGE;         //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI5_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
  //  SPI5_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_2;//定义波特率预分频的值:波特率预分频值为256
    SPI5_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;        //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI5_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //关闭TI模式
    SPI5_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//关闭硬件CRC校验
    SPI5_Handler.Init.CRCPolynomial=7;                  //CRC值计算的多项式
    HAL_SPI_Init(&SPI5_Handler);//初始化
    
    __HAL_SPI_ENABLE(&SPI5_Handler);                    //使能SPI5
	                       //启动传输
}

/*****************************************************************************
 * @name       :void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)	
 * @date       :2018-08-09 
 * @function   :Configuring SPI pins and clocks,
                it will be called by HAL_SPI_Init
 * @parameters :hspi:SPI handle
 * @retvalue   :None
******************************************************************************/
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOF_CLK_ENABLE();       //使能GPIOF时钟
    __HAL_RCC_SPI5_CLK_ENABLE();        //使能SPI5时钟
    
    //PF7,8,9
    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;                  //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;             //快速            
    GPIO_Initure.Alternate=GPIO_AF5_SPI5;           //复用为SPI5
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
}

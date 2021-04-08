/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : Universal_Init.h
* Author        : 李美华
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _TIMER_H
#define _TIMER_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "decode.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
////大灯亮度控制引脚定义
//#define HEADLIGHT_CTRL_GPIO       GPIOC
//#define HEADLIGHT_CTRL_PIN        GPIO_Pin_8
//#define HEADLIGHT_GPIO_AF         GPIO_AF_0
//#define HEADLIGHT_CTRL_PINSOURCE  GPIO_PinSource8

//仪表氛围灯控制
#define Ambient_TIMER           TIM3
#define Ambient_TIMER_PERIOD    999

//把套加热
#define HANDLE_TIMER            TIM17
#define HANDLE_TIMER_PERIOD     49

//ADC相关宏定义
#define ADC_USE					ADC1
#define ADC_DMA					DMA1
#define ADC_DMA_CH	 		    DMA1_Channel1

typedef enum
{
    AdcChanelWirelessChargVoltage,  // 无线充电压检测      AIN0
    //AdcChanelWirelessChargFB,     // 无线充反馈检测      AIN1
    AdcChanelHandleFault,           // 把套加热错误检测    AIN2
    AdcChanelAccFault,              // 油门地线检测        AIN3
    
    AdcChanelAcc,                   // 油门Hall            AIN4
    AdcChanelRightBreakFault,       // 右刹地线检测        AIN5
    AdcChanelRightBreak,            // 右刹Hall            AIN6
    AdcChanelLeftBreakFault,        // 左刹地线检测        AIN7
    
    AdcChanelLeftHandleTemp,        // 左把套温度          AIN10
    AdcChanelRightHandleTemp,       // 右把套温度          AIN11
    AdcChanelRightLightOverCur,     // 左转向灯过流检测    AIN12
    AdcChanelLeftLightOverCur,      // 右转向灯过流检测    AIN13

    AdcChanelLeftBreak,             // 左刹Hall            AIN14
    
    AdcChanelMaxNumber              // 通道最大数量             
} AdcChanel;

//#define ADC_CHANNEL_NUM     	4		
#define ADC_BUF_LEN		        (AdcChanelMaxNumber * 8)
////对应的模拟量在g_ADC1Value[]中的位置
//#define ADC_CH_12V_COL		    0
//#define ADC_CH_Oil		        1
//#define ADC_CH_Oil_GND		    2
//#define ADC_CH_WAIRLESS_CHARGE	3

/* Exported types ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void ADC_Int(void);		

void ADC_Int_WakeUp(void);
u16 ADC_Get_ConversionValue(u8 Channel);
void Get_RealVaule(void);

void Ambient_Timer_Init(void);

void Handle_Timer_Init(void);  //把套加热
void Handle_Temp_OFF(void);
void Handle_Temp_ON(void);
void Handle_Temp_Set(u8 pow);

void Timer2Init(void);
void ADCDMAConfiguration(void);

#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

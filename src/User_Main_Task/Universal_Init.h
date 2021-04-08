/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : Universal_Init.h
* Author        : ������
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
////������ȿ������Ŷ���
//#define HEADLIGHT_CTRL_GPIO       GPIOC
//#define HEADLIGHT_CTRL_PIN        GPIO_Pin_8
//#define HEADLIGHT_GPIO_AF         GPIO_AF_0
//#define HEADLIGHT_CTRL_PINSOURCE  GPIO_PinSource8

//�Ǳ��Χ�ƿ���
#define Ambient_TIMER           TIM3
#define Ambient_TIMER_PERIOD    999

//���׼���
#define HANDLE_TIMER            TIM17
#define HANDLE_TIMER_PERIOD     49

//ADC��غ궨��
#define ADC_USE					ADC1
#define ADC_DMA					DMA1
#define ADC_DMA_CH	 		    DMA1_Channel1

typedef enum
{
    AdcChanelWirelessChargVoltage,  // ���߳��ѹ���      AIN0
    //AdcChanelWirelessChargFB,     // ���߳䷴�����      AIN1
    AdcChanelHandleFault,           // ���׼��ȴ�����    AIN2
    AdcChanelAccFault,              // ���ŵ��߼��        AIN3
    
    AdcChanelAcc,                   // ����Hall            AIN4
    AdcChanelRightBreakFault,       // ��ɲ���߼��        AIN5
    AdcChanelRightBreak,            // ��ɲHall            AIN6
    AdcChanelLeftBreakFault,        // ��ɲ���߼��        AIN7
    
    AdcChanelLeftHandleTemp,        // ������¶�          AIN10
    AdcChanelRightHandleTemp,       // �Ұ����¶�          AIN11
    AdcChanelRightLightOverCur,     // ��ת��ƹ������    AIN12
    AdcChanelLeftLightOverCur,      // ��ת��ƹ������    AIN13

    AdcChanelLeftBreak,             // ��ɲHall            AIN14
    
    AdcChanelMaxNumber              // ͨ���������             
} AdcChanel;

//#define ADC_CHANNEL_NUM     	4		
#define ADC_BUF_LEN		        (AdcChanelMaxNumber * 8)
////��Ӧ��ģ������g_ADC1Value[]�е�λ��
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

void Handle_Timer_Init(void);  //���׼���
void Handle_Temp_OFF(void);
void Handle_Temp_ON(void);
void Handle_Temp_Set(u8 pow);

void Timer2Init(void);
void ADCDMAConfiguration(void);

#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

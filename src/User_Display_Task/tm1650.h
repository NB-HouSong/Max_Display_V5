/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : ����650.h
* Author        : ������
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TM1650_H__
#define __TM1650_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "System_Config.h"
#include "StateUpdate.h"
#include "UART.h"

//#define  TM1652_SDA_GPIO  GPIOA
//#define  TM1652_SDA_PIN   GPIO_Pin_9
//#define  TX_HIGH()  GPIO_SetBits(TM1652_SDA_GPIO,TM1652_SDA_PIN)
//#define  TX_LOW()   GPIO_ResetBits(TM1652_SDA_GPIO,TM1652_SDA_PIN)
	
#define  DIS_ADDRESS_COM      0x08   //��ʾ��ַ����
#define  DIS_CONTROL_COM      0x18   //��ʾ��������
#define  DIS_CONTROL_ADJ_COM  0xBB//0xFF//0xBB   //��ʾ���Ƶ������� ���λռ�ձ�13/16�����ε���6/8


#define  KMPH   0
#define  MPH    1

#define Digital_1   1   //��һ������
#define Digital_2   2   //�ڶ�������

#ifndef ONLY_ONEPIN_DRIVE_DATA
//��Ʊ�ʶ �ӿ�ΪԤ��
#define ERROE_ON		      TM1652_Dig5 |= 0x20
#define ERROE_OFF		      TM1652_Dig5 &= ~0x20 

#define LAMP_ON		          TM1652_Dig5 |= 0x03    //��Ʊ�־
#define LAMP_OFF	          TM1652_Dig5 &= ~0x03

#define KM_H_ON			      TM1652_Dig5 |= 0x04    //��λ��ʶ
#define KM_H_OFF              TM1652_Dig5 &= ~0x04

#define MPH_ON			      TM1652_Dig5 |= 0x08
#define MPH_OFF			      TM1652_Dig5 &= ~0x08

#define BAT_SOC_RED_ON   	  TM1652_Dig5 |= 0x10
#define BAT_SOC_RED_OFF   	  TM1652_Dig5 &= ~0x10

#define BAT_SOC_LEVEL0	      TM1652_Dig6 &= ~0x1F
#define BAT_SOC_LEVEL1	      TM1652_Dig6 |= 0x01

#define BAT_SOC_LEVEL2	      TM1652_Dig6 |= 0x03
#define BAT_SOC_LEVEL3	      TM1652_Dig6 |= 0x07
#define BAT_SOC_LEVEL4	      TM1652_Dig6 |= 0x0F
#define BAT_SOC_LEVEL5	      TM1652_Dig6 |= 0x1F

#else
//��Ʊ�ʶ �ӿ�ΪԤ��
#define ERROE_ON_R		  TM1652_Dig6 |= 0x01
#define ERROE_OFF_R		  TM1652_Dig6 &= ~0x01 

#define ERROE_ON_G		  TM1652_Dig2 |= 0x01
#define ERROE_OFF_G		  TM1652_Dig2 &= ~0x01 

#define ERROE_ON_B		  TM1652_Dig4 |= 0x01
#define ERROE_OFF_B		  TM1652_Dig4 &= ~0x01 

#define LOCATE_ON_R		  TM1652_Dig6 |= 0x08
#define LOCATE_OFF_R	  TM1652_Dig6 &= ~0x08

#define LOCATE_ON_G		  TM1652_Dig2 |= 0x08
#define LOCATE_OFF_G	  TM1652_Dig2 &= ~0x08

#define LOCATE_ON_B		  TM1652_Dig4 |= 0x08
#define LOCATE_OFF_B	  TM1652_Dig4 &= ~0x08

#define NET_ON_R		  TM1652_Dig6 |= 0x04
#define NET_OFF_R		  TM1652_Dig6 &= ~0x04

#define NET_ON_G		  TM1652_Dig2 |= 0x04
#define NET_OFF_G		  TM1652_Dig2 &= ~0x04

#define NET_ON_B		  TM1652_Dig4 |= 0x04
#define NET_OFF_B		  TM1652_Dig4 &= ~0x04

#define BT_ON_R		      TM1652_Dig6 |= 0x02
#define BT_OFF_R		  TM1652_Dig6 &= ~0x02

#define BT_ON_G		      TM1652_Dig2 |= 0x02
#define BT_OFF_G		  TM1652_Dig2 &= ~0x02

#define BT_ON_B		      TM1652_Dig4 |= 0x02
#define BT_OFF_B		  TM1652_Dig4 &= ~0x02

#define LAMP_ON		      TM1652_Dig5 |= 0x0C    //��Ʊ�־
#define LAMP_OFF	      TM1652_Dig5 &= ~0x0C

#define KM_H_ON			  TM1652_Dig5 |= 0x10    //��λ��ʶ
#define KM_H_OFF          TM1652_Dig5 &= ~0x10

#define MPH_ON			  TM1652_Dig5 |= 0x20
#define MPH_OFF			  TM1652_Dig5 &= ~0x20

#define BAT_SOC_LEVEL0	  TM1652_Dig6 &= ~0x1F
#define BAT_SOC_LEVEL1	  TM1652_Dig6 |= 0x01
#define BAT_SOC_LEVEL2	  TM1652_Dig6 |= 0x03
#define BAT_SOC_LEVEL3	  TM1652_Dig6 |= 0x07
#define BAT_SOC_LEVEL4	  TM1652_Dig6 |= 0x0F
#define BAT_SOC_LEVEL5	  TM1652_Dig6 |= 0x1F



#endif

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  INIT_SHOW_MODE = 0,
  NUM_SEGMENT_MODE,
  RGB_MODE,
  AMBIENTLIGHT_MODE,
  DISPLAYMODE_MAXNUM,
}DISPLAYMODE;
            
/* Exported functions ------------------------------------------------------- */
void TM1652_Init(void);
//void tm1652_send_data(u8 sdat);
void tm1652_send_data(u8 sdat, u8* sendstep);
void Display_Handler(void);

#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

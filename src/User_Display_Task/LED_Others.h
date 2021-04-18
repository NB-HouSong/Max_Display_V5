/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : LED_Others.h
* Author        : 李美华
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _LED_OTHER_H
#define _LED_OTHER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "main.h"

/* Exported macro ------------------------------------------------------------*/
//氛围灯引脚定义
#define LED_TIM					TIM1
#define LED_GPIO				GPIOA
#define LED_PIN  		        GPIO_Pin_10

//1812输出通道（RGB编号）相关宏定义
#define RGB_NUM_RGB           1  //左右转向氛围灯连接到一起
//#define RGB_NUM_RIGHT_RGB   2  //右转向氛围灯

//刹车彩灯相关宏定义
#define RGB_OFF       			0  //关闭
#define RGB_RED       			1  //红色
#define RGB_BLUE      			2  //蓝色
#define RGB_PURPLE      		3  //紫色
#define RGB_GREEN     		 	4  //绿色
#define RGB_YELLOW      		5  //黄色
#define RGB_CHING      			6  //青色
#define RGB_WHITE      			7  //白色

/* Exported functions ------------------------------------------------------- */
void PowerON_RGB(void);
void Ambientlight_OFF(void);
void ALL_RGB_OFF(void);
//void Ambientlight_Control(u8 Flash_flage);
void Handle_RGB_Control(u8 Flash_flage);

void RGB_Time_Init(void);
void Ambientlight_breath(u8 num, u8 period, u8 color);

#endif


/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

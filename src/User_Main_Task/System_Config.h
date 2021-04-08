/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : variable.h
* Author        : 李美华
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYSTEM_CONFIG_H_
#define __SYSTEM_CONFIG_H_

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "stm32f0xx.h"
#include "tm1650.h"
#include "stdio.h"
#include "FM175XX.h" 
#include "CAN.h"
#include "UART_3.h"
#include "UART.h"
#include "flash.h"
#include "Ambient_Light_App.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

////无线充故障输入引脚定义 用于输入捕获
//#define CAPTURE_GPIO       GPIOA
//#define CAPTURE_PIN        GPIO_Pin_1

//充电器插入检测
#define CHARGER_IN_GPIO    GPIOC
#define CHARGER_IN_PIN     GPIO_Pin_9

#define CHARGER_IN_CHECK()   (CHARGER_IN_GPIO->IDR & CHARGER_IN_PIN ? 0 : 1)

/* Exported types ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SystemCoreClockConfigure(void);
void InitAllPeripherals( void );
void Variables_Init(void);
void delay_us(u32 us);
void delay_ms(u16 ms);
void Disable_Interrupt(void);
void Enable_Interrupt(void);
void SetErrorByte(u8 ErrorByte);
void ClearErrorByte(u8 ErrorByte);
void delay_init(void);
s32 SaveFDatamap(void);
void LOwpower_IO_Init(void);
void ClearErrorByteFromCAN(void);
#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

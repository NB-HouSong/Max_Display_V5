/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : main.h
* Author        : ¿Ó√¿ª™
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _MAIN_H
#define _MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "variable.h"
#include "UART.h"
#include "Gpio.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void Display_Task_Init(void);
extern void Display_Task(void *pvParameters);
extern void Control_Task(void *pvParameters);

#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

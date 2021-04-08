/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : UART.h
* Author        : 李美华
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UART_H
#define UART_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define TM1652_UART			    USART1	//TM1652通讯

#define EU1_TX_PIN						GPIO_Pin_9
#define EU1_TX_GPIO						GPIOA

/* Exported types ------------------------------------------------------------*/


extern volatile u8 g_EUART_Tx_Finish;

/* Exported functions ------------------------------------------------------- */
void UART_Int(void);				//UART相关初始化
void UART_CheckReceive(void);
void UART_CheckSend(void);	
//void UART_PushFrame(u8 src_id, u8 dst_id, u8 DataLen, u8 Cmd, u8 Index, u8*pdata);
void UART_PopFrame(void);
void UART_PushFrame(u8 DataLen, u8*pdata);

#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

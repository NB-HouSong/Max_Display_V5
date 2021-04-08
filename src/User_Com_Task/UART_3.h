/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : UART3.h
* Author        : 李美华
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UART3_H
#define UART3_H 

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "ParseFrame.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define EXT_UART3 	                USART3			//与BMS通讯

#define EUART3_DMA					DMA1
#define EUART3_RX_DMA_CH	 		DMA1_Channel3
#define EUART3_TX_DMA_CH	 		DMA1_Channel2

#define EU3_TX_PIN					GPIO_Pin_10
#define EU3_RX_PIN					GPIO_Pin_11
#define EU3_TX_GPIO					GPIOB
/* Exported types ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void UART3_Int(void);				//UART相关初始化
void UART3_CheckReceive(void);
void UART3_CheckSend(void);	
void UART3_PushFrame(u8 src_id, u8 dst_id, u8 DataLen, u8 Cmd, u8 Index, u8*pdata);
void UART3_PopFrame(void);

void testSend(uint8_t *str);
/**
 * @brief uart 接收帧数据注册回调函数
 * @param dataHandler， 回调函数地址.
 * @return no.
 */
void scooterDataSetCallback(void (*dataHandler)(const uint8_t *pData));

#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

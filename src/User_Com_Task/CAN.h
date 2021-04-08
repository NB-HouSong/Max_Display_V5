/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : CAN.h
* Author        : 李美华
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CAN_H
#define CAN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"                  // Device header
#include "ParseFrame.h"
#include "IAP.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
//CAN使能脚
#define CAN_EN_GPIO    GPIOA
#define CAN_EN_PIN     GPIO_Pin_11

#define CAN_EN()   CAN_EN_GPIO->ODR &= ~CAN_EN_PIN 
#define CAN_DIS()  CAN_EN_GPIO->ODR |= CAN_EN_PIN  

#define CAN_MAX_FRM_LEN     8

/* Exported types ------------------------------------------------------------*/
struct CAN_EXTID  //CAN的扩展ID
{
	u32 CAN_Index:8;       //索引 8位
	u32 CAN_Cmd:8;         //指令 8位
	u32 CAN_Dstid:6;       //目标地址 6位
	u32 CAN_Srcid:6;       //源地址 6位
};

typedef union
{
	struct CAN_EXTID params;
	volatile u32 Cache;
}CAN_EXT_ID;	

typedef struct
{
	uint8_t		filter_number;
	uint16_t	fifo;
	uint32_t	id;
	uint32_t	mask;
	uint8_t		is_ext;
} CanFilterClass;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
extern CanTxMsg TxMessage;
extern CanRxMsg RxMessage;

/* Exported functions ------------------------------------------------------- */
void User_CAN_Init(void);
void CAN_PushMulFrame(u8 data_length, u8 src_id, u8 dst_id, u8 cmd, u8 index,u8* pdata);
void CAN_Check_Send(void);
void CAN_Check_Recive(void);

#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

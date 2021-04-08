/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : ParseFrame.h
* Author        : 李美华
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PARASEFRAME_H
#define PARASEFRAME_H

/* Includes ------------------------------------------------------------------*/
#include "commu.h"
#include "IAP.h"
#include "StateUpdate.h"
#include "CAN.h"
#include "batmanage.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
//通讯通道宏定义
//#define CAN_SEL			1			//CAN通讯接口
//#define COM_BMS		    2			//USART3 IOT
#define COM_SCOOTER		0			//主控

//数据长度
#define MAX_USART_DATA_SIZE 150

//帧相关宏定义
#define FRM_LEN			0
#define FRM_SRC_ID	    1
#define FRM_DST_ID	    2
#define FRM_CMD			3
#define FRM_INDEX		4
#define FRM_DATA		5

/* Exported types ------------------------------------------------------------*/
typedef struct 
{

    u32 FILESIZE;              //文件总大小
    u32 FrameTotalCntper128;   //得到上位机发送固件最大的包数
    u32 FrameResiveCntper128;  //已接收的包数
    u8  FrameCntRemainper8;    //原文件肯定是8倍数，对128取余后除8得到帧数
    u8  FrameResiveCntper8;
}FRAME_RESIVE_T;

typedef struct 
{
  u8 SlaveCh;
	u8 SrcId;
	u8 DstId;
	u8 Cmd;
  u8 MasterCh;
} CHAN_READ_INFO;

typedef struct
{
    uint8_t data[MAX_USART_DATA_SIZE];
    uint8_t dataSize;
    uint8_t srcId;
    uint8_t dstId;
    uint8_t cmd;
    uint8_t index;
} UsartMessage;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
u16 CheckSum(u8* pdata, u32 len);
void ParseInit(void);
u8 FormatFrame(u8 src_id, u8 dst_id, u8 DataLen, u8 Cmd, u8 Index, u8* pdata, u8* FormatBuf);
u16 FormatFrame_Ex(u8 src_id, u8 dst_id, u16 DataLen, u8 Cmd, u16 Index, u8* pdata, u8* FormatBuf);
void ParseFrame(u8 Channel, u8* pData);
void ParseFrame_EX(u8 Channel, u8* pData);
void PushMiniFrame(u8 src_id, u8 dst_id, u8 DataLen, u8 Cmd, u8 Index, u8*pdata, u8 Channel);
void CanParseDataHandler(CanRxMsg* pRxMessage);

#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

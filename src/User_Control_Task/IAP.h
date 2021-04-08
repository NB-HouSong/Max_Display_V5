/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : IAP.h
* Author        : 李美华
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IAP_H
#define IAP_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
//bootloader地址   60k程序空间
#define MAX_IAPSW_SIZE			0xF000 		//最大固件大小（29K）0x7400

//IAP状态宏定义
#define IAP_OK					    0
#define IAPERROR_SIZE			  1 			//固件大小超范围
#define IAPERROR_ERASE			2 			//擦除Flash失败
#define IAPERROR_WRITEFLASH	3			//写入Flash失败
#define IAPERROR_UNLOCK			4			//没有处在锁车状态
#define IAPERROR_INDEX			5			//索引错误
#define IAPERROR_BUSY			  6			//总线忙
#define IAPERROR_FORM			  7 			//数据格式错误
#define IAPERROR_CRC			  8			//校验错误
#define IAPERROR_OTHER			9			//其他错误

//flash控制表宏定义
#define FCTRL_MAP_LEN		(3<<2) 			//flash控制表字节长度
#define FCTRL_DOWNLOAD		1 				//是否从临时程序区下载程序到正式程序区
#define FCTRL_TEMPSW_SIZE	2 				//临时程序大小

/* Exported types ------------------------------------------------------------*/
typedef struct _iap_state_struct
{    
 u8 m_bIAP_Busy ;
 s32 m_IAP_FileSize;
 u8 m_IAPIndex;	  		//IAP数据连续索引确认字节
 s32 m_IAP_WritePos;
 u32 m_IAP_CRC;
 u8 m_bIAP_BinHeadisOK;		//固件头部格式是否正确
 u8 m_bIAP_BinCheckCRCOK;		//固件内部校验是否正确
 u8 m_bIAP_BinDownloadFinished;		//固件下载完成
 u8 m_IAP_elected_id;
}_IAP_STATE_T;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
extern u32 g_FCtrlMap[FCTRL_MAP_LEN>>2];  		//内存控制表
extern u32 g_FCtrlMap_Default[FCTRL_MAP_LEN>>2];
extern _IAP_STATE_T iap_state;

/* Exported functions ------------------------------------------------------- */
void IAP_Init(void);
void IAP_BootloaderInit(void);
void IAP_ParseFrame(u8* pData, u8 channel);
void IAP_decrypt(u8 *pData, u8 *key);
void IAP_encrypt(u8 *pData, u8 *key);
void IAP_Reset(void);
void IAP_Busy_pro(u8 *result,u8 src_id,u8 channel);
#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

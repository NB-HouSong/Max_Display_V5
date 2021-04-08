/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : commu.h
* Author        : ¿Ó√¿ª™
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  _COMMU_H_
#define  _COMMU_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "decode.h"
#include "ParseFrame.h"
//#include "variable.h"
#include "GD25QXX.h" 

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define VOICE_NOERROR				0
#define ERROR_CHECKSUM				1
#define ERROR_LENGTH				2

/* Exported functions ------------------------------------------------------- */
void VoiceDownload(u8 Channel, u8* pData);

#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

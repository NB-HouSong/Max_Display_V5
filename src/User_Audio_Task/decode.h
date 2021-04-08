/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : decode.h
* Author        : ������
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  _DECODE_H_
#define  _DECODE_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "GD25QXX.h" 
#include "Audio_Driver.h"

/* Exported types ------------------------------------------------------------*/
typedef union
{
	uint8_t   ByteBuf[256];
	int16_t  snBuf[128];
}WAVEBUFF;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
extern volatile uint8_t g_bBuffAEmpty;
extern volatile uint8_t g_bBuffBEmpty;//�ڶ�ʱ���п���֮д��һ�λ�����
extern volatile uint8_t g_bPlayBuffAorB;
extern volatile uint8_t m_volume, m_volume_offset;
extern WAVEBUFF arrWaveBufA;
extern WAVEBUFF arrWaveBufB;
extern volatile uint32_t WavePlayAddr;
extern volatile uint32_t Voicelenth;         //�������ݳ���

/* Exported functions ------------------------------------------------------- */
void Audio_Driver_Init(void);
void Check_Read_Voice_Data(void);
void PlayVoice(uint8_t VoiceNum);
void DecodeEraseVoice(uint8_t VoiceNum);//���������β���

#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

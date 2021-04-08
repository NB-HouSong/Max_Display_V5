///******************** (C) COPYRIGHT 2020 Ninebot *******************************
//* File Name     : commu.c
//* Author        : meihua
//* Version       : V1.0.0
//* Date          : 2020-11-13
//* Description   : 
//********************************************************************************/

///* Includes ------------------------------------------------------------------*/
//#include "commu.h"

///* Private typedef -----------------------------------------------------------*/
///* Private define ------------------------------------------------------------*/
///* Private macro -------------------------------------------------------------*/
///* Variables -----------------------------------------------------------------*/
///* Private function prototypes -----------------------------------------------*/
///* Functions -----------------------------------------------------------------*/

///*****************************************************************
//* Function Name : ReturnVoiceAckPack
//* Description   : 返回数据
//* Input         : error_info:故障信息 dst_id：目标地址
//				: Channel：通讯通道
//* Output        : None
//* Notes         :
//******************************************************************/
//void ReturnVoiceAckPack(uint8_t error_info, uint8_t dst_id, u8 Channel)
//{
//    PushMiniFrame(MY_ID, dst_id, 0, CMD_SPK_ACK, error_info, NULL, Channel);    //UART1
//}

///*****************************************************************
//* Function Name : DecodeEraseVoice
//* Description   : 擦除语音段
//* Input         : VoiceNum：要擦除的语音编号
//* Output        : None
//* Notes         :
//******************************************************************/
//void DecodeEraseVoice(uint8_t VoiceNum)
//{
//	uint32_t  i  = 0;
//    uint32_t StartAddr = 1 + (VoiceNum-1)*50;  //擦除起始扇区号
//	
//    for(i = 0;i < 50;i++) //擦除语音段对应的200k
//    {
//        GD25Q_SPIFLASH_EraseSector(StartAddr + i);
//    }
//}

///*****************************************************************
//* Function Name : VoiceDownload
//* Description   : 语音下载函数
//* Input         : Channel：通道 pData：数据
//* Output        : None
//* Notes         :
//******************************************************************/
//void VoiceDownload(u8 Channel, u8* pData)
//{
//	static uint8_t current_voice_num = 0;
//	static uint32_t current_address = 0;
//	static uint8_t aryData[256] = {0};         //1页256byte
//	static uint8_t speaker_receive_index = 0;
//	static uint32_t data_length = 0;
//	uint16_t checksum = 0;
//	
//	switch(pData[FRM_CMD])
//	{
//	case CMD_SPK_ERASE:
////		GD25Q_SPIFLASH_WakeUp();  //语音下载开始，先唤醒Flash
//		current_voice_num = pData[FRM_INDEX];       //要擦除的语音编号
//		memcpy(&data_length, &pData[FRM_DATA], 4);  //擦除长度
//		//判断发送数据包长度
//		if(data_length > WAVE_FILE_LEN)  //要擦除的长度大于200k,返回长度错误
//		{
//			ReturnVoiceAckPack(ERROR_LENGTH, pData[FRM_SRC_ID], Channel);
//			return;
//		}
//		portENTER_CRITICAL();
//		DecodeEraseVoice(current_voice_num); //擦除相应编号的语音段
//		portEXIT_CRITICAL();
//		speaker_receive_index = 0;
//		current_address = _0_START_ADDR + (current_voice_num-1)*50*GD25Q32_SECTOR_BYTE_SIZE;
//		//擦除flash完成ACK
//		ReturnVoiceAckPack(VOICE_NOERROR, pData[FRM_SRC_ID], Channel);
//		break;
//		
//	case CMD_SPK_WR_DATA:
//		if(pData[FRM_INDEX] == speaker_receive_index && pData[FRM_INDEX] < 2)	//每次接收到128byte，接收两次
//		{
//			//每256检验一次			
//			speaker_receive_index += 1;
//			memcpy(&aryData[pData[FRM_INDEX]*128], &pData[FRM_DATA], 128);
//		}
//		break;
//		
//	case CMD_SPK_VERIFY:
//		memcpy((uint8_t*)&checksum, &pData[FRM_DATA], 2);
//		if(CheckSum(&aryData[0], 256) == checksum)   //校验通过，将256byte数据写入flash
//		{
//			portENTER_CRITICAL();
//			GD25Q_SPIFLASH_WritePage(&aryData[0], current_address, 256); //按页写
//			portEXIT_CRITICAL();
//			current_address += 256;
//			if(current_address >= data_length) 
//			{
//				data_length = 0;
//			}
//			ReturnVoiceAckPack(VOICE_NOERROR, pData[FRM_SRC_ID], Channel);
//		}
//		else
//		{
//			//回复错误包
//			ReturnVoiceAckPack(ERROR_CHECKSUM, pData[FRM_SRC_ID], Channel);
//		}
//		speaker_receive_index = 0;
//		break;
//	default:
//		break;
//	}
//}

////====================================================================================//

////==================================== The End===============================================/

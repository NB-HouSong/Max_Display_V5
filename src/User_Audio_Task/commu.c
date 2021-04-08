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
//* Description   : ��������
//* Input         : error_info:������Ϣ dst_id��Ŀ���ַ
//				: Channel��ͨѶͨ��
//* Output        : None
//* Notes         :
//******************************************************************/
//void ReturnVoiceAckPack(uint8_t error_info, uint8_t dst_id, u8 Channel)
//{
//    PushMiniFrame(MY_ID, dst_id, 0, CMD_SPK_ACK, error_info, NULL, Channel);    //UART1
//}

///*****************************************************************
//* Function Name : DecodeEraseVoice
//* Description   : ����������
//* Input         : VoiceNum��Ҫ�������������
//* Output        : None
//* Notes         :
//******************************************************************/
//void DecodeEraseVoice(uint8_t VoiceNum)
//{
//	uint32_t  i  = 0;
//    uint32_t StartAddr = 1 + (VoiceNum-1)*50;  //������ʼ������
//	
//    for(i = 0;i < 50;i++) //���������ζ�Ӧ��200k
//    {
//        GD25Q_SPIFLASH_EraseSector(StartAddr + i);
//    }
//}

///*****************************************************************
//* Function Name : VoiceDownload
//* Description   : �������غ���
//* Input         : Channel��ͨ�� pData������
//* Output        : None
//* Notes         :
//******************************************************************/
//void VoiceDownload(u8 Channel, u8* pData)
//{
//	static uint8_t current_voice_num = 0;
//	static uint32_t current_address = 0;
//	static uint8_t aryData[256] = {0};         //1ҳ256byte
//	static uint8_t speaker_receive_index = 0;
//	static uint32_t data_length = 0;
//	uint16_t checksum = 0;
//	
//	switch(pData[FRM_CMD])
//	{
//	case CMD_SPK_ERASE:
////		GD25Q_SPIFLASH_WakeUp();  //�������ؿ�ʼ���Ȼ���Flash
//		current_voice_num = pData[FRM_INDEX];       //Ҫ�������������
//		memcpy(&data_length, &pData[FRM_DATA], 4);  //��������
//		//�жϷ������ݰ�����
//		if(data_length > WAVE_FILE_LEN)  //Ҫ�����ĳ��ȴ���200k,���س��ȴ���
//		{
//			ReturnVoiceAckPack(ERROR_LENGTH, pData[FRM_SRC_ID], Channel);
//			return;
//		}
//		portENTER_CRITICAL();
//		DecodeEraseVoice(current_voice_num); //������Ӧ��ŵ�������
//		portEXIT_CRITICAL();
//		speaker_receive_index = 0;
//		current_address = _0_START_ADDR + (current_voice_num-1)*50*GD25Q32_SECTOR_BYTE_SIZE;
//		//����flash���ACK
//		ReturnVoiceAckPack(VOICE_NOERROR, pData[FRM_SRC_ID], Channel);
//		break;
//		
//	case CMD_SPK_WR_DATA:
//		if(pData[FRM_INDEX] == speaker_receive_index && pData[FRM_INDEX] < 2)	//ÿ�ν��յ�128byte����������
//		{
//			//ÿ256����һ��			
//			speaker_receive_index += 1;
//			memcpy(&aryData[pData[FRM_INDEX]*128], &pData[FRM_DATA], 128);
//		}
//		break;
//		
//	case CMD_SPK_VERIFY:
//		memcpy((uint8_t*)&checksum, &pData[FRM_DATA], 2);
//		if(CheckSum(&aryData[0], 256) == checksum)   //У��ͨ������256byte����д��flash
//		{
//			portENTER_CRITICAL();
//			GD25Q_SPIFLASH_WritePage(&aryData[0], current_address, 256); //��ҳд
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
//			//�ظ������
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

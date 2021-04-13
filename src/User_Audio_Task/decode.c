///******************** (C) COPYRIGHT 2020 Ninebot *******************************
//* File Name     : decode.c
//* Author        : meihua
//* Version       : V1.0.0
//* Date          : 2020-11-13
//* Description   : 
//********************************************************************************/

///* Includes ------------------------------------------------------------------*/
//#include "decode.h"

///* Private typedef -----------------------------------------------------------*/
///* Private define ------------------------------------------------------------*/
///* Private macro -------------------------------------------------------------*/
///* Variables -----------------------------------------------------------------*/
//volatile uint8_t DownloadFlag = 1;
//volatile uint32_t VoiceStarAddr = 0;       //��¼���������ε���ʼ��ַ
//volatile uint32_t Voicelenth;              //�������ݳ���
//volatile uint8_t g_bBuffAEmpty = 1;        //��������A��������
//volatile uint8_t g_bBuffBEmpty = 1;        //��������B��������
//volatile uint8_t g_bPlayBuffAorB = PLAY_A; //���ŵĻ�����
//volatile uint8_t m_volume, m_volume_offset;

//WAVEBUFF arrWaveBufA;
//WAVEBUFF arrWaveBufB;
//volatile uint32_t WavePlayAddr = 0;  //�������ݵ���ʼ��ַ

///* Private function prototypes -----------------------------------------------*/
///* Functions -----------------------------------------------------------------*/

///*****************************************************************
//* Function Name : Check_Read_Voice_Data
//* Description   : ���������� 
//* Input         : None
//* Output        : None
//* Notes         :
//******************************************************************/
//void Check_Read_Voice_Data(void)
//{
//    static u16 cnt_256;         //��������������
//	static u16 read_times = 0;  //����256byte�Ĵ���
//	static u8 ramander = 0;     //�������ݲ���256byte���ֽ���
//	static u16 read_lenth = 0;  //���ζ�ȡ�������ݵĳ���
//	static u8* wave_save_p = 0; //���ζ����������ݴ�ŵĵ�ַ
//	
//	//���ȴ�����BuffΪ��
//	if(!DownloadFlag && (g_bBuffAEmpty == 1 || g_bBuffBEmpty == 1))
//	{
//		read_times = Voicelenth/256;
//		ramander = Voicelenth%256;
//		
//		if(cnt_256 < read_times)
//		{
//			cnt_256 ++;
//			read_lenth = 256;
//			WavePlayAddr += 256;
//		}
//		else
//		{
//			cnt_256 = 0;
//			read_lenth = ramander;
//			DownloadFlag = 1;
//		}
//		
//		if(read_lenth > 0)
//		{
//			if(g_bBuffAEmpty == 1)
//			{
//				g_bBuffAEmpty = 0;
//				wave_save_p = arrWaveBufA.ByteBuf;
//			}
//			else
//			{
//				g_bBuffBEmpty = 0;
//				wave_save_p = arrWaveBufB.ByteBuf;
//			}
//			//GD25Q_SPIFLASH_ReadBuffer(wave_save_p, WavePlayAddr, read_lenth);
//		}
//	}
//}

///*****************************************************************
//* Function Name : PlayVoice
//* Description   : ���Ŷ�Ӧ��ŵ����� 
//* Input         : VoiceNum:���ŵ��������
//* Output        : None
//* Notes         :
//******************************************************************/
//void PlayVoice(uint8_t VoiceNum)
//{
////	u8 i = 0;
////	//�������Ŵ򿪣��Ҹö�����δ������
////	if(g_myself_data.NFC_Audio_set.AudioEnableConfig.Audio_ON \
////		&& (g_myself_data.NFC_Audio_set.AudioEnableConfig.Audio_Enable) & (0x01<< (VoiceNum-1)))
////	{
////		if(g_bool[B_VOICE_ING])
////			return;
////		
//////		GD25Q_SPIFLASH_WakeUp();  //����Flash
////		
////		m_volume = Level_Matching(g_myself_data.Audio_Set.VolumeLevel);  //����
////		
////		m_volume_offset = 2048 - 2048 * m_volume / 100;
////		g_bool[B_VOICE_ING] = 1;

////		VoiceStarAddr = _0_START_ADDR + (VoiceNum-1)*50*GD25Q32_SECTOR_BYTE_SIZE; 
////		Voicelenth = 0;
////		for (i = 0;i < 4;i++)
////		{
////			Voicelenth += (u32)GD25Q_SPIFLASH_GetByte(VoiceStarAddr+i+40) << i*8;       //�����������
////		}	
////		WavePlayAddr = VoiceStarAddr + 44;  //�������ŵ�ַ
////		
////		if(Voicelenth> 0x32000) //�������ȳ���Χ
////		{
////			g_bool[B_VOICE_ING] = 0;
////			return;
////		}
////		
////		GF_ENABLE();  //ʹ�ܹ���
////		
////		DownloadFlag = 0; //������
////        
////		TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);//����ʱ���ж�
////		TIM_Cmd(TIM6, ENABLE);                  //������ʱ��
////	}
//}

////====================================================================================//

////==================================== The End===============================================/

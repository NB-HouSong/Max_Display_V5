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
//volatile uint32_t VoiceStarAddr = 0;       //记录播放语音段的起始地址
//volatile uint32_t Voicelenth;              //语音数据长度
//volatile uint8_t g_bBuffAEmpty = 1;        //语音数据A缓冲区空
//volatile uint8_t g_bBuffBEmpty = 1;        //语音数据B缓冲区空
//volatile uint8_t g_bPlayBuffAorB = PLAY_A; //播放的缓冲区
//volatile uint8_t m_volume, m_volume_offset;

//WAVEBUFF arrWaveBufA;
//WAVEBUFF arrWaveBufB;
//volatile uint32_t WavePlayAddr = 0;  //语音数据的起始地址

///* Private function prototypes -----------------------------------------------*/
///* Functions -----------------------------------------------------------------*/

///*****************************************************************
//* Function Name : Check_Read_Voice_Data
//* Description   : 读语音数据 
//* Input         : None
//* Output        : None
//* Notes         :
//******************************************************************/
//void Check_Read_Voice_Data(void)
//{
//    static u16 cnt_256;         //读语音次数计数
//	static u16 read_times = 0;  //读整256byte的次数
//	static u8 ramander = 0;     //语音数据不满256byte的字节数
//	static u16 read_lenth = 0;  //本次读取语音数据的长度
//	static u8* wave_save_p = 0; //本次读出语音数据存放的地址
//	
//	//喇叭打开且有Buff为空
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
//* Description   : 播放对应编号的语音 
//* Input         : VoiceNum:播放的语音编号
//* Output        : None
//* Notes         :
//******************************************************************/
//void PlayVoice(uint8_t VoiceNum)
//{
////	u8 i = 0;
////	//语音播放打开，且该段语音未被禁播
////	if(g_myself_data.NFC_Audio_set.AudioEnableConfig.Audio_ON \
////		&& (g_myself_data.NFC_Audio_set.AudioEnableConfig.Audio_Enable) & (0x01<< (VoiceNum-1)))
////	{
////		if(g_bool[B_VOICE_ING])
////			return;
////		
//////		GD25Q_SPIFLASH_WakeUp();  //唤醒Flash
////		
////		m_volume = Level_Matching(g_myself_data.Audio_Set.VolumeLevel);  //音量
////		
////		m_volume_offset = 2048 - 2048 * m_volume / 100;
////		g_bool[B_VOICE_ING] = 1;

////		VoiceStarAddr = _0_START_ADDR + (VoiceNum-1)*50*GD25Q32_SECTOR_BYTE_SIZE; 
////		Voicelenth = 0;
////		for (i = 0;i < 4;i++)
////		{
////			Voicelenth += (u32)GD25Q_SPIFLASH_GetByte(VoiceStarAddr+i+40) << i*8;       //获得语音长度
////		}	
////		WavePlayAddr = VoiceStarAddr + 44;  //语音播放地址
////		
////		if(Voicelenth> 0x32000) //语音长度超范围
////		{
////			g_bool[B_VOICE_ING] = 0;
////			return;
////		}
////		
////		GF_ENABLE();  //使能功放
////		
////		DownloadFlag = 0; //打开喇叭
////        
////		TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);//开定时器中断
////		TIM_Cmd(TIM6, ENABLE);                  //启动定时器
////	}
//}

////====================================================================================//

////==================================== The End===============================================/

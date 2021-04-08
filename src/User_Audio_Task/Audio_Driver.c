/******************** (C) COPYRIGHT 2020 Ninebot *******************************
* File Name     : Audio_Driver.c
* Author        : meihua
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "Audio_Driver.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
 #define AUDIO_DMA_Channel   DMA1_Channel3 //DAC DMA
 
 /* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
 volatile  uint16_t bDataIndex;
 static __IO uint16_t TIM6ARRValue = 2999;//5999;//2999;
 
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/*****************************************************************
* Function Name : Decode_Dac_Init
* Description   : DAC��ʼ�� 
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
 void Decode_Dac_Init(void)
{
	DAC_InitTypeDef DAC_InitStructure;
//	DMA_InitTypeDef DMA_InitStructure;
//	//DMA����
//  DMA_DeInit(AUDIO_DMA_Channel);
//	DMA_StructInit(&DMA_InitStructure);
//	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R2_ADDRESS; //�����ַ
//	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)DMAbuff; 
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
//	DMA_InitStructure.DMA_BufferSize = AUDIO_BUFFER_LEN;
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//	DMA_Init(AUDIO_DMA_Channel, &DMA_InitStructure);   //config DMA of DAC
  /* DAC ͨ��1���� */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;

  /* DAC ͨ��1��ʼ�� */
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  /* ʹ��DAC ͨ��1:��DACͨ��1��ʹ��,PA.04�Զ���DACת��������. */
  DAC_Cmd(DAC_Channel_1, ENABLE);   
}

/*****************************************************************
* Function Name : Timer6Init
* Description   : DAC��ʱ����ʼ�� 
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void Timer6Init(void)      //Timer6,���Ʋ�����
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	TIM_TimeBaseStructure.TIM_Period = TIM6ARRValue;//16kHz
	TIM_TimeBaseStructure.TIM_Prescaler =0;         //��Ƶϵ��1 48MHz 
	TIM_TimeBaseStructure.TIM_ClockDivision =0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
    TIM_SetAutoreload(TIM6, TIM6ARRValue);

	TIM_ClearFlag(TIM6, TIM_IT_Update);//���־
    /* TIM6�������ģʽѡ��  */
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
}

/*****************************************************************
* Function Name : Audio_Driver_Init
* Description   : DAC������ų�ʼ�� 
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void Audio_Driver_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
	//���ŵ�ʹ������ BUSY
	GPIO_InitStructure.GPIO_Pin = GF_EN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GF_EN_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(GF_EN_GPIO,GF_EN_PIN);
	
	//DAC�����
	GPIO_InitStructure.GPIO_Pin =  DAC_OUT_PIN;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(DAC_OUT_GPIO, &GPIO_InitStructure); 
	
	Decode_Dac_Init();
    Timer6Init();
}

/*****************************************************************
* Function Name : DecodeTimer_Int
* Description   : DAC���ݷ���
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void DecodeTimer_Int(void)
{
	static s16 s_playdata = 0;
	static u32 s_playdata_cnt = 0;

	if((g_bPlayBuffAorB == PLAY_A && g_bBuffAEmpty == 0) || (g_bPlayBuffAorB == PLAY_B && g_bBuffBEmpty == 0))
	{
		if(g_bPlayBuffAorB == PLAY_A && g_bBuffAEmpty == 0)
		{
			s_playdata = (uint16_t)arrWaveBufA.snBuf[bDataIndex]; //s16תu16   s16��-32768~32767
		}
		else if(g_bPlayBuffAorB == PLAY_B && g_bBuffBEmpty == 0)
		{
			s_playdata = (uint16_t)arrWaveBufB.snBuf[bDataIndex];
		}
		
		s_playdata = s_playdata * m_volume / 100 + 32768;
		DAC_SetChannel1Data(DAC_Align_12b_R, (s_playdata >> 4) + m_volume_offset);
		DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);
		
		s_playdata_cnt++;
		bDataIndex = bDataIndex + 1;
		
		//һҳ���ݲ�����ɻ����������ݲ������ ��buf��
		if(bDataIndex >= 128 || s_playdata_cnt>= Voicelenth/2)  
		{
			bDataIndex = 0;
			if(g_bPlayBuffAorB == PLAY_A)
			{
				g_bPlayBuffAorB = PLAY_B;
//            DAC_DMA_RefreshA();
				g_bBuffAEmpty = 1;
			}
			else if(g_bPlayBuffAorB == PLAY_B)
			{
				g_bPlayBuffAorB = PLAY_A;
//            DAC_DMA_RefreshB();
				g_bBuffBEmpty = 1;
			}
		}
		
		if(s_playdata_cnt >= Voicelenth/2) //������ɣ��ض�ʱ��
		{
			s_playdata_cnt = 0;
			bDataIndex = 0;
			g_bPlayBuffAorB = PLAY_A;
			g_bBuffAEmpty = 1;
			g_bBuffBEmpty = 1;
			//ֹͣ������Ҫ�Ѷ�ʱ��ͣ��
			DAC_SetChannel1Data(DAC_Align_12b_R, 2048);
			DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);
			TIM_Cmd(TIM6, DISABLE);
			GF_DISABLE();
			g_bool[B_VOICE_ING] = 0;
//			GD25Q_SPIFLASH_PowerDown();  //������������flash�������˯��
		}
	}
}

/*****************************************************************
* Function Name : TIM6_DAC_IRQHandler
* Description   : TIM6_DAC�жϺ���
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void TIM6_DAC_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    DecodeTimer_Int();
  }
}

//====================================================================================//

//==================================== The End===============================================/

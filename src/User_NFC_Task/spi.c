/******************** (C) COPYRIGHT 2020 Ninebot *******************************
* File Name     : main.c
* Author        : meihua
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "spi.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SPI_TIMEOUTCNT  100

/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/*****************************************************************
* Function Name : SPI1_Configuration
* Description   : SPI��س�ʼ�� 
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void SPI1_Configuration(void)  //NFC
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_PinAFConfig(SPI_GPIO_PORT, GPIO_PinSource3, GPIO_AF_0);
	GPIO_PinAFConfig(SPI_GPIO_PORT, GPIO_PinSource4, GPIO_AF_0);
	GPIO_PinAFConfig(SPI_GPIO_PORT, GPIO_PinSource5, GPIO_AF_0);

    //SPI���GPIO��ʼ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  
	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = SPI_PIN_CLK;
	GPIO_Init(SPI_GPIO_PORT, &GPIO_InitStructure);

	/* SPI  MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPI_PIN_MOSI;
	GPIO_Init(SPI_GPIO_PORT, &GPIO_InitStructure);

	/* SPI MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin = SPI_PIN_MISO;
	GPIO_Init(SPI_GPIO_PORT, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = PIN_NSS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(PORT_NSS, &GPIO_InitStructure);
	GPIO_SetBits(PORT_NSS,PIN_NSS);

    //////SPIģ������//////
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
  
	SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
	SPI_Cmd(SPI1, ENABLE);
}

/*****************************************************************
* Function Name : SPI_ReadWriteByte
* Description   : SPI��дһ���ֽڣ�������ɺ󷵻ر���ͨѶ��ȡ�����ݣ�
* Input         : TxData �����͵��� RxData �յ�����
* Output        : None
* Notes         :
******************************************************************/
u8 SPI_ReadWriteByte(u8 TxData, u8* pRxData, SPI_TypeDef* SPIx)
{
	 u16 timeoutcnt = 0;

	 timeoutcnt = SPI_TIMEOUTCNT;
	 
	 while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET && timeoutcnt)
	 {
		 timeoutcnt--;	 
	 }
	 if(!timeoutcnt)
		return SPI_TIMEOUT;// */
	 //?h??? 
	 SPI_SendData8(SPIx, TxData);
	 timeoutcnt = SPI_TIMEOUTCNT;
	 while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET && timeoutcnt)
	 {
		 timeoutcnt--;
	 }
	 if(!timeoutcnt)
		return SPI_TIMEOUT;
	 //SPI_I2S_ClearFlag(SPI2,SPI_I2S_FLAG_RXNE);
	 timeoutcnt = SPI_ReceiveData8(SPIx);
	 if(pRxData != 0)
	 {
		 *pRxData = timeoutcnt;
	 }
	 return SPI_SUCCESS;
}

/*****************************************************************
* Function Name : EXTI_Config
* Description   : �ⲿ�ж����ú���
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void EXTI_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef  EXTI_InitStructure;

    GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	//NFCˢ���¼�
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD,EXTI_PinSource2);
    EXTI_InitStructure.EXTI_Line = EXTI_Line2; 					    //���ӵ��ⲿ�ж�2
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 			//ʹ���ж�ģʽ
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		    //��ѹ�½�������ֵʱ�����ж�
    EXTI_InitStructure.EXTI_LineCmd = ENABLE; 						//ʹ���ж�
    EXTI_Init(&EXTI_InitStructure);
	
//	//����������¼�
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource9);
//    EXTI_InitStructure.EXTI_Line = EXTI_Line9; 					    //���ӵ��ⲿ�ж�9
//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 			//ʹ���ж�ģʽ
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		    //��ѹ�½�������ֵʱ�����ж�
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE; 						//ʹ���ж�
//    EXTI_Init(&EXTI_InitStructure);
	
	//CANͨѶ�����¼�
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource8);
//    EXTI_InitStructure.EXTI_Line = EXTI_Line8; 					//���ӵ��ⲿ�ж�8
//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 			//ʹ���ж�ģʽ
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		//��ѹ�½�������ֵʱ�����ж�
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE; 				    //ʹ���ж�
//    EXTI_Init(&EXTI_InitStructure);
}

/*****************************************************************
* Function Name : EXTI2_3_IRQHandler
* Description   : �ⲿ�жϺ���
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void EXTI2_3_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
        if(g_bool[B_LOWPOWER_MODE])
        {
			g_bool[B_SLEEP_MODE] = 0; //
            g_bool[B_LOWPOWER_MODE]=0;//�˳��͹���ģʽ
			
			//��ʼ��ʱ��
			SystemCoreClockConfigure();
			SystemCoreClockUpdate();
//			SystemInit();
			//��ʼ������
			InitAllPeripherals();
        }
		NFC_INT=1; //ˢ��,��λ
    }
    EXTI_ClearITPendingBit(EXTI_Line2);
}

/*****************************************************************
* Function Name : EXTI4_15_IRQHandler
* Description   : �ⲿ�жϺ���
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
//void EXTI4_15_IRQHandler(void)
//{
//	if(EXTI_GetITStatus(EXTI_Line9) != RESET)
//    {
//		PlayVoice(VOICE_BATIN); //�����������ʾ��
//        if(g_bool[B_LOWPOWER_MODE])
//        {
//			if(g_bool[B_SLEEP_MODE])//����ģʽ�������˳��͹��ģ�����״̬��ֻ�Ǳ��˳��͹��ģ����ָ�CANͨ�ţ�
//			{
//			  g_bool[B_SLEEP_MODE] = 0; 
//			  g_bool[B_LOWPOWER_MODE] = 0;
//			}
//			//��ʼ��ʱ��
//			SystemCoreClockConfigure();
//			SystemCoreClockUpdate();
//			InitAllPeripherals();
//        }
//		EXTI_ClearITPendingBit(EXTI_Line9);
//    }
//    
//	//CANͨѶ���ѵ͹���
//	if(EXTI_GetITStatus(EXTI_Line8) != RESET)
//	{
//		if(g_bool[B_LOWPOWER_MODE] && MC_Running_data.MC_Status.LockStatus == LOCKSTATE_LOCK)
//		{
//			g_bool[B_SLEEP_MODE] = 0; 
//		    g_bool[B_LOWPOWER_MODE] = 0;
//			//��ʼ��ʱ��
//			SystemCoreClockConfigure();
//			SystemCoreClockUpdate();
//			InitAllPeripherals();
//		}
//		EXTI_ClearITPendingBit(EXTI_Line8);
//	}
//}

//====================================================================================//

//==================================== The End===============================================/

/******************** (C) COPYRIGHT 2020 Ninebot *******************************
* File Name     : tm1650.c
* Author        : meihua
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "tm1650.h"

/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
//u8 TM1652_send_flage = 0;

/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/*****************************************************************
* Function Name : TM1652_Init
* Description   : tm1652�������ų�ʼ�� 
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
//void TM1652_Init(void)
//{
//  GPIO_InitTypeDef  GPIO_InitStructure;
//  
//  GPIO_InitStructure.GPIO_Pin = TM1652_SDA_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
//  GPIO_Init(TM1652_SDA_GPIO, &GPIO_InitStructure);					
//  GPIO_SetBits(TM1652_SDA_GPIO,TM1652_SDA_PIN);	
//}

/*****************************************************************
* Function Name : tm1652_send_data
* Description   : TM1652�������� 
* Input         : data��Ҫ��ʾ������ addr��Ҫ��ʾ���ֵ������
* Output        : None
* Notes         :
******************************************************************/
//void tm1652_send_data(u8 sdat, u8* sendstep)
//{
//	if((TM1652_UART->ISR & USART_FLAG_TC) != (uint16_t)RESET) 
//	{
//		TM1652_UART->TDR = sdat;
//		(*sendstep)++;
//	}
//}
//void tm1652_send_data(u8 sdat)
//{
//	unsigned char i=0, sfalg=0;
////	if(TM1652_send_flage == 1)
////		return;
////	TM1652_send_flage = 1;
//	//��ʼλ	
//	portENTER_CRITICAL();
//	TX_LOW();   
//	delay_us(50);    //����52us
//	
//   //����8λ����
//	for(i=0; i<8; i++)
//	{
//		if(sdat & 0x01)
//		{
//			TX_HIGH();
//			sfalg++;
//		}
//		else
//		{
//			TX_LOW();
//		}
//		delay_us(50); 
//		sdat >>=1;
//	}

//	//У��λ,���շ���������1�ĸ������ж�
//	if(sfalg%2==0)			         
//	{
//		TX_HIGH();
//	}
//	else			        
//	{
//		TX_LOW();
//	}
//	delay_us(50); 
//	
//	//ֹͣλ
//	TX_HIGH();  
//	delay_us(50);  
////	TM1652_send_flage = 0;
//	portEXIT_CRITICAL();
//}


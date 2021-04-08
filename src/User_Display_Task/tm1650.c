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
* Description   : tm1652控制引脚初始化 
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
* Description   : TM1652驱动函数 
* Input         : data：要显示的数字 addr：要显示数字的数码管
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
//	//起始位	
//	portENTER_CRITICAL();
//	TX_LOW();   
//	delay_us(50);    //保持52us
//	
//   //发送8位数据
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

//	//校验位,按照发送数据中1的个数来判断
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
//	//停止位
//	TX_HIGH();  
//	delay_us(50);  
////	TM1652_send_flage = 0;
//	portEXIT_CRITICAL();
//}


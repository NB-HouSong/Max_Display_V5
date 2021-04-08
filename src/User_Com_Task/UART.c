/******************** (C) COPYRIGHT 2020 Ninebot *******************************
* File Name     : UART.c
* Author        : meihua
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "UART.h"

/* Private define ------------------------------------------------------------*/
#define EXUART_BAUD_RADE	    19200

#define EUART_TX_BUF_SIZE		9	   //发送队列每帧缓存大小

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
u8 EUART_Tx_Buf[EUART_TX_BUF_SIZE]; //数据发送队列
volatile s8 m_EUART_Tx_Tail = 0;	//数据发送帧队列尾指针
volatile s8 m_EUART_Tx_Head = 0;	//数据发送帧队列头指针
volatile u8 g_EUART_Tx_Finish = 1;	//数据发送完成

/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/*****************************************************************
* Function Name : UART_Int
* Description   : UART初始化
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void UART_Int(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;
	
	//端口重映射
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1); 	//TX

	//初始化管脚
	GPIO_InitStructure.GPIO_Pin = EU1_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(EU1_TX_GPIO, &GPIO_InitStructure);
	
	USART_DeInit(TM1652_UART);
	USART_InitStructure.USART_BaudRate = EXUART_BAUD_RADE;       //波特率19200，每位52us
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;  //9位数据位（1位起始位，8位数据位）
	USART_InitStructure.USART_StopBits = USART_StopBits_1;       //1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_Odd;         //TM1652偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;

	USART_ClockInit(TM1652_UART, &USART_ClockInitStructure);
	USART_Init(TM1652_UART, &USART_InitStructure);
	
//    USART_ITConfig(TM1652_UART, USART_IT_TC, ENABLE);	//FreeRTOS任务创建完成后再开中断	
	
	USART_Cmd(TM1652_UART, ENABLE);
}

/*****************************************************************
* Function Name : UART_PushFrame
* Description   : 将一帧数据压入发送队列
* Input         : DataLen：数据长度 pdata：数据 
* Output        : None
* Notes         : 每次压入9byte数据，1byte显示地址，6byte显示数据，
                : 1byte显示控制命令，1byte显示控制调节命令
******************************************************************/	
void UART_PushFrame(u8 DataLen, u8*pdata)
{
	if(m_EUART_Tx_Tail+DataLen <= EUART_TX_BUF_SIZE) //一帧数据发送完成允许压入下一帧
	{
		memcpy(&EUART_Tx_Buf[m_EUART_Tx_Tail], pdata , DataLen);
		m_EUART_Tx_Tail += DataLen;
	}
}

/*****************************************************************
* Function Name : UART_CheckSend
* Description   : 查询方式发送数据
* Input         : None
* Output        : None
* Notes         : 发送完显示地址和显示数据（共7byte）之后，停留3ms
                : 以上再发送显示控制指令和显示控制调节命令（2byte）
******************************************************************/	
void UART_CheckSend(void)
{
	static u8 s_cnt = 0;
	//发送完显示数据和显示控制调节命令后均停留3ms以上，否则显示可能不稳定
	if(g_EUART_Tx_Finish > 0 && s_cnt <= 4) 
	{
		s_cnt++;
		return;
	}
	
	//发送显示地址和显示控制命令
	if(m_EUART_Tx_Tail > m_EUART_Tx_Head && (m_EUART_Tx_Head == 0 || m_EUART_Tx_Head == 7))
	{
		s_cnt = 0;
		g_EUART_Tx_Finish = 0;
		TM1652_UART->TDR = EUART_Tx_Buf[m_EUART_Tx_Head];
		m_EUART_Tx_Head++;
	}
}

/*****************************************************************
* Function Name : USART1_IRQHandler
* Description   : 串口1中断函数
* Input         : None
* Output        : None
* Notes         : 进入发送完成中断后再发送下一字节数据
******************************************************************/	
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(TM1652_UART, USART_IT_TC) != RESET) //发送中断
	{ 
		USART_ClearITPendingBit(TM1652_UART, USART_IT_TC); 
		if(m_EUART_Tx_Tail > m_EUART_Tx_Head && g_EUART_Tx_Finish == 0)
		{
			TM1652_UART->TDR = EUART_Tx_Buf[m_EUART_Tx_Head];
			m_EUART_Tx_Head++;
			if(m_EUART_Tx_Head == EUART_TX_BUF_SIZE) //发送完控制命令
			{
				m_EUART_Tx_Tail = 0;
				m_EUART_Tx_Head = 0;
				g_EUART_Tx_Finish = 1;
			}
			else if(m_EUART_Tx_Head == 7)  //发送完显示数据
			{
				g_EUART_Tx_Finish = 1;
			}
		}
	}
}
	
//====================================================================================//

//==================================== The End===============================================/

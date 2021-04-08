#include "stm32f0xx.h"
#include "UART_2.h"
#include "ParseFrame.h"
#include "main.h"

#include "LED_Others.h"



#define BT_UART								USART2		//  IOT
//#define EXUART2_USE_TXDMA				//是否使用DMA发送
#define EXUART2_BAUD_RADE				115200

#define EUART2_DMA						DMA1
#define EUART2_RX_DMA_CH	 		DMA1_Channel5
//#define EUART2_TX_DMA_CH	 				DMA1_Channel4
//#define EUART2_TX_DMA_IT_TC			DMA1_IT_TC4

#define EU2_TX_PIN						GPIO_Pin_2
#define EU2_RX_PIN						GPIO_Pin_3
#define EU2_TX_GPIO						GPIOA
#define EU2_RX_GPIO						GPIOA



#define EUART2_RX_BUF_SIZE 			150			//接收缓存大小

#define EUART2_TX_FRM_SIZE				4				//发送队列中的最大帧数
#define EUART2_TX_FRMBUF_SIZE		150			//发送队列每帧缓存大小

typedef struct _EUART2_Frame {				//数据发送帧队列结构体
	s32 len;													//帧长度
	u8 buf[EUART2_TX_FRMBUF_SIZE];			//帧数据
} EUART2_Frame;

//第一串口相关
//数据发送队列变量
EUART2_Frame m_EUART2_TxFrames[EUART2_TX_FRM_SIZE];	//数据发送帧队列	
volatile s8 m_EUART2_TxFrm_Tail = 0;					//数据发送帧队列尾指针
volatile s8 m_EUART2_TxFrm_Head = 0;					//数据发送帧队列头指针
volatile s8 m_EUART2_TxFrm_FreeFrmLen = 0;		//数据发送帧队列剩余帧数
//DMA数据接收缓存
u8 m_EUART2_DMA_RXBuf[EUART2_RX_BUF_SIZE]; 	//DMA数据接收缓存
volatile s32 m_EUART2_DMA_RXPtr = 0;			//当前数据地址
//标志变量
volatile u8 m_bEUART2PushingFrms = 0;		//正在往发送队列存数据
volatile u8 m_bEUART2CheckingRec = 0;		//正在确认数据接收
volatile u8 m_bEUART2CheckingSend = 0;		//正在确认数据发送
volatile u8 m_bEUART2TxEn = 0;						//使能发送

void UART2_InitVariables(void)
{	
	m_EUART2_TxFrm_FreeFrmLen = EUART2_TX_FRM_SIZE-1;
}

//UART2 DMA的配置
void UART2_DMAConfiguration(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	//UART2的DMA数据接收初始化
	DMA_DeInit(EUART2_RX_DMA_CH);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)( &(BT_UART->RDR));			//外设数据寄存器
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)m_EUART2_DMA_RXBuf;				//数据Buf
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//外设作源头
	DMA_InitStructure.DMA_BufferSize = EUART2_RX_BUF_SIZE;						//Buf大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//外设地址不增加
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//内存地址增加
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//字节
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;				//字节（注意与上一个变量名称不同！！）
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								//循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						//优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//非内存到内存
	DMA_Init(EUART2_RX_DMA_CH, &DMA_InitStructure);
	USART_DMACmd(BT_UART, USART_DMAReq_Rx, ENABLE);							
	DMA_Cmd(EUART2_RX_DMA_CH, ENABLE);			
	
#ifdef EXUART2_USE_TXDMA
	//UART2的DMA数据发送初始化
	DMA_DeInit(EUART2_TX_DMA_CH);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(BT_UART->DR));			//外设数据寄存器
	DMA_InitStructure.DMA_MemoryBaseAddr = 0;									//数据Buf
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;							//外设作目标
	DMA_InitStructure.DMA_BufferSize = 0;										//Buf大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//外设地址寄存器不递增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//内存地址递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//外设字节为单位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;				//内存字节为单位（注意与上一个变量名称不同！！）
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;								//普通模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;							//优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//非内存到内存
	DMA_Init(EUART2_TX_DMA_CH, &DMA_InitStructure);

//	DMA_ITConfig(EUART2_TX_DMA_CH, DMA_IT_TC, ENABLE);							//DMA传输完成中断
	USART_DMACmd(BT_UART,USART_DMAReq_Rx,ENABLE);								//外设使能DMA  // */
#endif					
}

#ifdef SMT_TEST_MODE
//UART2初始化
void UART2_TEST_Int(void)
{
//	s16 temp16;
	GPIO_InitTypeDef GPIO_InitStructure;
	//初始化管脚
  GPIO_InitStructure.GPIO_Pin = EU2_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EU2_TX_GPIO, &GPIO_InitStructure);		
}
#endif	

//UART2初始化
void UART2_Int(void)
{
//	s16 temp16;
	GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;

	UART2_InitVariables();		//初始化相关变量
	

	//端口重映射
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1); 	//TX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);  //RX

	//初始化管脚	
	GPIO_InitStructure.GPIO_Pin = EU2_TX_PIN | EU2_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EU2_TX_GPIO, &GPIO_InitStructure);  
	
	USART_DeInit(BT_UART);
	USART_InitStructure.USART_BaudRate = EXUART2_BAUD_RADE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;

	USART_ClockInit(BT_UART, &USART_ClockInitStructure);
	USART_Init(BT_UART, &USART_InitStructure);

	// Enable the USARTx 
	USART_Cmd(BT_UART, ENABLE);
	//DMA配置（须放到UART2初始化之后）
	UART2_DMAConfiguration();
}

void Recover_Tx_AF(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = EU2_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//Tx初始时悬空输入
    GPIO_Init(EU2_TX_GPIO, &GPIO_InitStructure);
}


void Wake_Up_Iot(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = EU2_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(EU2_TX_GPIO, &GPIO_InitStructure);
    GPIO_ResetBits(EU2_TX_GPIO,EU2_TX_PIN); //20190924新增
}

//将一帧数据压入发送队列
void UART2_PushFrame(u8 src_id, u8 dst_id, u8 DataLen, u8 Cmd, u8 Index, u8*pdata)
{
//	u8 temp8 = 0;
	if(m_bEUART2PushingFrms || m_bEUART2CheckingSend)
		return;
	m_bEUART2PushingFrms = 1;
	
	if(DataLen > EUART2_TX_FRMBUF_SIZE-8 || m_EUART2_TxFrm_FreeFrmLen <= 0)		//帧长度超范围或缓存已满
	{
		m_bEUART2PushingFrms = 0;
		return;
	}

	//将要发送的数据帧压入队列
	m_EUART2_TxFrames[m_EUART2_TxFrm_Head].len = FormatFrame(src_id, dst_id, DataLen, Cmd, Index, pdata, m_EUART2_TxFrames[m_EUART2_TxFrm_Head].buf);
	m_EUART2_TxFrm_Head++;
	if(m_EUART2_TxFrm_Head == EUART2_TX_FRM_SIZE)
		m_EUART2_TxFrm_Head = 0;
	m_EUART2_TxFrm_FreeFrmLen--;
	m_bEUART2PushingFrms = 0;
}

//将最新一帧数据Pop出队列
void UART2_PopFrame(void)
{
	if(m_bEUART2PushingFrms || m_bEUART2CheckingSend)
		return;
	if(m_EUART2_TxFrm_FreeFrmLen >= EUART2_TX_FRM_SIZE-1)
		return;
	m_EUART2_TxFrm_Head--;
	if(m_EUART2_TxFrm_Head < 0)
		m_EUART2_TxFrm_Head = EUART2_TX_FRM_SIZE-1;
	m_EUART2_TxFrm_FreeFrmLen++;
}

//解析单字节数据
void UART2_ParseData(u8 data)
{
	static unsigned char s_RXBuf[EUART2_RX_BUF_SIZE] = {0};
	static unsigned char s_bFrameHead1 = 0;
	static unsigned char s_bBeginFrame = 0;
	static unsigned char s_rxPos = 0;
	static unsigned char s_rxLen = 0;
	static unsigned short s_csum = 0;
	unsigned short temp16 = 0;
	
	if(s_bBeginFrame)   //是否开始新的一帧
	{
		s_RXBuf[s_rxPos] = data;
		if(s_rxPos == 0)
		{
			s_rxLen = data+7;
			if(s_rxLen > EUART2_RX_BUF_SIZE-7)
			{
				s_bFrameHead1 = s_bBeginFrame = 0;
				s_csum = 0;
				return;
			}
		}
		s_rxPos++;
		if(s_rxPos == s_rxLen)         //一帧数据传输完毕
		{ 
			s_csum -= s_RXBuf[s_rxPos-2];	//校验和多加了一个字节
			s_csum = ~s_csum;
			temp16 = s_RXBuf[s_rxPos-2] + (s_RXBuf[s_rxPos-1]<<8);
			if(s_csum == temp16)
				ParseFrame(COM_IOT, s_RXBuf);  //解析帧数据
			s_bFrameHead1 = s_bBeginFrame = 0;
			s_rxPos = 0;
			s_csum = 0;
		}
		else
		{
			s_csum += data;
		}
	}
	else if(data == 0x5A && !s_bFrameHead1)
	{
		s_bFrameHead1 = 1;
	}
	else if(data == 0xA5 && s_bFrameHead1 && !s_bBeginFrame)
	{
		s_bBeginFrame = 1;
		s_csum = 0;
	}
	else
	{
		s_bFrameHead1 = s_bBeginFrame = 0;
		s_csum = 0;
	}
}

void UART2_CheckReceive(void)
{
	s32 DAMCnt = 0;
	s32 MaxDataLen = EUART2_RX_BUF_SIZE;	
	
	//如果正在往发送队列中添加数据，退出
	if(m_bEUART2PushingFrms)
		return;
	//判断是否正在Check
	if(m_bEUART2CheckingRec)
		return;
	m_bEUART2CheckingRec = 1;
	if(EUART2_RX_DMA_CH->CNDTR == 0)
	{
		m_bEUART2CheckingRec = 0;
		return;
	}
	DAMCnt = EUART2_RX_BUF_SIZE - (EUART2_RX_DMA_CH->CNDTR);
	while( m_EUART2_DMA_RXPtr != DAMCnt && MaxDataLen > 0)
	{
		UART2_ParseData(m_EUART2_DMA_RXBuf[m_EUART2_DMA_RXPtr]);
		m_EUART2_DMA_RXPtr++;
		if( m_EUART2_DMA_RXPtr >= EUART2_RX_BUF_SIZE )
		{
			m_EUART2_DMA_RXPtr = 0;
		}
		DAMCnt = EUART2_RX_BUF_SIZE - (EUART2_RX_DMA_CH->CNDTR);
		MaxDataLen--;
	}
	m_bEUART2CheckingRec = 0;
}

#ifndef EXUART2_USE_TXDMA							
//查询方式发送数据
void UART2_CheckSend(void)
{
	static s32 s_count = 0;
//	u32 temp32 = 0;

	if(m_bEUART2PushingFrms || m_bEUART2CheckingSend)
		return;
	m_bEUART2CheckingSend = 1;

	//判断队列是否为空以及DMA是否空闲
	if ((BT_UART->ISR & USART_FLAG_TXE) == (uint16_t)RESET)	//上次发送未完成
	{
		m_bEUART2CheckingSend = 0;
		return;
	}
	if(m_EUART2_TxFrm_Head == m_EUART2_TxFrm_Tail)					//队列为空
	{
		if((BT_UART->ISR & USART_FLAG_TC) != (uint16_t)RESET)	 	//发送完毕
		{
			if(m_bEUART2TxEn)
			{
				m_bEUART2TxEn = 0;
/*				temp32 = GPIOC->CRH;			//C10悬空输入
				temp32 &= ~(0x00000000F<<8);		
				temp32 |= (0x000000004<<8);
				GPIOC->CRH = temp32;// */
			}
		}
		m_bEUART2CheckingSend = 0;
		return;
	}
	if(!m_bEUART2TxEn)
	{
		m_bEUART2TxEn = 1;
/*		temp32 = GPIOC->CRH;			//C10复用推挽输出
		temp32 &= ~(0x00000000F<<8);
		temp32 |= (0x000000009<<8);
		GPIOC->CRH = temp32;// */
	}
	//发送一个字节数据
	BT_UART->TDR = m_EUART2_TxFrames[m_EUART2_TxFrm_Tail].buf[s_count];		
	s_count++;
	if(s_count >= m_EUART2_TxFrames[m_EUART2_TxFrm_Tail].len)		//一帧数据发送完毕
	{
		s_count = 0;
		m_EUART2_TxFrm_Tail++;
		if(m_EUART2_TxFrm_Tail == EUART2_TX_FRM_SIZE)
			m_EUART2_TxFrm_Tail = 0;
		m_EUART2_TxFrm_FreeFrmLen++;
	}
	m_bEUART2CheckingSend = 0;
}		   // */

#else
 /*
//检查是否数据帧需要发送
void UART2_CheckSend(void)
{
	//判断是否正在Check
	if(m_bEUART2PushingFrms || m_bEUART2CheckingSend)
		return;
	m_bEUART2CheckingSend = 1;

	//判断DMA通道是否正在工作
	if((EUART2_TX_DMA_CH->CCR & 0x01) != 0)	//通道正在工作
	{
		if((EUART2_DMA->ISR & EUART2_TX_DMA_IT_TC) == (uint32_t)RESET) 	//传输未完成
		{
			m_bEUART2CheckingSend = 0;
			return;
		}
		else
		{
			EUART2_DMA->IFCR = EUART2_TX_DMA_IT_TC;				//清除标志（注意寄存器和标志名称）
			EUART2_TX_DMA_CH->CCR &= ~((u32)0x01);	//禁用DMA
		}
	}
	
	//判断队列是否为空
	if(m_EUART2_TxFrm_Head == m_EUART2_TxFrm_Tail)
	{
		m_bEUART2CheckingSend = 0;
		return;
	}
	
	//开启一次数据发送
	EUART2_TX_DMA_CH->CMAR = (u32)m_EUART2_TxFrames[m_EUART2_TxFrm_Tail].buf;	//要发送的内存地址 
	EUART2_TX_DMA_CH->CNDTR = m_EUART2_TxFrames[m_EUART2_TxFrm_Tail].len;		//要发送的字节数
	EUART2_TX_DMA_CH->CCR |= 0x01;											//开启一次DMA  
	m_EUART2_TxFrm_Tail++;
	if(m_EUART2_TxFrm_Tail == EUART2_TX_FRM_SIZE)
		m_EUART2_TxFrm_Tail = 0;	
	m_EUART2_TxFrm_FreeFrmLen++;
	m_bEUART2CheckingSend = 0;	 
}					// */	
#endif





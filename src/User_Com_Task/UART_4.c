#include "main.h"
#include "UART_4.h"
#include "ParseFrame.h"

#define DVE_UART								USART4		//BMS
//#define EXUART4_USE_TXDMA				//是否使用DMA发送
#define EXUART4_BAUD_RADE				115200

#define EUART4_DMA						DMA1
#define EUART4_RX_DMA_CH	 		DMA1_Channel6

#define EU4_TX_PIN								GPIO_Pin_10
#define EU4_RX_PIN								GPIO_Pin_11
#define EU4_TX_GPIO							GPIOC
#define EU4_RX_GPIO							GPIOC

#define EUART4_RX_BUF_SIZE 			200			//接收缓存大小
#define EUART4_TX_FRM_SIZE			3			//发送队列中的最大帧数
#define EUART4_TX_FRMBUF_SIZE		200			//发送队列每帧缓存大小

typedef struct _EUART4_Frame {				//数据发送帧队列结构体
	u8 len;													//帧长度
	u8 buf[EUART4_TX_FRMBUF_SIZE];			//帧数据
}EUART4_Frame;

//第一串口相关
//数据发送队列变量
EUART4_Frame m_EUART4_TxFrames[EUART4_TX_FRM_SIZE];	//数据发送帧队列	
volatile s8 m_EUART4_TxFrm_Tail = 0;					//数据发送帧队列尾指针
volatile s8 m_EUART4_TxFrm_Head = 0;					//数据发送帧队列头指针
volatile s8 m_EUART4_TxFrm_FreeFrmLen = 0;		//数据发送帧队列剩余帧数
//DMA数据接收缓存
u8 m_EUART4_DMA_RXBuf[EUART4_RX_BUF_SIZE]; 	//DMA数据接收缓存
volatile s32 m_EUART4_RXPtr = 0;			//当前数据地址
//标志变量
volatile u8 m_bEUART4PushingFrms = 0;		//正在往发送队列存数据
volatile u8 m_bEUART4CheckingRec = 0;		//正在确认数据接收
volatile u8 m_bEUART4CheckingSend = 0;		//正在确认数据发送
volatile u8 m_bEUART4TxEn = 0;						//使能发送


void UART4_InitVariables(void)
{	
	m_EUART4_TxFrm_FreeFrmLen = EUART4_TX_FRM_SIZE-1;
}

//UART4 DMA的配置
void UART4_DMAConfiguration(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	//UART2的DMA数据接收初始化
	DMA_DeInit(EUART4_RX_DMA_CH);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)( &(DVE_UART->RDR));			//外设数据寄存器
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)m_EUART4_DMA_RXBuf;				//数据Buf
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//外设作源头
	DMA_InitStructure.DMA_BufferSize = EUART4_RX_BUF_SIZE;						//Buf大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//外设地址不增加
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//内存地址增加
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//字节
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;				//字节（注意与上一个变量名称不同！！）
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								//循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						//优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//非内存到内存
	DMA_Init(EUART4_RX_DMA_CH, &DMA_InitStructure);
	USART_DMACmd(DVE_UART, USART_DMAReq_Rx, ENABLE);							
	DMA_Cmd(EUART4_RX_DMA_CH, ENABLE);			
}

//UART4初始化
void UART4_Int(void)
{
//	s16 temp16;
	GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;

	UART4_InitVariables();		//初始化相关变量
	
	//端口重映射
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_0); 	//TX
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_0);   //RX

	//初始化管脚
  GPIO_InitStructure.GPIO_Pin = EU4_TX_PIN | EU4_RX_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EU4_TX_GPIO, &GPIO_InitStructure);  
	
//	 GPIO_InitStructure.GPIO_Pin = EU4_TX_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_Init(EU4_TX_GPIO, &GPIO_InitStructure); 

	USART_DeInit(DVE_UART);
	USART_InitStructure.USART_BaudRate = EXUART4_BAUD_RADE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;						//校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;

	USART_ClockInit(DVE_UART, &USART_ClockInitStructure);
	USART_Init(DVE_UART, &USART_InitStructure);

	USART_Cmd(DVE_UART, ENABLE);
  //DMA配置（须放到UART4初始化之后）
	UART4_DMAConfiguration();
}


//将一帧数据压入发送队列
void UART4_PushFrame(u8 src_id, u8 dst_id, u8 DataLen, u8 Cmd, u8 Index, u8*pdata)
{
	if(m_bEUART4PushingFrms || m_bEUART4CheckingSend)
		return;
	m_bEUART4PushingFrms = 1;
	
	if(DataLen > EUART4_TX_FRMBUF_SIZE-8 || m_EUART4_TxFrm_FreeFrmLen <= 0)		//帧长度超范围或缓存已满
	{
		m_bEUART4PushingFrms = 0;
		return;
	}

	//将要发送的数据帧压入队列
	m_EUART4_TxFrames[m_EUART4_TxFrm_Head].len = FormatFrame(src_id, dst_id, DataLen, Cmd, Index, pdata, m_EUART4_TxFrames[m_EUART4_TxFrm_Head].buf);
	m_EUART4_TxFrm_Head++;
	if(m_EUART4_TxFrm_Head == EUART4_TX_FRM_SIZE)
		m_EUART4_TxFrm_Head = 0;
	m_EUART4_TxFrm_FreeFrmLen--;
	m_bEUART4PushingFrms = 0;
}

//将最新一帧数据Pop出队列
void UART4_PopFrame(void)
{
	if(m_bEUART4PushingFrms || m_bEUART4CheckingSend)
		return;
	if(m_EUART4_TxFrm_FreeFrmLen >= EUART4_TX_FRM_SIZE-1)
		return;
	m_EUART4_TxFrm_Head--;
	if(m_EUART4_TxFrm_Head < 0)
		m_EUART4_TxFrm_Head = EUART4_TX_FRM_SIZE-1;
	m_EUART4_TxFrm_FreeFrmLen++;
}

//解析单字节数据
void UART4_ParseData(u8 data)
{
	static unsigned char s_RXBuf[EUART4_RX_BUF_SIZE] = {0};
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
			if(s_rxLen > EUART4_RX_BUF_SIZE-7)
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
				ParseFrame(COM_ECU, s_RXBuf);  //解析帧数据
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

void UART4_CheckReceive(void)
{
	s32 DAMCnt = 0;

	s32 MaxDataLen = EUART4_RX_BUF_SIZE;	

	//如果正在往发送队列中添加数据，退出
	if(m_bEUART4PushingFrms)
		return;
	//判断是否正在Check
	if(m_bEUART4CheckingRec)
		return;
	m_bEUART4CheckingRec = 1;
	if(EUART4_RX_DMA_CH->CNDTR == 0)
	{
		m_bEUART4CheckingRec = 0;
		return;
	}
	DAMCnt = EUART4_RX_BUF_SIZE - (EUART4_RX_DMA_CH->CNDTR);
	while( m_EUART4_RXPtr != DAMCnt && MaxDataLen > 0)
	{
		UART4_ParseData(m_EUART4_DMA_RXBuf[m_EUART4_RXPtr]);
		m_EUART4_RXPtr++;
		if( m_EUART4_RXPtr >= EUART4_RX_BUF_SIZE )
		{
			m_EUART4_RXPtr = 0;
		}
		DAMCnt = EUART4_RX_BUF_SIZE - (EUART4_RX_DMA_CH->CNDTR);
		MaxDataLen--;
	}
	m_bEUART4CheckingRec = 0;
}


//查询方式发送数据
void UART4_CheckSend(void)
{
	static s32 s_count = 0;
//	u32 temp32 = 0;

	if(m_bEUART4PushingFrms || m_bEUART4CheckingSend)
		return;
	m_bEUART4CheckingSend = 1;

	//判断队列是否为空以及DMA是否空闲
	if ((DVE_UART->ISR & USART_FLAG_TXE) == (uint16_t)RESET)	//上次发送未完成
	{
		m_bEUART4CheckingSend = 0;
		return;
	}
	
	if(m_EUART4_TxFrm_Head == m_EUART4_TxFrm_Tail)					//队列为空
	{
		if((DVE_UART->ISR & USART_FLAG_TC) != (uint16_t)RESET)	 	//发送完毕
		{
			if(m_bEUART4TxEn)
			{
				m_bEUART4TxEn = 0;
/*				temp32 = GPIOC->CRH;			//C10悬空输入
				temp32 &= ~(0x00000000F<<8);		
				temp32 |= (0x000000004<<8);
				GPIOC->CRH = temp32;// */
			}
		}
		m_bEUART4CheckingSend = 0;
		return;
	}
	if(!m_bEUART4TxEn)
	{
		m_bEUART4TxEn = 1;
/*		temp32 = GPIOC->CRH;			//C10复用推挽输出
		temp32 &= ~(0x00000000F<<8);
		temp32 |= (0x000000009<<8);
		GPIOC->CRH = temp32;// */
	}
	//发送一个字节数据
	DVE_UART->TDR = m_EUART4_TxFrames[m_EUART4_TxFrm_Tail].buf[s_count];		
	s_count++;
	if(s_count >= m_EUART4_TxFrames[m_EUART4_TxFrm_Tail].len)		//一帧数据发送完毕
	{
		s_count = 0;
		m_EUART4_TxFrm_Tail++;
		if(m_EUART4_TxFrm_Tail == EUART4_TX_FRM_SIZE)
			m_EUART4_TxFrm_Tail = 0;
		m_EUART4_TxFrm_FreeFrmLen++;
	}
	m_bEUART4CheckingSend = 0;
 }		   // */


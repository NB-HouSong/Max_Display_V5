/******************** (C) COPYRIGHT 2020 Ninebot *******************************
* File Name     : UART_3.c
* Author        : meihua
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "UART_3.h"

/* Private define ------------------------------------------------------------*/
#define EUART3_BAUD_RADE			115200

#define EUART3_BUF_SIZE 			150	//接收缓存大小
#define EUART3_TX_FRM_SIZE			4	//发送队列中的最大帧数


/* Private typedef -----------------------------------------------------------*/
typedef struct _EUART3_Frame            //数据发送帧队列结构体
{			
	u8 len;								//帧长度
	u8 buf[EUART3_BUF_SIZE];		    //帧数据
} EUART3_Frame;


/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
//数据发送队列变量
EUART3_Frame m_EUART3_TxFrames[EUART3_TX_FRM_SIZE];	    //数据发送帧队列	
volatile s8 m_EUART3_TxFrm_Tail = 0;					//数据发送帧队列尾指针
volatile s8 m_EUART3_TxFrm_Head = 0;					//数据发送帧队列头指针
volatile s8 m_EUART3_TxFrm_FreeFrmLen = 0;		        //数据发送帧队列剩余帧数

//DMA数据接收缓存
u8 m_EUART3_DMA_RXBuf[EUART3_BUF_SIZE]; 	            //DMA数据接收缓存
volatile s32 m_EUART3_DMA_RXPtr = 0;			        //当前数据地址
//标志变量
volatile u8 m_bEUART3PushingFrms = 0;		            //正在往发送队列存数据
volatile u8 m_bEUART3CheckingRec = 0;		            //正在确认数据接收
volatile u8 m_bEUART3CheckingSend = 0;		            //正在确认数据发送
volatile u8 m_bEUART3TxEn = 0;						    //使能发送

static DMA_InitTypeDef Uart3SendDMAInitStructure;

/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

static void UART3_SetPinToTx(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //发送前配置IO为推挽，使能发送，禁止接收
    GPIO_InitStructure.GPIO_Pin = EU3_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//Tx推挽复用输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//Tx推挽复用输出
    GPIO_Init(EU3_TX_GPIO, &GPIO_InitStructure);

    EXT_UART3->CR1 &= ~USART_CR1_RE;
    EXT_UART3->CR1 |= USART_CR1_TE;
}

static void UART3_SetPinToRx(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = EU3_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//Rx复用开漏输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		//Rx复用开漏输出
    GPIO_Init(EU3_TX_GPIO, &GPIO_InitStructure);

    EXT_UART3->CR1 &= ~USART_CR1_TE;
    EXT_UART3->CR1 |= USART_CR1_RE;
}

/*****************************************************************
* Function Name : UART3_InitVariables
* Description   : UART3的变量初始化
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void UART3_InitVariables(void)
{	
	m_EUART3_TxFrm_FreeFrmLen = EUART3_TX_FRM_SIZE-1;
}

/*****************************************************************
* Function Name : UART3_DMAConfiguration
* Description   : UART3 DMA的配置
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void UART3_DMAConfiguration(void)
{
	DMA_InitTypeDef DMA_InitStructure;
    
    // UART3 DMA 映射
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_USART3_DMA_RMP;
    
	//UART的DMA数据接收初始化
	DMA_DeInit(EUART3_RX_DMA_CH);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)( &(EXT_UART3->RDR));		//外设数据寄存器
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)m_EUART3_DMA_RXBuf;				//数据Buf
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//外设作源头
	DMA_InitStructure.DMA_BufferSize = EUART3_BUF_SIZE;						    //Buf大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//外设地址不增加
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//内存地址增加
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//字节
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;				//字节（注意与上一个变量名称不同！！）
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								//循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;						    //优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//非内存到内存
	DMA_Init(EUART3_RX_DMA_CH, &DMA_InitStructure);
	USART_DMACmd(EXT_UART3, USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(EUART3_RX_DMA_CH, ENABLE);

	//UART的DMA数据发送初始化
	DMA_DeInit(EUART3_TX_DMA_CH);
	Uart3SendDMAInitStructure.DMA_PeripheralBaseAddr = (u32)(&(EXT_UART3->TDR));		//外设数据寄存器
	Uart3SendDMAInitStructure.DMA_MemoryBaseAddr = 0;				                    //数据Buf
	Uart3SendDMAInitStructure.DMA_DIR = DMA_DIR_PeripheralDST;							//外设作目标
	Uart3SendDMAInitStructure.DMA_BufferSize = 0;										//Buf大小
	Uart3SendDMAInitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//外设地址寄存器不递增
	Uart3SendDMAInitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//内存地址递增
	Uart3SendDMAInitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//外设字节为单位
	Uart3SendDMAInitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;				//内存字节为单位（注意与上一个变量名称不同！！）
	Uart3SendDMAInitStructure.DMA_Mode = DMA_Mode_Normal;								//普通模式
	Uart3SendDMAInitStructure.DMA_Priority = DMA_Priority_High;							//优先级
	Uart3SendDMAInitStructure.DMA_M2M = DMA_M2M_Disable;								//非内存到内存
	DMA_Init(EUART3_TX_DMA_CH, &Uart3SendDMAInitStructure);

	DMA_ITConfig(EUART3_TX_DMA_CH, DMA_IT_TC, ENABLE);							        //DMA传输完成中断
    DMA_Cmd(EUART3_TX_DMA_CH, DISABLE);
    USART_DMACmd(EXT_UART3, USART_DMAReq_Tx, ENABLE);								    //外设使能DMA  // */
}

/*****************************************************************
* Function Name : UART3_Int
* Description   : UART3 初始化
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void UART3_Int(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

	UART3_InitVariables();		//初始化相关变量
	
	//端口重映射
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_4); 	//TX
	//GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_4);  //RX

	//初始化管脚
    GPIO_InitStructure.GPIO_Pin = EU3_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(EU3_TX_GPIO, &GPIO_InitStructure);

	USART_DeInit(EXT_UART3);
	USART_InitStructure.USART_BaudRate = EUART3_BAUD_RADE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(EXT_UART3, &USART_InitStructure);

    //半双工需要的配置
    EXT_UART3->CR2 &= ~(USART_CR2_LINEN | USART_CR2_CLKEN);
    EXT_UART3->CR2 &= ~(USART_CR3_IREN | USART_CR3_SCEN);
    USART_HalfDuplexCmd(EXT_UART3, ENABLE);
    
    // Enable the USARTx
	USART_Cmd(EXT_UART3, ENABLE);
    
    //初始化使能接收
    UART3_SetPinToRx();

	//DMA配置（须放到UART初始化之后）
	UART3_DMAConfiguration();
}

/*****************************************************************
* Function Name : UART3_DmaSend
* Description   : 将发送数据和长度放入DMA缓存里面
* Input         : sendFramePtr：发送帧地址
* Output        : None
* Notes         :
******************************************************************/	
static void UART3_DmaSend(EUART3_Frame *sendFramePtr)
{
    //DMA_ClearFlag(DMA1_FLAG_TC2);
    DMA_Cmd(EUART3_TX_DMA_CH, DISABLE);

    EUART3_TX_DMA_CH->CMAR = (uint32_t)sendFramePtr->buf;
    DMA_SetCurrDataCounter(EUART3_TX_DMA_CH, sendFramePtr->len);
    DMA_Cmd(EUART3_TX_DMA_CH, ENABLE);
}
    
/*****************************************************************
* Function Name : UART3_PushFrame
* Description   : 将一帧数据压入发送队列
* Input         : src_id：源地址 dst_id：目标地址 DataLen：数据长度
                : Cmd：指令 Index：索引 pdata：数据 
* Output        : None
* Notes         :
******************************************************************/	
void UART3_PushFrame(u8 src_id, u8 dst_id, u8 DataLen, u8 Cmd, u8 Index, u8*pdata)
{
	if(m_bEUART3PushingFrms || m_bEUART3CheckingSend)
		return;
	m_bEUART3PushingFrms = 1;
	
	if(DataLen > EUART3_BUF_SIZE - 8 || m_EUART3_TxFrm_FreeFrmLen <= 0)		//帧长度超范围或缓存已满
	{
		m_bEUART3PushingFrms = 0;
		return;
	}

	//将要发送的数据帧压入队列
	m_EUART3_TxFrames[m_EUART3_TxFrm_Head].len = FormatFrame(src_id, dst_id, DataLen, Cmd, Index, pdata, m_EUART3_TxFrames[m_EUART3_TxFrm_Head].buf);
	m_EUART3_TxFrm_Head++;
	if(m_EUART3_TxFrm_Head == EUART3_TX_FRM_SIZE)
		m_EUART3_TxFrm_Head = 0;
	m_EUART3_TxFrm_FreeFrmLen--;
	m_bEUART3PushingFrms = 0;
}

/*****************************************************************
* Function Name : UART3_PopFrame
* Description   : 将最新一帧数据Pop出队列
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void UART3_PopFrame(void)
{
	if(m_bEUART3PushingFrms || m_bEUART3CheckingSend)
		return;
	if(m_EUART3_TxFrm_FreeFrmLen >= EUART3_TX_FRM_SIZE-1)
		return;
	m_EUART3_TxFrm_Head--;
	if(m_EUART3_TxFrm_Head < 0)
		m_EUART3_TxFrm_Head = EUART3_TX_FRM_SIZE-1;
	m_EUART3_TxFrm_FreeFrmLen++;
}

/*****************************************************************
* Function Name : UART3_ParseData
* Description   : 解析单字节数据
* Input         : data：要解析的数据
* Output        : None
* Notes         :
******************************************************************/	
void UART3_ParseData(u8 data)
{
	static unsigned char s_RXBuf[EUART3_BUF_SIZE] = {0};
	static unsigned char s_bFrameHead1 = 0;
	static unsigned char s_bBeginFrame = 0;
	static unsigned short s_rxPos = 0;
	static unsigned short s_rxLen = 0;
	static unsigned short s_csum = 0;
	unsigned short temp16 = 0;
	
	if(s_bBeginFrame)   //是否开始新的一帧
	{
		s_RXBuf[s_rxPos] = data;
		if(s_rxPos == 0)
		{
			s_rxLen = data+7;
			if(s_rxLen > EUART3_BUF_SIZE - 7)
			{
				s_bFrameHead1 = s_bBeginFrame = 0;
				s_csum = 0;
				return;
			}
		}
		s_rxPos++;
		if(s_rxPos == s_rxLen)              //一帧数据传输完毕
		{
			s_csum -= s_RXBuf[s_rxPos-2];	//校验和多加了一个字节
			s_csum = ~s_csum;
			temp16 = s_RXBuf[s_rxPos-2] + (s_RXBuf[s_rxPos-1]<<8);
			if(s_csum == temp16)
				ParseFrame(COM_SCOOTER, s_RXBuf);  //解析帧数据
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

/*****************************************************************
* Function Name : UART3_CheckReceive
* Description   : 串口查询接收
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void UART3_CheckReceive(void)
{
	s32 DAMCnt = 0;
	s32 MaxDataLen = EUART3_BUF_SIZE;	
	
	//如果正在往发送队列中添加数据，退出
	if(m_bEUART3PushingFrms)
		return;
	//判断是否正在Check
	if(m_bEUART3CheckingRec)
		return;
	m_bEUART3CheckingRec = 1;
	if(EUART3_RX_DMA_CH->CNDTR == 0)
	{
		m_bEUART3CheckingRec = 0;
		return;
	}
	DAMCnt = EUART3_BUF_SIZE - (EUART3_RX_DMA_CH->CNDTR);
	while( m_EUART3_DMA_RXPtr != DAMCnt && MaxDataLen > 0)
	{
		UART3_ParseData(m_EUART3_DMA_RXBuf[m_EUART3_DMA_RXPtr]);
		m_EUART3_DMA_RXPtr++;
		if( m_EUART3_DMA_RXPtr >= EUART3_BUF_SIZE )
		{
			m_EUART3_DMA_RXPtr = 0;
		}
		DAMCnt = EUART3_BUF_SIZE - (EUART3_RX_DMA_CH->CNDTR);
		MaxDataLen--;
	}
	m_bEUART3CheckingRec = 0;	
}


/*****************************************************************
* Function Name : UART3_CheckSend
* Description   : 查询方式发送数据
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void UART3_CheckSend(void)
{
	if(m_bEUART3PushingFrms || m_bEUART3CheckingSend)
		return;
	m_bEUART3CheckingSend = 1;

	//判断队列是否为空以及DMA是否空闲
	if ((EXT_UART3->ISR & USART_FLAG_TXE) == (uint16_t)RESET)	    //上次发送未完成
	{
		m_bEUART3CheckingSend = 0;
		return;
	}
	if(m_EUART3_TxFrm_Head == m_EUART3_TxFrm_Tail)					//队列为空
	{
		if((EXT_UART3->ISR & USART_FLAG_TC) != (uint16_t)RESET)	 	//发送完毕
		{
			if(m_bEUART3TxEn)
			{
				m_bEUART3TxEn = 0;
			}
		}
		m_bEUART3CheckingSend = 0;
		return;
	}
	if(!m_bEUART3TxEn)
	{
		m_bEUART3TxEn = 1;
	}
    
    UART3_SetPinToTx();
    UART3_DmaSend(&m_EUART3_TxFrames[m_EUART3_TxFrm_Tail]);
//    for(int s_count = 0; s_count < m_EUART3_TxFrames[m_EUART3_TxFrm_Tail].len; s_count++)
//    {
        //发送一个字节数据
//        EXT_UART3->TDR = m_EUART3_TxFrames[m_EUART3_TxFrm_Tail].buf[s_count];
//        while (USART_GetFlagStatus(EXT_UART3, USART_FLAG_TC) == RESET)
//            ; // Wait for the data to be send completely
//    }
//    m_EUART3_TxFrm_Tail++;
//    if(m_EUART3_TxFrm_Tail == EUART3_TX_FRM_SIZE)
//        m_EUART3_TxFrm_Tail = 0;
//    m_EUART3_TxFrm_FreeFrmLen++;

//    m_bEUART3CheckingSend = 0;

//    USART_ClearFlag(EXT_UART3, USART_FLAG_TC);
//	//发送一个字节数据
//	EXT_UART3->TDR = m_EUART3_TxFrames[m_EUART3_TxFrm_Tail].buf[s_count];

//	s_count++;
//	if(s_count >= m_EUART3_TxFrames[m_EUART3_TxFrm_Tail].len)		//一帧数据发送完毕
//	{
//		s_count = 0;
//		m_EUART3_TxFrm_Tail++;
//		if(m_EUART3_TxFrm_Tail == EUART3_TX_FRM_SIZE)
//			m_EUART3_TxFrm_Tail = 0;
//		m_EUART3_TxFrm_FreeFrmLen++;
//	}
//	m_bEUART3CheckingSend = 0;
}		   // */

void DMA1_Channel2_3_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC2))
    {
        DMA_ClearFlag(DMA1_FLAG_TC2);
        DMA_ClearITPendingBit(DMA1_IT_GL2);

        while ((EXT_UART3->ISR & USART_FLAG_TC) == (uint16_t)RESET)
            ; // Wait for the data to be send completely

        //置接收
        UART3_SetPinToRx();
        
        //发送结束，释放帧
        m_EUART3_TxFrm_Tail++;
        if(m_EUART3_TxFrm_Tail == EUART3_TX_FRM_SIZE)
            m_EUART3_TxFrm_Tail = 0;
        m_EUART3_TxFrm_FreeFrmLen++;
        
        m_bEUART3CheckingSend = 0;
    }
}

 //==================================== The End===============================================/

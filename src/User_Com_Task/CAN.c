///******************** (C) COPYRIGHT 2020 Ninebot *******************************
//* File Name     : CAN.c
//* Author        : meihua
//* Version       : V1.0.0
//* Date          : 2020-11-13
//* Description   : 
//********************************************************************************/

///* Includes ------------------------------------------------------------------*/
//#include "CAN.h"

///* Private typedef -----------------------------------------------------------*/
///* Private define ------------------------------------------------------------*/
//#define MAX_CAN_FRM_NUM	    25			    //队列中的最大帧数

///* Private macro -------------------------------------------------------------*/
///* Variables -----------------------------------------------------------------*/
//CanTxMsg m_TxMessage;
//CanRxMsg m_RxMessage;

//CanTxMsg m_CANTxMsgs[MAX_CAN_FRM_NUM];  //CAN发送帧队列
//volatile u8 m_CAN_TxFrm_Tail = 0;		//CAN数据发送帧队列尾指针
//volatile u8 m_CAN_TxFrm_Head = 0;		//CAN数据发送帧队列头指针

//volatile u8 m_bCANPushingFrms = 0;		//正在往发送队列存数据
//volatile u8 m_bCANCheckingRec = 0;		//正在确认数据接收
//volatile u8 m_bCANCheckingSend= 0;		//正在确认数据发送

//uint8_t mFilterCnt[2];                  //FIFO0/FIFO1的CAN过滤器编号

///* Private function prototypes -----------------------------------------------*/
///* Functions -----------------------------------------------------------------*/

///*****************************************************************
//* Function Name : CanFilterConfig
//* Description   : 初始化过滤器
//* Input         : CanFilterClass* self
//* Output        : None
//* Notes         :
//******************************************************************/	
//void CanFilterConfig(CanFilterClass* self)
//{
//	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

//	uint32_t id = self->id;
//	uint32_t mask = self->mask;

//	if(!self->is_ext)
//	{
//		id = id << 3 | CAN_Id_Standard | CAN_RTR_Data;
//	}
//	else
//	{
//		id = id << 3 | CAN_Id_Extended | CAN_RTR_Data ;
//		mask = mask << 3 | CAN_Id_Extended; 
//	}

//	CAN_FilterInitStructure.CAN_FilterNumber = self->filter_number;					
//	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;		//屏蔽位模式
//	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;	//
//	CAN_FilterInitStructure.CAN_FilterIdHigh = (uint16_t)(id >> 16);
//	CAN_FilterInitStructure.CAN_FilterIdLow = (uint16_t)(id); 
//	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (uint16_t)(mask >> 16);
//	CAN_FilterInitStructure.CAN_FilterMaskIdLow = (uint16_t)(mask);
//	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = self->fifo;
//	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;

//	CAN_FilterInit(&CAN_FilterInitStructure);
//}

///*****************************************************************
//* Function Name : CAN_Form_Extid
//* Description   : 构建扩展ID
//* Input         : src_id：源地址 dst_id：目标地址 
//                : cmd：指令 index：索引
//* Output        : None
//* Notes         : 扩展ID 28位，组成部分依次为：源地址6位，目标地址
//                : 6位，指令8位，索引8位
//******************************************************************/	
//static u32 CAN_Form_Extid(u8 src_id, u8 dst_id, u8 cmd, u8 index)
//{
//	CAN_EXT_ID can_extid;
//	can_extid.params.CAN_Index = index;
//	can_extid.params.CAN_Cmd = cmd;
//	can_extid.params.CAN_Dstid = dst_id;
//	can_extid.params.CAN_Srcid = src_id;
//	
//	return can_extid.Cache;
//}

///*****************************************************************
//* Function Name : NbCanFilterConfig
//* Description   : 初始化过滤器
//* Input         : id_to_recv：接收数据的ID  fifo：选用的FIFO
//* Output        : None
//* Notes         :
//******************************************************************/	
//int NbCanFilterConfig(uint8_t id_to_recv, uint8_t fifo)
//{	
//	CanFilterClass filter;

//	if(fifo > 1)
//		while(1);

//	filter.filter_number = mFilterCnt[fifo];
//	filter.fifo = fifo;
//	filter.id = CAN_Form_Extid(0, id_to_recv, 0, 0);
//	filter.mask = CAN_Form_Extid(0, 0x3F, 0, 0);
//	filter.is_ext = 1;

//	mFilterCnt[fifo] ++;

//	CanFilterConfig(&filter);

//	return filter.filter_number;
//}

///*****************************************************************
//* Function Name : User_CAN_Init
//* Description   : CAN初始化
//* Input         : None
//* Output        : None
//* Notes         :
//******************************************************************/	
//void User_CAN_Init(void)
//{
//	GPIO_InitTypeDef       GPIO_InitStructure;
//	CAN_InitTypeDef        CAN_InitStructure;

//	uint8_t id_recv[] = {EBIKE_DIS_ID, EBIKE_BROADCAST_ID, EBIKE_CABLELOCK_ID, EBIKE_ECU_ID, EBIKE_IOT_ID, EBIKE_BMS_ID};
//	uint8_t i;
//	//外设时钟设置
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);	
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_4);
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_4);

//	// Configure CAN pin: TX | RX
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_Level_3;
//	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = CAN_EN_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_Level_3;
//	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
//	GPIO_Init(CAN_EN_GPIO, &GPIO_InitStructure);
//	CAN_EN();  //	GPIO_ResetBits(CAN_EN_GPIO, CAN_EN_PIN); //使能CAN

//	//CAN寄存器初始化
////	CAN_DeInit(CAN);
//	CAN_StructInit(&CAN_InitStructure);

//	//CAN单元初始化
//	CAN_InitStructure.CAN_TTCM=DISABLE;			   //MCR-TTCM  关闭时间触发通信模式使能
//	CAN_InitStructure.CAN_ABOM=ENABLE;			   //MCR-ABOM  自动离线管理
//	CAN_InitStructure.CAN_AWUM=ENABLE;			   //MCR-AWUM  使用自动唤醒模式
//	CAN_InitStructure.CAN_NART=DISABLE;			   //MCR-NART  允许报文自动重传	  DISABLE-自动重传
////	CAN_InitStructure.CAN_NART=ENABLE;			    //不自动重传
//	CAN_InitStructure.CAN_RFLM=DISABLE;			   //MCR-RFLM  接收FIFO 锁定模式  DISABLE-溢出时新报文会覆盖原有报文
////	CAN_InitStructure.CAN_TXFP=ENABLE;			   //MCR-TXFP  发送FIFO优先级 DISABLE-优先级取决于报文标示符
//	CAN_InitStructure.CAN_TXFP=DISABLE;            //先进先出
//	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;  //正常工作模式
//	
//	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;		   //BTR-SJW 重新同步跳跃宽度 1个时间单元
//	CAN_InitStructure.CAN_BS1=CAN_BS1_10tq;		   //BTR-TS1 时间段1 占用了5个时间单元
//	CAN_InitStructure.CAN_BS2=CAN_BS2_1tq;		   //BTR-TS1 时间段2 占用了3个时间单元
//	CAN_InitStructure.CAN_Prescaler = 4;		   //BTR-BRP 波特率分频器  定义了时间单元的时间长度 48/(1+1+10)/8=500K

//	if(CAN_Init(CAN,&CAN_InitStructure) == CANINITFAILED)
//	{
////		while(1);
//	}

//	//CAN过滤器初始化
//	for(i = 0; i < (sizeof(id_recv) / sizeof(id_recv[0])); i ++)
//	{
//		NbCanFilterConfig(id_recv[i], CAN_FIFO0);
//	}
//	
//	m_TxMessage.RTR = CAN_RTR_DATA;	 				
//	m_TxMessage.IDE = CAN_ID_EXT;
//}

///*****************************************************************
//* Function Name : CAN_PushFrameStru
//* Description   : 单帧数据压栈
//* Input         : CanTxMsg* TxMessage：要发送给的一帧数据
//* Output        : 返回压栈结果
//* Notes         :
//******************************************************************/	
//int CAN_PushFrameStru(CanTxMsg* TxMessage)
//{
//	if(m_bCANPushingFrms)
//		return 0;	
//	m_bCANPushingFrms = 1;

//	memcpy(&m_CANTxMsgs[m_CAN_TxFrm_Head], TxMessage, sizeof(CanTxMsg));

//	m_CAN_TxFrm_Head++;
//	if(m_CAN_TxFrm_Head == MAX_CAN_FRM_NUM)
//		m_CAN_TxFrm_Head = 0;

//	//如果缓存已满，新的数据将覆盖旧的数据
//	if(m_CAN_TxFrm_Head == m_CAN_TxFrm_Tail)
//	{
//		m_CAN_TxFrm_Tail++;
//		if(m_CAN_TxFrm_Tail == MAX_CAN_FRM_NUM)
//			m_CAN_TxFrm_Tail = 0;
//	}
//	m_bCANPushingFrms = 0;
//	return 1;
//}

///*****************************************************************
//* Function Name : CAN_PushMulFrame
//* Description   : 将多于一帧的数据压栈
//* Input         : data_length：数据长度 src_id：源地址 
//                : dst_id：目标地址 cmd：指令 index：索引 
//                : pdata：数据
//* Output        : None
//* Notes         :
//******************************************************************/	
//void CAN_PushMulFrame(u8 data_length, u8 src_id, u8 dst_id, u8 cmd, u8 index,u8* pdata)
//{
//	u16 i = 0;
//	u16 len = 0;

//	if(data_length == 0)
//	{
//		m_TxMessage.ExtId = CAN_Form_Extid(src_id, dst_id, cmd, index);	
//		m_TxMessage.DLC = 0;
//		CAN_PushFrameStru(&m_TxMessage); 
//	}
//	else
//	{
//		while(i < data_length)
//		{
//			len = data_length - i;
//			len = len > CAN_MAX_FRM_LEN ? CAN_MAX_FRM_LEN : len;
//			m_TxMessage.ExtId = CAN_Form_Extid(src_id, dst_id, cmd, index + (i>>1));	
//			m_TxMessage.DLC = len;
//			memcpy((u8*)&m_TxMessage.Data[0], &pdata[i], m_TxMessage.DLC);
//			CAN_PushFrameStru(&m_TxMessage); 
//			i += len;
//		}
//	}
//}

///*****************************************************************
//* Function Name : CAN_Check_Send
//* Description   : 查询方式发送
//* Input         : None
//* Output        : None
//* Notes         :
//******************************************************************/	
//void CAN_Check_Send(void)
//{
//	if(m_bCANPushingFrms == 1)
//		return;
//	else if(m_bCANPushingFrms != 0)
//		m_bCANPushingFrms = 0;
//	//判断是否正在Check
//	if(m_bCANCheckingSend == 1)
//		return;
//	m_bCANCheckingSend = 1;
//	if(m_CAN_TxFrm_Head == m_CAN_TxFrm_Tail)		//队列为空
//	{
//		m_bCANCheckingSend = 0;
//		return;
//	}
//	if ((CAN->MSR & ((uint32_t)0x00000100)) != 0) 	//正在发送
//	{
//		m_bCANCheckingSend = 0;
//		return;	
//	}
//	if(CAN_Transmit(CAN, &m_CANTxMsgs[m_CAN_TxFrm_Tail]) != CAN_NO_MB)		//发送成功
//	{
//		m_CAN_TxFrm_Tail++;
//		if(m_CAN_TxFrm_Tail == MAX_CAN_FRM_NUM)
//			m_CAN_TxFrm_Tail = 0;
//	}
//	m_bCANCheckingSend = 0;
//}

///*****************************************************************
//* Function Name : CAN_Check_Recive
//* Description   : 查询方式接收
//* Input         : None
//* Output        : None
//* Notes         :
//******************************************************************/	
//void CAN_Check_Recive(void)
//{
//	int MaxFrmNum = 3;

//	//判断是否正在Check
//	if(m_bCANCheckingRec == 1)
//		return;
//	else if(m_bCANCheckingRec != 0)
//		m_bCANCheckingRec = 0;
//	m_bCANCheckingRec = 1;
//	while(((uint8_t)(CAN->RF0R&(uint32_t)0x03) > 0) && (MaxFrmNum > 0))   //Reception of a new message, FIFO0，主要接收扩展帧格式数据
//	{
//		CAN_Receive(CAN, CAN_FIFO0, &m_RxMessage);       
//		CanParseDataHandler(&m_RxMessage); 
//		MaxFrmNum --;
//	}
//	m_bCANCheckingRec = 0;
//}

////==================================== The End===============================================/

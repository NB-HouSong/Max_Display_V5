#include "COMx_Config.h"


static void COMX_DMAConfiguration(_COMX_OB_CONFIG_T *COMx_this)
{
    DMA_InitTypeDef DMA_InitStructure;

    //UART的DMA数据接收初始化
    DMA_DeInit(COMx_this->Rx_dma_channelx);
    SYSCFG->CFGR1 |= COMx_this->dma_remap_address;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)( &(COMx_this->COMX_Port->RDR));			//外设数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)COMx_this->dma_buf;				//数据Buf
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//外设作源头
    DMA_InitStructure.DMA_BufferSize = (u32)COMx_this->dma_buf_size;						//Buf大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//外设地址不增加
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//内存地址增加
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;				//字节（注意与上一个变量名称不同！！）
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								//循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						//优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//非内存到内存
    DMA_Init(COMx_this->Rx_dma_channelx, &DMA_InitStructure);
    USART_DMACmd(COMx_this->COMX_Port, USART_DMAReq_Rx, ENABLE);
    DMA_Cmd(COMx_this->Rx_dma_channelx, ENABLE);

}


//COM初始化
void COMX_Drive_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    USART_ClockInitTypeDef USART_ClockInitStructure;
    _COMX_OB_CONFIG_T *COMx_this = &_COM1_object;

    //端口重映射
    GPIO_PinAFConfig(COMx_this->COMx_TX_GPIO_PORT, COMx_this->TX_GPIO_PinSource, COMx_this->GPIO_AF_X); 	//TX
    GPIO_PinAFConfig(COMx_this->COMx_TX_GPIO_PORT, COMx_this->RX_GPIO_PinSource, COMx_this->GPIO_AF_X);  //RX

    //初始化管脚
    GPIO_InitStructure.GPIO_Pin = COMx_this->_COMx_TX_PIN | COMx_this->_COMx_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(COMx_this->COMx_RX_GPIO_PORT, &GPIO_InitStructure);
    //GPIO_Init(COMx_this->COMx_TX_GPIO_PORT, &GPIO_InitStructure);
    USART_DeInit(COMx_this->COMX_Port);
    USART_InitStructure.USART_BaudRate = COMx_this->COMX_BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
    USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
    USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
    USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;

    USART_ClockInit(COMx_this->COMX_Port, &USART_ClockInitStructure);
    USART_Init(COMx_this->COMX_Port, &USART_InitStructure);
    //Enable usartx receive interrupt if needed
    USART_ITConfig(COMx_this->COMX_Port, USART_IT_RXNE, ENABLE);
    // Enable the USARTx
    USART_Cmd(COMx_this->COMX_Port, ENABLE);
}

static u16 CheckSum(u8* pdata, u8 len) {
	uint16_t sum = 0;
	uint8_t i;
	for (i = 0; i < len; i++)
		sum += pdata[i];
	sum = ~sum;
	return sum;
}


/** @brief Function for the push data in buff.
 */

void COMX_Push_Frame(_PUSH_DATA_STRC push_data) 
{
	uint16_t csum = 0;
	uint8_t i;
    //_COMX_OB_CONFIG_T *COMx_this = &_COM1_object;
    
    if (push_data.data_len > COM_TX_BUF_SIZE - 8)
    return;
    
	for (i = 0; i < COM_TX_FRM_SIZE; i++) {
		if (_COM1_object.send_buf[i].send_cmd_flag == DATAFREE) {							
			_COM1_object.send_buf[i].send_WPointer = push_data.data_len + 9;
			_COM1_object.send_buf[i].send_cmd_flag = DATAON;
			break;
		}
	}
    if(i == COM_TX_FRM_SIZE)
    return;
    
    _COM1_object.send_buf[i].data[0] = DATAHEAD0;
    _COM1_object.send_buf[i].data[1] = DATAHEAD1;
    memcpy(&_COM1_object.send_buf[i].data[2],&push_data.data_len,5);
    
    memcpy(&_COM1_object.send_buf[i].data[7],push_data.pdata,push_data.data_len);
	csum = CheckSum(&_COM1_object.send_buf[i].data[2], push_data.data_len + 5);	 	//CheckSum
	_COM1_object.send_buf[i].data[push_data.data_len + 7] = csum;
	_COM1_object.send_buf[i].data[push_data.data_len + 8]  = csum >> 8;

}


void COMX_Analysis_Frame(void) {
	uint16_t writeinpoiner = 0;
	uint16_t readoutpointer = 0;
	uint16_t datapointer = 0;
	uint16_t resetpointer = 0;
	uint16_t buflength = 0;
	uint16_t datalength = 0;
	uint16_t i,j;
	uint8_t header[2];
	uint16_t crc_result = 0;
    uint16_t effective_datalen;
    uint16_t temp_received_crc;
    
    _COMX_OB_CONFIG_T *COM1_this = &_COM1_object;
    
	writeinpoiner = COM1_this->writein_pointer;
	readoutpointer = COM1_this->readout_pointer;
    
	if (readoutpointer > writeinpoiner) 
    {
		buflength = COM_RX_BUF_SIZE - readoutpointer + writeinpoiner;
	} 
    else 
    {
		buflength = writeinpoiner - readoutpointer;
	}    
	if (buflength > 2) {
        
        switch (readoutpointer) {
            
		case COM_RX_BUF_SIZE - 2:
			header[0] = COM1_this->data_buf[readoutpointer];
			header[1] = COM1_this->data_buf[readoutpointer + 1];
			datalength = COM1_this->data_buf[0];
			resetpointer = datapointer = 1;
			break;
        
		case COM_RX_BUF_SIZE - 1:
			header[0] = COM1_this->data_buf[readoutpointer];
			header[1] = COM1_this->data_buf[0];
			datalength = COM1_this->data_buf[1];
			resetpointer = datapointer = 2;
			break;
        
		default:
			header[0] = COM1_this->data_buf[readoutpointer];
			header[1] = COM1_this->data_buf[readoutpointer + 1];
			datalength = COM1_this->data_buf[readoutpointer + 2];
			resetpointer = datapointer = readoutpointer + 3;
			if (datapointer >= COM_RX_BUF_SIZE) {
				resetpointer = datapointer = 0;
			}
			break;
		}
		

        if(header[0] == DATAHEAD0 && header[1] == DATAHEAD1)
        {
			if (buflength < (datalength +9))
				return;
            if (datalength > 247)
				return;          
            if(datalength > 0)
            {
                COM1_this->out_buf[0] = header[0];
                COM1_this->out_buf[1] = header[1];
                COM1_this->out_buf[2] = datalength;
               // datapointer = datapointertemp;
                effective_datalen = datalength + 4;// src_id,dst_id,cmd,index
                for (i = 0; i < effective_datalen + 1; i++)
                {
                    if (i < effective_datalen)
                    {
                        COM1_this->out_buf[i + 3] =  COM1_this->data_buf[datapointer];
                        datapointer++;
                        if (datapointer >= 255)//256
                            datapointer = 0;
                    }
                    crc_result +=  COM1_this->out_buf[2 + i];
                }                
                crc_result = ~crc_result;
                COM1_this->out_buf[i + 2] = COM1_this->data_buf[datapointer];
				COM1_this->out_buf[i + 3] = COM1_this->data_buf[datapointer + 1];
                datapointer += 2;
				temp_received_crc = (COM1_this->out_buf[i+3] << 8) + COM1_this->out_buf[i+2];
                if (crc_result == temp_received_crc)
                {
                    for(j=0;j<COM_RX_FRM_SIZE;j++)
                    {
                        if(COM1_this->recvcmd[j].recv_cmd_flag == DATAFREE)//有空的buff
                        {
                            break;                       
                        }                        
                    }
                    
                    if(j!= COM_RX_FRM_SIZE)//有空的buff
                    {
                        memcpy(COM1_this->recvcmd[j].data_buf,COM1_this->out_buf,datalength+7);
                        COM1_this->recvcmd[j].recv_cmd_flag = DATAON; 
                        COM1_this->recvcmd[j].frame_type = 2;                        
                        //p->cmd_process_pointer = p->cmd_buf[FRM_CMD-1];   //取出命令字                     
                    }   
                    if (datapointer >= COM_RX_BUF_SIZE)
                        datapointer = 0;
                    COM1_this->readout_pointer = datapointer;
                    
                }
                else 
                {
					COM1_this->readout_pointer = resetpointer;					
				}
            }   
        }
		else 
        {
			readoutpointer++;
			if (readoutpointer >= COM_RX_BUF_SIZE)
				readoutpointer = 0;            
			COM1_this->readout_pointer = readoutpointer;
		}
   }
}

/***********************************************************************************
 * @brief     Function for  send uart buff data
 * @param[in] void
 * @return:   void	 
************************************************************************************/  
void COMX_Transmit_Frame(void)
{
#if 0	
    static uint32_t s_delay_cnt = 0;
#endif
        
    switch(_COM1_object.send_buff_pointer->send_cmd_flag){
    case DATAFREE:
        _COM1_object.send_buff_pointer = _COM1_object.send_buff_pointer->next;
    break;
    
    case DATAON:
        USART_SendData(_COM1_object.COMX_Port, _COM1_object.send_buff_pointer->data[_COM1_object.send_buff_pointer->send_RPointer]);
        _COM1_object.send_buff_pointer->send_RPointer = 1;
        _COM1_object.send_buff_pointer->send_cmd_flag = DATASENDING;
        USART_ITConfig(_COM1_object.COMX_Port, USART_IT_TC, ENABLE);              
    break;
    
    case DATASENDOVER:
#if 0            
     if(++s_delay_cnt > 500)
     {
        _COM1_object.send_buff_pointer->send_cmd_flag = DATAFREE;
        s_delay_cnt = 0;
     }
#endif         
    break;			
    default:break;				
	}	
}

//==================================== The End===============================================/

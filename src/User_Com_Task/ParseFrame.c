/* Includes ------------------------------------------------------------------*/
#include "ParseFrame.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
u8 pData_iap[150] = {0};

/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/


/*****************************************************************
* Function Name : CheckSum
* Description   : 计算串口接收到数据的校验码
* Input         : u8* pdata：接收到的数据 u32 len：接收到的数据长度
* Output        : 校验和
* Notes         :
******************************************************************/	
u16 CheckSum(u8* pdata, u32 len)
{
	u16 sum = 0;
	u32 i;
	for(i=0; i<len; i++)
		sum += pdata[i];
	sum = ~sum;
	return sum;
}

/*****************************************************************
* Function Name : FormatFrame
* Description   : 将一帧数据压入堆栈
* Input         : src_id：源地址 dst_id：目标地址 DataLen：数据长度
                : Cmd：指令 Index：索引 pdata：数据 FormatBuf：位置
* Output        : 返回数据帧长度
* Notes         :
******************************************************************/	
u8 FormatFrame(u8 src_id, u8 dst_id, u8 DataLen, u8 Cmd, u8 Index, u8* pdata, u8* FormatBuf)
{
  u8 i = 0;
  u8 pos = 0;
  u16 csum = 0;
  
  FormatBuf[pos++] = 0x5A;
  FormatBuf[pos++] = 0xA5;
  FormatBuf[pos++] = DataLen;
  FormatBuf[pos++] = src_id;
	FormatBuf[pos++] = dst_id;
  FormatBuf[pos++] = Cmd;
  FormatBuf[pos++] = Index;
  for(i=0; i<DataLen; i++)
  {
    FormatBuf[pos++] = pdata[i];
  }
  csum = CheckSum(&FormatBuf[2],pos-2);
  FormatBuf[pos++] = csum;
  FormatBuf[pos++] = csum>>8;
  return pos;
}

/*****************************************************************
* Function Name : PushMiniFrame
* Description   : 将要发送的数据压入相应的堆栈
* Input         : src_id：源地址 dst_id：目标地址 DataLen：数据长度
                : Cmd：指令 Index：索引 pdata：数据 Channel：通道
* Output        : None
* Notes         :
******************************************************************/	
void PushMiniFrame(u8 src_id, u8 dst_id, u8 DataLen, u8 Cmd, u8 Index, u8*pdata, u8 Channel)
{
//	if(Channel == CAN_SEL)
//		CAN_PushMulFrame(DataLen, src_id, dst_id, Cmd, Index, pdata);
//	else if(Channel == COM_BMS)
        UART3_PushFrame(src_id, dst_id, DataLen, Cmd, Index, pdata);
}

/*****************************************************************
* Function Name :  ParseFrame
* Description   :  解析通过串口和CAN总线接收的数据
* Input         :  Channel：通讯通道  pData：要解析的数据
* Output        :  None
* Notes         :
******************************************************************/
void ParseFrame(uint8_t Channel, uint8_t* pData)
{
    //void Channel;
    uint16_t srcID = 0, dstID, cmd;
    srcID = pData[FRM_SRC_ID];
    dstID = pData[FRM_DST_ID];

    u8 BLE_Uart_ID = MY_ID;   //Voi定制

    if(pData[FRM_SRC_ID] == ID_UART_DEBUGGER && pData[FRM_CMD] == CMD_IAP_RD)
    {
        PushMiniFrame(MY_ID, pData[FRM_SRC_ID], 1, CMD_IAP_RD_ACK, 0, &BLE_Uart_ID, Channel);	
        return;
    }
        
    if(dstID == MY_ID)
    {
        g_bool[B_ECU_ACK] = 1;
        
        cmd = pData[FRM_CMD];
        switch(cmd)
        {
            case CMD_SCO_CTL:
                //更新滑板车数据，回调给主函数
				memcpy((u8*)&g_myself_data.Scooter_Info, &pData[FRM_DATA], pData[FRM_LEN]);
                break;
            case CMD_IAP_BEGIN:
            case CMD_IAP_WR:
            case CMD_IAP_CRC:
            case CMD_IAP_RESET:
                //仪表升级数据，回调给主函数
                g_bool[B_UPD_IAP] = 1;
                IAP_ParseFrame((uint8_t *)pData, 0);
                break;
            case CMD_NFC_UPDATE: //上载NFC卡片信息返回
                g_bool[B_NFC_UPDATE_OK] = 1;
                break;
            default:break;
        }
        return;
    }
}

////==================================== The End===============================================/

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


///*****************************************************************
//* Function Name :  ParseIAPFrame
//* Description   :  解析IAP数据
//* Input         :  Channel：通讯通道  pData：要解析的数据
//* Output        :  None
//* Notes         :
//******************************************************************/
//void ParseIAPFrame(u8 Channel, u8* pData)
//{
//	//通过CAN转发IAP数据时，将串口的128byte数据分成16包8byte发送，索引高4bit为串口数据帧索引，低4bit为CAN包数
//	u8 ackdata[2] = {0}; 
//	static u8 BMS_IAP_Index = 0; //BMS IAP 数据索引
//	static u8 Ack_cmd = 0;       //每收到8byte CAN帧的返回指令
//	
//	g_bool[B_UPD_IAP] = 1;

//	if(pData[FRM_CMD] == CMD_IAP_WR || pData[FRM_CMD] == CMD_SPK_WR_DATA)
//	{
//		ackdata[0] = pData[FRM_INDEX]&0x0F;      //小index
//		ackdata[1] = (pData[FRM_INDEX]&0xF0)>>4; //大index
//		memcpy((u8*)&pData_iap[FRM_LEN], &pData[FRM_LEN], 4);//帧头不变
//		memcpy((u8*)&pData_iap[FRM_DATA +ackdata[0]*8], &pData[FRM_DATA], 8);
//		
//		if(ackdata[0]== 15)
//		{
//			pData_iap[FRM_INDEX] = ackdata[1];
//			pData_iap[FRM_LEN] = 128;
//			if(pData[FRM_DST_ID] == MY_ID)
//			{
//				if(pData[FRM_CMD] == CMD_IAP_WR)
//					IAP_ParseFrame(pData_iap, Channel);
//				else
//				{
//					//写语音索引偏移量是16，第一帧128byte索引是0，第二帧16，经过中继板
//					VoiceDownload(Channel, pData_iap); //串口每次发送128byte
//					ackdata[0] = 0;
//					PushMiniFrame(pData[FRM_DST_ID], pData[FRM_SRC_ID], 0, CMD_SPK_ACK, ackdata[0], 0, Channel);
//				}
//			}
//			else
//			{
//				PushMiniFrame(pData[FRM_SRC_ID], pData[FRM_DST_ID], pData_iap[FRM_LEN], pData_iap[FRM_CMD], BMS_IAP_Index, &pData_iap[FRM_DATA], COM_BMS);
//				if(BMS_IAP_Index == 0xFF)
//				{
//					BMS_IAP_Index = 0;
//				}
//				else
//					BMS_IAP_Index++;
//			}
//		}
//		else
//		{
//			if(pData[FRM_CMD] == CMD_IAP_WR)
//				Ack_cmd = CMD_IAP_ACK;
//			else
//				Ack_cmd = CMD_SPK_ACK;
//			ackdata[0] = 0;
//			PushMiniFrame(pData[FRM_DST_ID], pData[FRM_SRC_ID], 0, Ack_cmd, ackdata[0], 0, Channel);
//		}
//	}
//	else
//	{
//		if(pData[FRM_CMD] == CMD_IAP_BEGIN)
//			BMS_IAP_Index = 0;

//		if(pData[FRM_DST_ID] == MY_ID)
//		{
//			if(pData[FRM_CMD] >= CMD_IAP_BEGIN && pData[FRM_CMD] <= CMD_IAP_RESET)
//				IAP_ParseFrame(pData, Channel);
//			else
//				VoiceDownload(Channel, pData); //串口每次发送128byte
//		}  
//		else
//			PushMiniFrame(pData[FRM_SRC_ID], pData[FRM_DST_ID], pData[FRM_LEN], pData[FRM_CMD], pData[FRM_INDEX], &pData[FRM_DATA], COM_BMS);
//	}
//}

///*****************************************************************
//* Function Name :  ParseMySelfFrame
//* Description   :  解析通过CAN发给自己（仪表板）的数据
//* Input         :  pData：要解析的数据
//* Output        :  None
//* Notes         :
//******************************************************************/
//void ParseMySelfFrame(u8 Channel, u8* pData)
//{
//	static u8 Ack_Dst_ID = 0;  //回复读写指令的ID
//	if(pData[FRM_SRC_ID] == EBIKE_CABLELOCK_ID || pData[FRM_SRC_ID] == EBIKE_ECU_ID)
//	{
//		if(pData[FRM_SRC_ID] == EBIKE_CABLELOCK_ID)
//			g_bool[B_RELAY_ACK] = 1;     //跟中继板有通讯
//		Ack_Dst_ID = MY_ID;              //回复CAN总线上控制器的读写指令，源地址目标地址都是自己
//	}
//	else
//		Ack_Dst_ID = pData[FRM_SRC_ID];  //回复非CAN总线上控制器的读写指令，目标地址是原源地址 
//	
//	switch(pData[FRM_CMD])		//判断指令
//	{
//	case CMD_RD:
//		switch(pData[FRM_INDEX])
//		{
//		case DIS_VERSION_SW:	
//			PushMiniFrame(MY_ID, Ack_Dst_ID, DIS_VERSION_SW_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_myself_data.DIS_Version_sw, CAN_SEL);
//			break;
//		case DIS_VERSION_HW:	
//			PushMiniFrame(MY_ID, Ack_Dst_ID, DIS_VERSION_HW_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_myself_data.DIS_Version_hw, CAN_SEL);
//			break;

//		case BMS_SN_L:
//			PushMiniFrame(MY_ID, Ack_Dst_ID, BMS_SN_L_LENTH, CMD_ACK_RD, pData[FRM_INDEX], g_stBMS_Info.sn, CAN_SEL);
//			break;
//		
//		case BMS_SN_H:
//			PushMiniFrame(MY_ID, Ack_Dst_ID, BMS_SN_H_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_stBMS_Info.sn[BMS_SN_L_LENTH], CAN_SEL);
//			break;
//		
//		case DIS_PN_L:	
//			PushMiniFrame(MY_ID, Ack_Dst_ID, DIS_PN_L_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_myself_data.DIS_PN[0], CAN_SEL);
//			break;
//		
//		case DIS_PN_H:	
//			PushMiniFrame(MY_ID, Ack_Dst_ID, DIS_PN_H_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_myself_data.DIS_PN[DIS_PN_L_LENTH], CAN_SEL);
//			break;
//		
//		case BMS_DATA:
//			PushMiniFrame(MY_ID, Ack_Dst_ID, BMS_DATA_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_myself_data.BMS_Data, CAN_SEL);
//			break;
//		
//		case BMS_TEMP:
//			PushMiniFrame(MY_ID, Ack_Dst_ID, BMS_TEMP_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_myself_data.BMS_Temp, CAN_SEL);
//			break;
//		
//		case DIS_CPID_L:
//			PushMiniFrame(MY_ID, Ack_Dst_ID, DIS_CPID_L_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)g_myself_data.DIS_CPID.Data16, CAN_SEL);
//			break;
//		
//		case DIS_CPID_H:
//			PushMiniFrame(MY_ID, Ack_Dst_ID, DIS_CPID_H_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)(g_myself_data.DIS_CPID.Data16)+8, CAN_SEL);
//			break;
//		
//		case NFC_CARD_INFO_L:
//			PushMiniFrame(MY_ID, Ack_Dst_ID, NFC_CARD_INFO_L_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_myself_data.NFC_Card_Info.NFC_Card_Info_L[0], CAN_SEL);
//			break;
//		
//		case NFC_CARD_INFO_H:
//			PushMiniFrame(MY_ID, Ack_Dst_ID, NFC_CARD_INFO_H_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_myself_data.NFC_Card_Info.NFC_Card_Info_H[0], CAN_SEL);
//			break;
//		
//		case VIRTUAL_QRCODE_L:
//			PushMiniFrame(MY_ID, Ack_Dst_ID, VIRTUAL_QRCODE_L_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_myself_data.QRCode_Info.Virtual_QRcode_L[0], CAN_SEL);
//			break;
//		
//		case VIRTUAL_QRCODE_H:
//			PushMiniFrame(MY_ID, Ack_Dst_ID, VIRTUAL_QRCODE_H_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_myself_data.QRCode_Info.Virtual_QRcode_H[0], CAN_SEL);
//			break;

//		case NFC_MODE_SET:
//			PushMiniFrame(MY_ID, Ack_Dst_ID, NFC_MODE_SET_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_myself_data.NFC_Audio_set, CAN_SEL);
//		  break;
//		
//		case FRONTLIGHT_SET:
//			PushMiniFrame(MY_ID, Ack_Dst_ID, FRONTLIGHT_SET_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_myself_data.Frontlight_Ctr, CAN_SEL);
//		  break;
//		
//		case AMBIENTLIGHT_SET:
//			PushMiniFrame(MY_ID, Ack_Dst_ID, AMBIENTLIGHT_SET_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_myself_data.Ambientlight_Ctr, CAN_SEL);
//		  break;
//		
//		case AUDIO_SET:
//			PushMiniFrame(MY_ID, Ack_Dst_ID, AUDIO_SET_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_myself_data.Audio_Set, CAN_SEL);
//		  break;
//		case PANEL_SET:
//			PushMiniFrame(MY_ID, Ack_Dst_ID, PANEL_SET_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_myself_data.Panel_Set, CAN_SEL);
//		  break;
//		case WIRELESSCHARGER_SET:
//			PushMiniFrame(MY_ID, Ack_Dst_ID, WIRELESSCHARGER_SET_LENTH, CMD_ACK_RD, pData[FRM_INDEX], (u8*)&g_myself_data.Wirelesscharger_Set, CAN_SEL);
//		  break;
//		
//		default:
//			break;
//		}
//		break;
//	case CMD_WR:
//		switch(pData[FRM_INDEX])
//		{
//		case AUDIO_SET:
//			if(pData[FRM_LEN] == AUDIO_SET_LENTH)
//				memcpy((u8*)&g_myself_data.Audio_Set, &pData[FRM_DATA], pData[FRM_LEN]);
//			break;	
//		case NFC_MODE_SET:
//			if(pData[FRM_LEN] == NFC_MODE_SET_LENTH)
//				memcpy((u8*)&g_myself_data.NFC_Audio_set, &pData[FRM_DATA], pData[FRM_LEN]);
//			break;
//		
//		case FRONTLIGHT_SET:
////			if(pData[FRM_LEN] == FRONTLIGHT_SET_LENTH)
//			{
//				memcpy((u8*)&g_myself_data.Frontlight_Ctr.LightCtrl, &pData[FRM_DATA], 2);
//			}
//			break;
//		
//		case AMBIENTLIGHT_SET:
//			if(pData[FRM_LEN] == AMBIENTLIGHT_SET_LENTH)
//				memcpy((u8*)&g_myself_data.Ambientlight_Ctr, &pData[FRM_DATA], pData[FRM_LEN]);
//			break;

//		case PANEL_SET:
//			if(pData[FRM_LEN] == PANEL_SET_LENTH)
//			{
//				memcpy((u8*)&g_myself_data.Panel_Set, &pData[FRM_DATA], pData[FRM_LEN]);
//				if(g_myself_data.Panel_Set.IOT_ERR >= 0xE0)
//					SetErrorByte(g_myself_data.Panel_Set.IOT_ERR, FromCAN_Err);
//			}
//			break;
//		
//		case WIRELESSCHARGER_SET:
//			if(pData[FRM_LEN] == WIRELESSCHARGER_SET_LENTH)
//				memcpy((u8*)&g_myself_data.Wirelesscharger_Set.PowerCtrl, &pData[FRM_DATA], pData[FRM_LEN]-1);
//			break;
//			
//		case VIRTUAL_QRCODE_L:
//			memcpy((u8*)&g_myself_data.QRCode_Info, &pData[FRM_DATA], pData[FRM_LEN]);
//			break;
//		
//		case VIRTUAL_QRCODE_H:
//			memcpy((u8*)&g_myself_data.QRCode_Info.Virtual_QRcode_H, &pData[FRM_DATA], pData[FRM_LEN]);
//			g_bool[B_SAVE_FDATA] = 1;
//			break;
//		
//		case DIS_PN_L:
//			memcpy((u8*)&PN_Wr.DIS_PNWRBUF[0], &pData[FRM_DATA], pData[FRM_LEN]);
//			g_bool[B_PN_WR_FLAG] |= 0x01;
//			break;
//		
//		case DIS_PN_H:
//			memcpy((u8*)&PN_Wr.DIS_PNWRBUF[DIS_PN_L_LENTH], &pData[FRM_DATA], pData[FRM_LEN]);
//			g_bool[B_PN_WR_FLAG] |= 0x02;
//			break;
//		
//		case DIS_PN_CRC:
//			PN_Wr.SRC_ID = pData[FRM_SRC_ID];
//			PN_Wr.DST_ID = pData[FRM_DST_ID];
//			PN_Wr.CMD = pData[FRM_CMD];
//			memcpy((u8*)&PN_Wr.DIS_PNWRBUF[DIS_PN_L_LENTH+DIS_PN_H_LENTH], &pData[FRM_DATA], pData[FRM_LEN]);
//			g_bool[B_PN_WR_FLAG] |= 0x04;
//			break;
//		
//		default:
//			break;
//		}
//
//		break;
//	case CMD_NFC_UPDATE: //上载NFC卡片信息返回
//		g_bool[B_NFC_UPDATE_OK] = 1;
//		break;
//	
//	case CMD_IAP_BEGIN:	    //IAP指令
//	case CMD_IAP_WR:
//	case CMD_IAP_CRC:
//	case CMD_IAP_RESET:

//	case CMD_SPK_ERASE:    //语音下载指令
//	case CMD_SPK_WR_DATA:
//	case CMD_SPK_VERIFY:
//		ParseIAPFrame(Channel, pData); //IAP及语音更新数据解析
//		break;
//	default:
//		break;
//	}
//}

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

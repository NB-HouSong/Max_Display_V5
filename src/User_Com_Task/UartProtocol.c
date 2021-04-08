#include "UartProtocol.h"

////公用指令类型宏定义
//#define CMD_RD					0x01		//读指令
//#define CMD_WR					0x02		//写指令
//#define CMD_WR_NR				0x03		//写控制表指令（无返回）
//#define CMD_ACK_RD			    0x04		//读返回指令
//#define CMD_ACK_WR			    0x05		//写返回指令
////数据下载相关指令
//#define CMD_IAP_BEGIN			0x07		//IAP开启指令
//#define CMD_IAP_WR				0x08		//IAP写固件指令
//#define CMD_IAP_CRC				0x09		//IAP校验固件指令
//#define CMD_IAP_RESET			0x0A		//系统复位指令
//#define CMD_IAP_ACK				0x0B		//应答帧

//#define	CMD_WRERROR_OK			0x00		//写入成功
//#define	CMD_WRERROR_AUTH		0x01		//写入地址没有写入权限
//#define	CMD_WRERROR_BUSY		0x02		//正在操作控制表，不允许写入
//#define	CMD_WRERROR_SIZE		0x03		//写入数据超范围
//#define	CMD_WRERROR_FORM		0x04		//写入格式错误




void (*scooterUpDataHandler)(const uint8_t *pData);
void (*scooterUpgradeDataHandler)(const uint8_t *pData);

static void usartProtocolHandleData(const uint8_t *pData)
{
    uint16_t srcID = 0, dstID, cmd;
    srcID = pData[FRM_SRC_ID];
    dstID = pData[FRM_DST_ID];

    if(dstID == MY_ID)
    {
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
                IAP_ParseFrame((uint8_t *)pData, 0);
                break;
            default:break;
        }
        return;
    }
}

void usartProtocolInitialize(void)
{
    scooterDataSetCallback(usartProtocolHandleData);
}

//void scooterUpDataSetCallback(void (*dataHandler)(const uint8_t *pData))
//{
//    scooterUpDataHandler = dataHandler;
//}

//void scooterUpgradeDataSetCallback(void (*dataHandler)(const uint8_t *pData))
//{
//    scooterUpgradeDataHandler = dataHandler;
//}

//void usartProtocolIapResponse(uint8_t *message)
//{

//}

void usartProtocolToScooterResponse(UsartMessage *message)
{
    UART3_PushFrame(message->srcId, message->dstId, message->dataSize, message->cmd, message->index, message->data);
}

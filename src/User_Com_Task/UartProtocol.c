#include "UartProtocol.h"

////����ָ�����ͺ궨��
//#define CMD_RD					0x01		//��ָ��
//#define CMD_WR					0x02		//дָ��
//#define CMD_WR_NR				0x03		//д���Ʊ�ָ��޷��أ�
//#define CMD_ACK_RD			    0x04		//������ָ��
//#define CMD_ACK_WR			    0x05		//д����ָ��
////�����������ָ��
//#define CMD_IAP_BEGIN			0x07		//IAP����ָ��
//#define CMD_IAP_WR				0x08		//IAPд�̼�ָ��
//#define CMD_IAP_CRC				0x09		//IAPУ��̼�ָ��
//#define CMD_IAP_RESET			0x0A		//ϵͳ��λָ��
//#define CMD_IAP_ACK				0x0B		//Ӧ��֡

//#define	CMD_WRERROR_OK			0x00		//д��ɹ�
//#define	CMD_WRERROR_AUTH		0x01		//д���ַû��д��Ȩ��
//#define	CMD_WRERROR_BUSY		0x02		//���ڲ������Ʊ�������д��
//#define	CMD_WRERROR_SIZE		0x03		//д�����ݳ���Χ
//#define	CMD_WRERROR_FORM		0x04		//д���ʽ����




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
                //���»��峵���ݣ��ص���������
				memcpy((u8*)&g_myself_data.Scooter_Info, &pData[FRM_DATA], pData[FRM_LEN]);
                break;
            case CMD_IAP_BEGIN:
            case CMD_IAP_WR:
            case CMD_IAP_CRC:
            case CMD_IAP_RESET:
                //�Ǳ��������ݣ��ص���������
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

/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : variable.h
* Author        : ������
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _VARIBALE_H
#define _VARIBALE_H

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"/* FreeRTOS includes. */
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

#include "LED_Others.h"
#include "SPI_FLASH.h"
#include "READER_API.h"
#include "spi.h"
#include "Flash.h"
#include "Universal_Init.h"
#include "Control.h"
#include "System_Config.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define ABS(a)	((a)<0?-(a):(a))
#define MAX(x,y) (((x)>(y))?(x):(y))
#define MIN(x,y) (((x)<(y))?(x):(y))
#define AVG(x,y) (((x)+(y))/2)
#define LIMIT(a, l, h)	(a > h ? h : (a < l ? l : a))

#define DEBUG_1

#define B_FLASH_CATCH           0x7FFF
#define B_FLASH_MAX_WR_CNT      6000
#define ERASE_FAILURE           0x01
#define WRITE_FAILURE           0x02

//�ڴ���Ʊ���غ궨�� ��Ҫflash���������
#define NB_CMDMAP_LEN		    16	//�ڴ���Ʊ��ܳ��ȣ�int16��λ��
#define NB_Virtual_QRcode       0   //NFC�����ά��  16byte
#define NB_DIS_PN               8   //PN�� 14byte
#define NB_INF_FLASH_CNT_RECORD NB_CMDMAP_LEN-1 //IOT��ϵͳ����������16Byte���ڴ�����һ��Ԫ����flash��д���� 

//�̼��汾��
#define FW_VERSION			    (u16)(0x100)

//��ģ��ID��غ궨��

#define ECU_ID		              0x20   //����ID
#define DIS_ID	                  0x2B   //�Ǳ�ID
#define ID_UART_DEBUGGER		  0x3D

#define MY_ID		              DIS_ID	 //����ID

//����ָ�����ͺ궨��
#define CMD_RD					  0x01	//��ָ��
#define CMD_WR					  0x02	//дָ��
#define CMD_ACK_RD			      0x04	//������ָ��
#define CMD_ACK_WR			      0x05	//д����ָ��

//IAP�������ָ��
#define CMD_IAP_BEGIN			  0x07	//IAP����ָ��
#define CMD_IAP_WR				  0x08	//IAPд�̼�ָ��
#define CMD_IAP_CRC				  0x09	//IAPУ��̼�ָ��
#define CMD_IAP_RESET			  0x0A	//ϵͳ��λָ��
#define CMD_IAP_ACK				  0x0B	//Ӧ��֡

#define CMD_SCO_CTL               0x64  //scooterר�ã����ݶ�ֻ�������ź�ɲ���ȿ������ݣ�������Ҫ���ص�����ǰ�ƿ��Ƶ�����
#define CMD_SCO_CTL_NR            0x65  //scooterר�ã����ݶ�ֻ�������ź�ɲ���ȿ������ݣ��������ط�����Ϣ

//VOI���ƹ��ܣ���ȡ�Ǳ�ID
#define  CMD_IAP_RD               0X79  //Voi����
#define  CMD_IAP_RD_ACK           0X7A

//NFC���ݸ�������
#define CMD_NFC_UPDATE            0xE0  //����NFC��Ϣ




//�����غ궨��
#define LED_OFF      0
#define LED_ON       1
#define LED_Flash    2
#define LED_Breath   3

#define COM1MSG_Q_NUM           256


//�����붨��
#define MINI_ERROR_MCU_COM	              51   //�������Ǳ�ͨ���쳣


//����������غ궨��
#define B_LEN				  26  //������������
#define B_BGN_LOOP			  0	  //�Ƿ������ѭ��
//#define B_LOCKED			  1	  //�Ƿ�����
//#define B_VOICE_ING		  2	  //���ڲ�������
#define B_SAVE_FDATA		  3	  //�Ƿ񱣴��ڴ��
//#define B_ALARMING		  4	  //�Ƿ����ڱ���
#define B_IN_IAPMODE		  5	  //�Ƿ�IAPģʽ
#define B_UPD_IAP			  6	  //�Ƿ�IAP�����Ѹ���
//#define B_LOWPOWER_MODE	  7	  //�Ƿ�͹���ģʽ
//#define B_TEST_MODE		  8	  //�Ƿ���߲���ģʽ
#define B_RESET				  9	  //�Ƿ�����
//#define B_IS_CHARGING		  10  //�Ƿ����ڳ��
//#define B_LED_CTRL		  11  //�Ǳ�ƿ���
#define B_ECU_ACK             12  //ECU���߱�ʶ
//#define	B_CTL_UNLOCK      13  //��������
//#define	B_CTL_LOCK        14  //��������
//#define	HEADLIGHT_ON      15  //��Ƶ�����ʶ
#define	B_NFC_LEARN_BOOL      16  //�Ƿ�ѧϰ
#define	B_WIRELESS_ERR        17  //���߳����
//#define	B_RUNNING_MODE_SHIFT  18  //����ģʽ�л�
#define	B_NFC_UPDATE_OK       19  //NFC��Ƭ��Ϣ�ϴ��ɹ�
//#define	B_SLEEP_MODE          20  //�Ƿ��������ģʽ
#define	B_TURN_LIGHT_FLASH    21  //ת���������˸
#define	B_HANDLE_TEMP_STATUS  22  //���׼���״̬

//������ָ��궨��
#define MINI_BEEP_CMD_ONCE_S	    ((unsigned char)0x01)		//����
#define MINI_BEEP_CMD_ONCE_L		((unsigned char)0x02)		//����
#define MINI_BEEP_CMD_DOUBLE_S	    ((unsigned char)0x03)		//��������
#define MINI_BEEP_CMD_ONCE_LL	    ((unsigned char)0x04)		//�ӳ���

// ���׼���״̬ö��
typedef enum
{
    HANDLE_TEMP_OFF         = 0x00,
    HANDLE_TEMP_ON          = 0x01,
    HANDLE_TEMP_DIFF_LARGE  = 0x02,
    HANDLE_TEMP_LEFT_BROKN  = 0x04,
    HANDLE_TEMP_RIGHT_BROKN = 0x08,
} HANDLE_TEMP_STATUS;

// ���߳�״̬ö��
typedef enum
{
    WIRELESS_STATUS_OFF     = 0x00,
    WIRELESS_STATUS_ON      = 0x01,
    WIRELESS_STATUS_ERROR   = 0x02,
    WIRELESS_STATUS_DIFF_VOL   = 0x04,
} WIRELESS_STATUS;

// ��Χ��ö��
typedef enum
{
    RGB_AMBIENT_DASHBOARD   = 0x00,
    RGB_AMBIENT_HANDLE      = 0x10,
} RGB_AMBIENT;

#pragma pack(1)
/* Exported types ------------------------------------------------------------*/
//��Χ�Ʋ����ṹ����
typedef struct 
{
    volatile u8 AmbientLightMode;           //��Χ�ƹ���ģʽ
	volatile u8 AmbientLightColor;          //��Χ����ɫ (1~7 , �ֱ��Ӧ7����ɫ)
    volatile u8 AmbientLightLux;            //��Χ������
	volatile u8 AmbientLight_Period;        //��Χ�ƺ���Ƶ��
    volatile u8 AmbientLight_Off_Intens;    //��Χ�ƹر�״̬
} AMBIENT_LED;

/*********************
//�Ǳ�������ض���
**********************/
//1.�Ǳ�����汾����Ϣ
typedef struct 
{
	volatile u16 Dis_SoftwareVersion; //�Ǳ�����汾��
	volatile u16 BMS_SoftwareVersion; //BMS����汾��
}DIS_VERSION_SW_INFO;

//2.�Ǳ�Ӳ���汾����Ϣ
typedef struct 
{
	volatile u16 Dis_HardwareVersion; //�Ǳ�Ӳ���汾��
	volatile u16 BMS_HardwareVersion; //BMSӲ���汾��
}DIS_VERSION_HW_INFO;

//2.�Ǳ�оƬΨһCPID
typedef union
{
	u32  Data32[3];
	s16  Data16[6];
}UNIQ_ID;

//3.NFC����Ϣ
typedef struct 
{
	volatile u8 NFC_Card_Info_L[8];
	volatile u8 NFC_Card_Info_H[8];
}NFC_CARD_INFO;

//4.�����ά����Ϣ
typedef struct 
{
	volatile u8 Virtual_QRcode_L[8];
	volatile u8 Virtual_QRcode_H[8];
}VIRTURE_QRCODE_INFO;

//5.ת����Ϣ
typedef struct 
{
    volatile u8 FrameLen;                   //֡����
	volatile u8 GasValue;                   //����ֵ
	volatile u8 LeftBreakValue;             //��ɲֵ
    volatile u8 Bool;                       //boolֵ
    
    volatile u8 Beep;                       //������״̬
    volatile u8 Version_A;                  //�̼��汾
    volatile u8 Version_B;                  //�̼��汾
	volatile u8 HW_Version;                 //Ӳ���汾
    
	volatile u8 RightBreakValue;            //��ɲ
    volatile WIRELESS_STATUS WirelessChargerStatus;      //���߳�״̬     (0:�ر� 1:��)
    volatile HANDLE_TEMP_STATUS HandleTempStatus;           //���׼���״̬  (�ο� HANDLE_TEMP_STATUS����)
    volatile s8 HandleLeftTemp;             //������¶�
    
    volatile s8 HandleRightTemp;            //�Ұ����¶�
    volatile u8 TurnSignalLightStatus;      //ת���״̬
} HANDLE_BAR_INFO;


//6.���峵��Ϣ
typedef struct 
{
	volatile u8 FrontlightBrightness;       //ǰ������
	volatile u8 A_LED;                      //A LED״̬
	volatile u8 B_LED;                      //B LED״̬
	volatile u8 C_LED;                      //C LED״̬
    
	volatile u8 D_LED;                      //D LED״̬
	volatile u8 AmbientLightMode;           //��Χ�ƹ�װ״̬(ǰ4λ��Χ��(0���Ǳ�; 1:���; 2:�Ұ�), ����λת���ģʽ(0���ر� 1������ 2����˸ 3������) )
	volatile u8 AmbientLightColor;          //��Χ����ɫ (1~7 , �ֱ��Ӧ7����ɫ)
    volatile u8 AmbientLightLux;            //��Χ������
    
	volatile u8 AmbientLight_Period;        //��Χ�ƺ���Ƶ��
    volatile u8 AmbientLight_Off_Intens;    //��Χ�ƹر�״̬
    volatile u8 BeepTime;                   //����������ʱ��
    volatile u8 TurnSigStatus;              //ת��ƹ���״̬
    
    volatile u8 ControllerStatus;           //����״̬��0:���� 1:������
    volatile u8 WirelessChargerSet;         //���߳����ã�0:�ر� 1:�򿪣�
    
    volatile u8 HandleTempMode;             //���׽���ģʽ��0���رգ�1:�򿪣�
    volatile u8 HandleTempSetValue;         //���׼����趨�¶�ֵ��16~30�ȣ�
    
    volatile u8 GpsPowerSet;                //���׼����趨�¶�ֵ��16~30�ȣ�
}SCOOTER_INFO;


//�Ǳ����ݽṹ�嶨��
typedef struct 
{
    u8 CommuTimeout;                          // ���峵ͨ�ų�ʱ  
	DIS_VERSION_SW_INFO DIS_Version_sw;       //1.�Ǳ�����汾����Ϣ
	DIS_VERSION_HW_INFO DIS_Version_hw;       //2.�Ǳ�Ӳ���汾����Ϣ
	volatile u8 DIS_PN[14];                   //3.�Ǳ� PN��
	volatile UNIQ_ID DIS_CPID;                //4.�Ǳ�оƬΨһCPID
    NFC_CARD_INFO NFC_Card_Info;              //5.NFC����Ϣ
	VIRTURE_QRCODE_INFO QRCode_Info;          //6.�����ά����Ϣ	
	HANDLE_BAR_INFO Handle_Bar_Info;          //7.ת����Ϣ
    SCOOTER_INFO    Scooter_Info;             //8.���峵����״̬
    AMBIENT_LED     Ambient_Light;            //9. ���̷�Χ��
    AMBIENT_LED     RGB_Led;                  //10. ����ת�ѷ�Χ��
}MYSELF_DATA;

#pragma pack()

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
extern volatile u8 g_bool[B_LEN];		//��������
extern volatile u16 g_Clear_error_cnt;  //�����������
extern volatile u8 g_ErrorByte;
extern volatile u8 g_wireless_charger_vol; 
extern volatile u8 g_12V_vol;
extern volatile u16 g_leftBreakGnd_vol;
extern volatile u16 g_rightBreakGnd_vol;
extern volatile u16 g_accGnd_vol;
extern QueueHandle_t COM1_Message_Queue;
extern MYSELF_DATA g_myself_data;
//extern MC_RUNNING_DATA MC_Running_data;
extern volatile s16 g_NBCmdMap[NB_CMDMAP_LEN];	//�ڴ���Ʊ�
//extern PN_WR_STR PN_Wr;

/* Exported functions ------------------------------------------------------- */
void COM1_Task(void *pvParameters);

#endif 

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

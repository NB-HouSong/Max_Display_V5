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
#define DIS_ID	                  0x21   //�Ǳ�ID


#define MY_ID		              DIS_ID	 //����ID

//����ָ�����ͺ궨��
#define CMD_RD					  0x01	//��ָ��
#define CMD_WR					  0x02	//дָ��
#define CMD_ACK_RD			      0x04	//������ָ��
#define CMD_ACK_WR			      0x05	//д����ָ��
#define CMD_SCO_CTL               0x64  //scooterר�ã����ݶ�ֻ�������ź�ɲ���ȿ������ݣ�������Ҫ���ص�����ǰ�ƿ��Ƶ�����
#define CMD_SCO_CTL_NR            0x65  //scooterר�ã����ݶ�ֻ�������ź�ɲ���ȿ������ݣ��������ط�����Ϣ
#define CMD_HEARTBEAT             0xE0  //��IOT֮�������
#define CMD_NFC_UPDATE            0xE1  //����NFC��Ϣ

//IAP�������ָ��
#define CMD_IAP_BEGIN			  0x07	//IAP����ָ��
#define CMD_IAP_WR				  0x08	//IAPд�̼�ָ��
#define CMD_IAP_CRC				  0x09	//IAPУ��̼�ָ��
#define CMD_IAP_RESET			  0x0A	//ϵͳ��λָ��
#define CMD_IAP_ACK				  0x0B	//Ӧ��֡

////�����������ָ��
//#define CMD_SPK_ERASE			  0x12
//#define CMD_SPK_WR_DATA			  0x13
//#define CMD_SPK_READ			  0x14
//#define	CMD_SPK_VERIFY			  0x15
//#define CMD_SPK_ACK				  0x16

////BMS��غ궨��
//#define CMD_SCO_BASE64UNLOCK_IAP     0x99   //��IAPָ��
//#define CMD_UPDATE_BMS_VERIFY_CODE   0x80   //BMS CRC
//#define CMD_BMS_POWEROFF		     0x54	//��BMSָ��
//#define INDEX_BMS_POWEROFF		     0x7A	//��BMS����

////���������궨��
//#define LOCKSTATE_UNLOCK     0
//#define LOCKSTATE_LOCK       1

//#define LOCKCMD_UNLOCK       1
//#define LOCKCMD_LOCK         2

//�����غ궨��
#define LED_OFF      0
#define LED_ON       1
#define LED_Flash    2
#define LED_Breath   3

#define Level_0  0
#define Level_1  1
#define Level_2  2
#define Level_3  3

#define COM1MSG_Q_NUM           256

//������궨��
#define	ALARM_MARK_SHOCK1                 1    //�𶯱���
#define ALARM_MARK_SPEED			  	  2	   //�ֶ�����
#define ALARM_MARK_FALL			  	      3	   //�㵹����
#define ALARM_MARK_CRASH			  	  4	   //ˤ������
#define ALARM_MARK_BMS_TEMPH			  5    //��ظ��±���
#define ALARM_MARK_BMS_TEMPL			  6    //��ص��±���
#define VEHICLE_ALARM_BAT_TEMP            7    //MOS/CORE �¸��쳣
#define VEHICLE_ALARM_MOS_HIGH_TEMP       8    //MOS���±���
#define VEHICLE_ALARM_MOTOR_HIGH_TEMP     9    //MOTOR���±���
#define VEHICLE_ALARM_OVER_LOAD           10   //����
#define VEHICLE_ALARM_OVER_VOLTAGE        11   //��ѹ

//�����봦����غ궨��
#define MySelf_Err     1  //�������
#define FromCAN_Err    2  //����CAN���ߵ��������ӵĹ���

//�����붨��
#define MINI_ERROR_MCU_COM	              51   //�������Ǳ�ͨ���쳣
//#define MINI_BIKE_ERROR_ADC_PHA           11   //���1A����������쳣
//#define MINI_BIKE_ERROR_ADC_PHB           12   //���1B����������쳣
//#define MINI_BIKE_ERROR_ADC_PHC           13   //���1C����������쳣
//#define MINI_BIKE_ERROR_GAS_HALL          14   //���Ż����쳣
//#define MINI_BIKE_ERROR_EBS_HALL          15   //ɲ��һ�����쳣
//#define MINI_BIKE_ERROR_EBS2_HALL         16   //ɲ�Ѷ������쳣                                          
//#define MINI_BIKE_ERROR_DRIVER_PARA_SAVE  17   //�����������ñ������
//#define MINI_BIKE_ERROR_MOTOR_HALL        18   //���Hall�쳣
//#define MINI_BIKE_ERROR_SYS_VOL           19   //ϵͳ��ѹ�쳣
//#define MINI_BMS1_ERROR_INIT_SN           20   //BMSĬ�����к�
//#define MINI_BMS1_ERROR_COM               21   //BMSͨѶ����
//#define MINI_BMS1_ERROR_PASSWORD          22   //BMS�������
//#define MINI_BMS1_ERROR_MOS_TEMP          23   //BMS MOS�¶ȴ������쳣
//#define MINI_BAT1_ERROR_CELL_TEMP         24   //��ص�о�¶ȴ������쳣
//#define MINI_BMS1_ERROR_VOL               25   //��ص�ѹ�쳣
//#define MINI_BMS1_VERIFY_FAILUE           26   //������֤ʧ��
//#define MINI_BIKE_ERROR_FLASH_SAVE        27   //FLASH�������
//#define MINI_BIKE_ERROR_CHARGE_SOCKET     28   //��������ϻ����߹���
////#define MINI_BIKE_ERROR_                  29   //��������ϻ����߹���
//#define MINI_BIKE_ERROR_INIT_SN           30   //������Ĭ�����к�
//#define MINI_BIKE_ERROR_PASSWORD          31   //�����������
////#define MINI_BIKE_ERROR_                  32          
//#define MINI_BIKE_ERROR_MOS_TEMP          33   //�������¶��쳣
//#define MINI_BIKE_ERROR_MOTOR_TEMP        34   //����¶��쳣
//#define MINI_ERROR_IMU_COMMU              35   //��̬������ͨѶ����
//#define MINI_ERROR_GYRO                   36   //�����������쳣
//#define MINI_TORQ_ERROR_COMMU             37   //���ش�����ͨѶ�쳣      
//#define MINI_TORQ_ERROR_TORQ_VALUE        38   //���ش������쳣
//#define MINI_PASEL_ERROR_FRENCY           39   //̤Ƶ�������쳣
//#define MINI_SPEED_SENSOR_ERROR           40   //�ٶȴ������쳣
//#define MINI_BATTERY_LOCK_ERROR_COM       41   //����ͨѶ����
//#define MINI_BATTERY_LOCK_ID_NOT_MATCH    42   //����������ID��ƥ��
//#define MINI_BATTERY_LOCK_OPENED_STATE    43   //�ǰ����򿪹���
//#define MINI_MONITOR_MCU_ERROR_COMMU      44   //���оƬͨѶ����
//#define MINI_ERROR_GPS_TIMEOUT            60   //IOTͨѶ��ʱ
//#define MINI_ERROR_IOT_ENCRYPTCOM         61   //IOT����ͨѶʧ��
//#define MINI_NFC_CAL_ERROR		          62   //nfcУ׼ʧ��
//#define MINI_ERROR_RELAY_COM	          63   //�м̰��Ǳ�ͨ���쳣
//#define	MINI_ERROR_CODE_INIT_PN		      64   //�Ǳ���Ĭ��PN��

//������ſ���
#define VOICE_LOCK				          1    //����
#define VOICE_UNLOCK		              2    //����
#define VOICE_BATIN				          3    //�����أ�ϵͳ�ϵ磩���������ʾ
#define VOICE_SPEED				          4    //�ֶ�����
#define VOICE_ERROR				          5    //����
#define VOICE_ALARM				          6    //�������㵹����/ˤ��������
#define VOICE_LOCATE                      7    //Ѱ��
#define VOICE_SWITCH                      8    //ģʽ�л�
#define VOICE_OTA_SUCCESS                 9    //OTA�ɹ� Ԥ��
#define VOICE_NFC_READ_SUCCESS            10   //NFC�����ɹ�
#define VOICE_NFC_READ_FAILED             11   //NFC����ʧ��
#define VOICE_COVERLOCK_UNLOCK			  12   //�ǰ�����
#define VOICE_COVERLOCK_LOCK			  13   //�ǰ����ر�
#define VOICE_CABLELOCK_UNLOCK			  14   //��������
#define VOICE_CABLELOCK_LOCK			  15   //�������ر�
#define VOIC_CHARGE_COMPLETE			  16   //������

//����������غ궨��
#define B_LEN				  26  //������������
#define B_BGN_LOOP			  0	  //�Ƿ������ѭ��
#define B_LOCKED			  1	  //�Ƿ�����
#define B_VOICE_ING			  2	  //���ڲ�������
#define B_SAVE_FDATA		  3	  //�Ƿ񱣴��ڴ��
#define B_ALARMING			  4	  //�Ƿ����ڱ���
#define B_IN_IAPMODE		  5	  //�Ƿ�IAPģʽ
#define B_UPD_IAP			  6	  //�Ƿ�IAP�����Ѹ���
#define B_LOWPOWER_MODE		  7	  //�Ƿ�͹���ģʽ
#define B_TEST_MODE			  8	  //�Ƿ���߲���ģʽ
#define B_RESET				  9	  //�Ƿ�����
#define B_IS_CHARGING		  10  //�Ƿ����ڳ��
#define B_LED_CTRL			  11  //�Ǳ�ƿ���
#define B_ECU_ACK             12  //ECU���߱�ʶ
#define	B_CTL_UNLOCK          13  //��������
#define	B_CTL_LOCK            14  //��������
#define	HEADLIGHT_ON          15  //��Ƶ�����ʶ
#define	B_NFC_LEARN_BOOL      16  //�Ƿ�ѧϰ
#define	B_WIRELESS_ERR        17  //���߳����
#define	B_RUNNING_MODE_SHIFT  18  //����ģʽ�л�
#define	B_NFC_UPDATE_OK       19  //NFC��Ƭ��Ϣ�ϴ��ɹ�
#define	B_SLEEP_MODE          20  //�Ƿ��������ģʽ
#define B_RELAY_ACK           21  //�м̰����߱�ʶ
#define B_PN_WR_FLAG          22  //дPN���־
#define B_INIT_SHOW_OK        23  //���������������
#define B_ANGLE_READ_ACK      24  //���Ƕȷ���
#define B_READ_MC_ACK         25  //�����������ò�������

/*********************
//�Ǳ�����������غ궨��
**********************/
//Ϊ�˽�NFCˢ������������ָ��͸��м̰�
#define NFC_VEHICLE_LOCK    0x02    //NFCˢ������
#define NFC_VEHICLE_UNLOCK  0x03    //NFCˢ������

//���ݿ��ַ����
#define DIS_VERSION_SW             0x01   //4�ֽ� ����汾֡ u16*2
#define DIS_VERSION_HW             0x02   //4�ֽ� Ӳ���汾֡ u16*2
#define NFC_CARD_INFO_L            0x04   //8�ֽ� NFC����Ϣ��8�ֽ�
#define NFC_CARD_INFO_H            0x05   //8�ֽ� NFC����Ϣ��8�ֽ�
#define VIRTUAL_QRCODE_L           0x06   //8�ֽ� �����ά����Ϣ��8�ֽ�
#define VIRTUAL_QRCODE_H           0x07   //8�ֽ� �����ά����Ϣ��8�ֽ�
#define NFC_MODE_SET               0x08   //6�ֽ� NFCģʽ���� 0������ 1����Ƭ +��������
#define HANDLEBAR_INFO             0x09   //8�ֽ� ת����Ϣ
#define FRONTLIGHT_SET             0x0A   //4�ֽ� ǰ���
#define AMBIENTLIGHT_SET           0x0B   //4�ֽ� ��Χ������
#define AUDIO_SET                  0x0C   //3�ֽ� ��������
#define PANEL_SET                  0x0D   //5�ֽ� �����ʾ
#define WIRELESSCHARGER_SET        0x0E   //4�ֽ� ���߳�����

#define DIS_PN_L                   0x11   //8�ֽ� �Ǳ�PN���8λ
#define DIS_PN_H                   0x12   //6�ֽ� �Ǳ�PN���6λ
#define DIS_PN_CRC                 0x13   //4�ֽ� �Ǳ�PN��CRCУ����
#define BMS_SN_L                   0x14   //8�ֽ� BMS���кŵ�8�ֽ�
#define BMS_SN_H                   0x15   //6�ֽ� BMS���кŸ�6�ֽ�

#define BMS_DATA                   0x16   //7�ֽ� ��ص�����ѹ����Ϣ s16*3+u8
#define BMS_TEMP                   0x17   //6�ֽ� ����¶���Ϣ s8*6

#define DIS_CPID_L                 0xFA   //8�ֽ� CPID��8�ֽ�
#define DIS_CPID_H                 0xFB   //4�ֽ� CPID��4�ֽ�
#define PRIVATE_COMMKEY_L          0xFC   //8�ֽ� ͨѶ��Կ��8�ֽ�
#define PRIVATE_COMMKEY_H          0xFD   //8�ֽ� ͨѶ��Կ��8�ֽ�
#define PRIVATE_IAPKEY_L           0xFE   //8�ֽ� IAP��Կ��8�ֽ�
#define PRIVATE_IAPKEY_H           0xFF   //8�ֽ� IAP��Կ��8�ֽ�

//�����ݿ鳤��
#define DIS_VERSION_SW_LENTH       4   //4�ֽ� �Ǳ��BMS����汾֡ u16*2
#define DIS_VERSION_HW_LENTH       4   //4�ֽ� �Ǳ��BMSӲ���汾֡ u16*2
#define NFC_CARD_INFO_L_LENTH      8   //8�ֽ� NFC����Ϣ��8�ֽ�
#define NFC_CARD_INFO_H_LENTH      8   //8�ֽ� NFC����Ϣ��8�ֽ�
#define VIRTUAL_QRCODE_L_LENTH     8   //8�ֽ� �����ά����Ϣ��8�ֽ�
#define VIRTUAL_QRCODE_H_LENTH     8   //8�ֽ� �����ά����Ϣ��8�ֽ�
#define NFC_MODE_SET_LENTH         6   //6�ֽ� NFCģʽ����+��������
#define HANDLEBAR_INFO_LENTH       8   //8�ֽ� ת����Ϣ
#define FRONTLIGHT_SET_LENTH       4   //4�ֽ� ǰ���
#define AMBIENTLIGHT_SET_LENTH     4   //4�ֽ� ��Χ������
#define AUDIO_SET_LENTH            3   //3�ֽ� ��������
#define PANEL_SET_LENTH            5   //5�ֽ� �����ʾ
#define WIRELESSCHARGER_SET_LENTH  4   //4�ֽ� ���߳�����
#define BMS_SN_L_LENTH             8   //8�ֽ� BMS���кŵ�8�ֽ�
#define BMS_SN_H_LENTH             6   //6�ֽ� BMS���кŸ�6�ֽ�

#define DIS_PN_L_LENTH             8   //8�ֽ� �Ǳ�PN���8λ
#define DIS_PN_H_LENTH             6   //6�ֽ� �Ǳ�PN���6λ
#define DIS_PN_CRC_LENTH           4   //4�ֽ� �Ǳ�PN��CRCУ����

#define BMS_DATA_LENTH             7   //7�ֽ� ��ص�����ѹ����Ϣ s16*3+u8
#define BMS_TEMP_LENTH             6   //6�ֽ� ����¶���Ϣ s8*6

#define DIS_CPID_L_LENTH           8   //8�ֽ� CPID��8�ֽ�
#define DIS_CPID_H_LENTH           4   //4�ֽ� CPID��4�ֽ�
#define PRIVATE_COMMKEY_L_LENTH    8   //8�ֽ� ͨѶ��Կ��8�ֽ�
#define PRIVATE_COMMKEY_H_LENTH    8   //8�ֽ� ͨѶ��Կ��8�ֽ�
#define PRIVATE_IAPKEY_L_LENTH     8   //8�ֽ� IAP��Կ��8�ֽ�
#define PRIVATE_IAPKEY_H_LENTH     8   //8�ֽ� IAP��Կ��8�ֽ�

/*********************
//������������غ궨��
**********************/
//���������ݿ��ַ����
#define MC_POWER                    0x08   //8�ֽ� ��������������Ӳ���汾 s16*4
#define MC_ERR                      0x09   //2�ֽ� �����������뼰������ u8*2
#define MC_SPEED                    0x0A   //6�ֽ� �������ٶ������Ϣ s16*2+u8*2
#define MC_STATUS                   0x0B   //4�ֽ� ������״̬��Ϣ u8*4
#define MC_MOTER_TEMP               0x0C   //4�ֽ� ������������¶� s16*2
#define MC_TOTAL_TIME_MILES         0x0F   //8�ֽ� ������������̺���ʱ�� s16*4

#define MC_SET                      0x20   //8�ֽ� ���ز������� u16*2+u8*4

#define MC_ANGLE_SET_EN             0x22   //1�ֽ� �Ƕ�У׼ʹ�ܿ��� u8
#define MC_ANGLE_INFO               0x14   //1�ֽ� �Ƕ���Ϣ s32*2

#define VEHICLE_CTL_LOCK            0x30   //1�ֽ� ���� u8
#define VEHICLE_CTL_UNLOCK          0x31   //1�ֽ� ���� u8
#define VEHICLE_CTL_POWEROFF        0x32   //1�ֽ� �ػ� u8
#define VEHICLE_CTL_RESET           0x33   //1�ֽ� ���� u8
#define VEHICLE_CTL_SLEEP           0x34   //1�ֽ� ���� u8

//�����������ݿ鳤��
#define MC_POWER_LENTH              8   //8�ֽ� ��������������Ӳ���汾 s16*4
#define MC_ERR_LENTH                2   //2�ֽ� �����������뼰������ u8*2
#define MC_SPEED_LENTH              6   //6�ֽ� �������ٶ������Ϣ s16*2+u8*2
#define MC_STATUS_LENTH             4   //4�ֽ� ������״̬��Ϣ u8*4
#define MC_MOTER_TEMP_LENTH         4   //4�ֽ� ������������¶� s16*2
#define MC_TOTAL_TIME_MILES_LENTH   8   //8�ֽ� ������������̺���ʱ�� s16*4

#define MC_SET_LENTH                8   //8�ֽ� ���ز������� u16*2+u8*4

//������ָ��궨��
#define MINI_BEEP_CMD_ONCE_S	    ((unsigned char)0x01)		//����
#define MINI_BEEP_CMD_ONCE_L		((unsigned char)0x02)		//����
#define MINI_BEEP_CMD_DOUBLE_S	    ((unsigned char)0x03)		//��������
#define MINI_BEEP_CMD_ONCE_LL	    ((unsigned char)0x04)		//�ӳ���

#pragma pack(1)
/* Exported types ------------------------------------------------------------*/

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
    volatile u8 WirelessChargerStatus;      //���߳�״̬     (0:�ر� 1:��)
    volatile u8 HandleTempStatus;           //���׼���״̬   (0:�ر� 1:��)
    volatile u8 HandleLeftTemp;             //������¶�
    
    volatile u8 HandleRightTemp;            //�Ұ����¶�
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
}SCOOTER_INFO;

////6.ǰ���
//typedef struct 
//{
//	volatile u8 LightStatus; //ǰ���״̬ 0���� 1������ 2����˸
//	volatile u8 LightCtrl;   //ǰ��ƿ��� 0���� 1������ 2����˸
//	volatile u8 Brightness;  //��������   1~3,��ֵԽ������Խ��
//	volatile u8 Reserve;     //Ԥ��
//}FRONTLIGHT_INFO;

////7.��Χ������
//typedef struct 
//{
////	volatile u8 DisMode;     //��ʾģʽ 0���ر� 1������ 2����˸ 3������
////	volatile u8 Brightness;  //�������� 1~3,��ֵԽ������Խ��
////	volatile u8 Color;       //��ɫ���� 1~7 , �ֱ��Ӧ7����ɫ
////  volatile u8 EbikeMode;   //����ģʽ 0������ 1Exerciseģʽ��ɫ 2Eco����ģʽ��ɫ 3Turbo����ģʽ��ɫ 4����״̬��ɫ
//    volatile uint8_t lightMode;
//    volatile uint8_t lightColor;
//    volatile uint8_t lightLux;
//    volatile uint8_t lightPeriod;
//    volatile uint8_t lightIntens;
//    
//}AMBIENTLIGHT_INFO;

////8.��������
//typedef struct 
//{
////	volatile u8 AudioStatus; //����״̬ 0���޲��� 1�����ڲ���
//	volatile u8 AudioCtrl;   //�������� 0����  1������
//	volatile u8 VolumeNum;   //������� 1~12��Ӧ��ͬ������
//	volatile u8 VolumeLevel; //�������� 1~3��ֵԽ������Խ��
//}AUDIOSET_INFO;

////9.��ʾ������
//typedef struct
//{
//    uint8_t aLedControl;
//    uint8_t bLedControl;
//    uint8_t cLedControl;
//    uint8_t dLedControl;
//    
//    uint8_t turnSigControl;
//} PANELSET_INFO;

////10.���߳����
//typedef struct 
//{
//	volatile u8 ChargeStatus; //���״̬    0���ر� 1���� 
//	volatile u8 PowerCtrl;    //���߳����  0���ر� 1����
//	volatile u8 Reserve1;     //Ԥ��
//	volatile u8 Reserve2;     //Ԥ��
//}WIRELESSCHARGER_INFO;

////11.�������
//typedef struct 
//{
//	volatile s16 BMSVolt;  //��ص�ѹ
//	volatile s16 BMSCur;   //��طŵ����
//	volatile u16 BMSSoc;   //��ص���
//	volatile u8 Charging_State; //���״̬
//}BMS_DATA_INFO;

//14.IAP��Կ
typedef struct 
{
	volatile u8 Private_IAPkey_L[8];  //��8�ֽ�
	volatile u8 Private_IAPkey_H[8];  //��8�ֽ�
}PRIVATE_IAPKEY_INFO;

//typedef struct 
//{
//	volatile u8 VehicleNFCmodeSet;        //NFCģʽ����
//	struct Audio_SW AudioEnableConfig;    //Bit[0]:ʹ���ܿ��أ�0�رգ�1�����ţ�Bit[1:31]λ��ַ��Ӧ��Ƶlist������λ����0��Ƶ�ر� 1������
//	volatile u8 AudioVolumeConfig;        //��������
//}NFC_AUDIO_SET;

//�Ǳ����ݽṹ�嶨��
typedef struct 
{
    u8 CommuTimeout;                          // ���峵ͨ�ų�ʱ  
	DIS_VERSION_SW_INFO DIS_Version_sw;       //1.�Ǳ�����汾����Ϣ
	DIS_VERSION_HW_INFO DIS_Version_hw;       //�Ǳ�Ӳ���汾����Ϣ
	volatile u8 DIS_PN[14];                   //�Ǳ� PN��
	volatile UNIQ_ID DIS_CPID;                //2.�Ǳ�оƬΨһCPID
    NFC_CARD_INFO NFC_Card_Info;              //3.NFC����Ϣ
	VIRTURE_QRCODE_INFO QRCode_Info;          //4.�����ά����Ϣ	
	//NFC_AUDIO_SET NFC_Audio_set;              //NFCģʽ����������
	HANDLE_BAR_INFO Handle_Bar_Info;          //5.ת����Ϣ
    SCOOTER_INFO    Scooter_Info;             //6.���峵����״̬
	//FRONTLIGHT_INFO Frontlight_Ctr;           //6.ǰ���
	//AMBIENTLIGHT_INFO Ambientlight_Ctr;       //7.��Χ������
	//AUDIOSET_INFO Audio_Set;                  //8.��������
	//PANELSET_INFO Panel_Set;                  //9.��ʾ������
	//WIRELESSCHARGER_INFO Wirelesscharger_Set; //10.���߳����
	//BMS_DATA_INFO BMS_Data;                   //11.�������
	//BMS_TEMP_INFO BMS_Temp;                   //12.����¶�����
	//PRIVATE_COMMKEY_INFO Private_Comkey;      //13.ͨѶ��Կ
	//PRIVATE_IAPKEY_INFO Priver_IAPkey;        //14.IAP��Կ

}MYSELF_DATA;

/*********************
//������״̬������ض���
**********************/
//1.��������������
typedef struct 
{
	volatile s16 MC_Voltage;    //��������ѹ Unit��10mV
	volatile s16 MC_Current;    //���������� Unit��10mA
	volatile s16 MC_InputPower; //���������빦�� Unit��10mW
	volatile s16 Motor_Power;   //������� Unit��10mW
}MC_POWER_INFO;

//2.�������������뱨����
typedef struct 
{
	volatile u8 AlarmCode;    //������
	volatile u8 ErrCode;      //������
}MC_ERRCODE_INFO;

//3.�������ٶ�����
typedef struct 
{
	volatile s16 VehicleSpeed;     //�����ٶ�
	volatile u8 VehicleSpeedUnit; //�����ٶȵ�λ 0: KM/H 1:MPH
	volatile s16 MotorSpeed;       //����ٶ�
	volatile u8 MotorSpeedUnit;   //����ٶȵ�λ 0��rpm�� 1��elespeed; 2: km/h
}MC_SPEED_INFO;

//4.������״̬����
typedef struct 
{
	volatile u8 Null;                 //���״̬  Ŀǰ��Ч
	volatile u8 CoverlockStaus;       //�ǰ���״̬
	volatile u8 LockStatus;           //����״̬
	volatile u8 VehicleDrivingMode;   //����ģʽ����λ����0: ������ģʽ 1��ECO ģʽ 2:  Sportģʽ 3: ���綯ģʽ
}MC_STATUS_INFO;

//5.�������¶�����
typedef struct 
{
	volatile s16 MCTemp;           //�������¶�
	volatile s16 MotorTemp;        //����¶�
}MC_MOTER_TEMP_INFO;

//8.������ʱ�������������
typedef struct 
{
	volatile u16 Total_RunningTime_L;   //������ʱ��
	volatile u16 Total_RunningTime_H;   //������ʱ��
	volatile u16 TotalMiles_L;          //���������
	volatile u16 TotalMiles_H;          //���������
}MC_TOTAL_TIME_MILES_INFO;

//8.�Ƕ���Ϣ
typedef struct 
{
	volatile s32 ANGLE_ROAD;          //�����ĽǶ�
	volatile s32 ANGLE_REAL;          //��ʵ�Ƕ�
}MC_ANGLE1_INFO;

//9.������������Ϣ
typedef struct 
{
	volatile u8 VehicleAlarmSet;      //������������
	volatile u8 AlarmSensitivitySet;  //��������������
	volatile u8 SpeedUnitSet;         //�ٶȵ�λ
	volatile u16 MaxSpeedSet;         //����ٶ�����
	volatile u16 MaxPowerSet;         //���������
	volatile u8 DrivingMode;          //����ģʽ����
}MC_SET_INFO;

//���������ݽṹ�嶨��
typedef struct 
{
	MC_POWER_INFO MC_Power;           //1.��������������
	MC_ERRCODE_INFO MC_Errcode;       //2.�������������뱨����
	MC_SPEED_INFO MC_Speed;           //3.�������ٶ�����
	MC_STATUS_INFO MC_Status;         //4.������״̬����
	MC_MOTER_TEMP_INFO MC_Moter_Temp; //5.�������¶�����
	MC_TOTAL_TIME_MILES_INFO MC_Total_Time_Miles;//8.������ʱ�������������
	MC_ANGLE1_INFO MC_Angle;
	MC_SET_INFO MC_Set;
}MC_RUNNING_DATA;

typedef struct 
{
	volatile u8 DIS_PNWRBUF[18];                 //PN���ݴ�
	volatile u8 SRC_ID;
	volatile u8 DST_ID;
	volatile u8 CMD;
}PN_WR_STR;

#pragma pack()

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
extern volatile u8 g_bool[B_LEN];		//��������
extern volatile u16 g_Clear_error_cnt;  //�����������
extern volatile u8 g_ErrorByte;
extern volatile u8 g_wireless_charger_vol; 
extern volatile u8 g_12V_vol;
extern QueueHandle_t COM1_Message_Queue;
extern MYSELF_DATA g_myself_data;
extern MC_RUNNING_DATA MC_Running_data;
extern volatile s16 g_NBCmdMap[NB_CMDMAP_LEN];	//�ڴ���Ʊ�
extern PN_WR_STR PN_Wr;

/* Exported functions ------------------------------------------------------- */
void COM1_Task(void *pvParameters);

#endif 

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

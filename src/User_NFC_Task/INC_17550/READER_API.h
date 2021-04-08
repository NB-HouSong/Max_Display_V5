#ifndef _READERAPI_H_
#define _READERAPI_H_


#include "stm32f0xx.h"
#include "string.h"
#include "DEVICE_CFG.h"
#include "FM175XX_REG.h"
#include "FM175XX.h"
#include "MIFARE.h"
#include "DEFINE.h"
#include "Control.h"
#include "main.h"
#include "startup.h"
#include "REDEFINE.h"

static const unsigned char LPCD_GAIN[11] = {0x00,0x02,0x01,0x03,0x07,0x0B,0x0F,0x13,0x17,0x1B,0x1F};
static const unsigned char LPCD_P_DRIVER[7]={0x01,0x02,0x03,0x04,0x05,0x06,0x07};
static const unsigned char LPCD_N_DRIVER[7]={0x01,0x04,0x06,0x08,0x0A,0x0C,0x0F};

static const unsigned char RF_CMD_REQA = 0x26;
static const unsigned char RF_CMD_WUPA	= 0x52;
static const unsigned char RF_CMD_ANTICOL[3] = {0x93,0x95,0x97} ;
static const unsigned char RF_CMD_SELECT[3] = {0x93,0x95,0x97} ;

//发射参数设置
#define MODWIDTH_106 0x26 //106Kbps为0x26
#define MODWIDTH_212 0x13 //212kbps为0x13
#define MODWIDTH_424 0x09 //424kbps为0x09
#define MODWIDTH_848 0x04 //848kbps为0x04
//TYPE A
#define RXGAIN_A		6		//设置范围0~7
#define GSNON_A			15			//设置范围0~15
#define GSP_A 			31			//设置范围0~63
#define COLLLEVEL_A 4	//设置范围0~7
#define MINLEVEL_A  6	//设置范围0~15
//TYPE B
#define RXGAIN_B		6		//设置范围0~7        0x26      not use 7
#define GSNON_B			15			//设置范围0~15   0x27
#define MODGSNON_B 	8 	//设置范围0~15
#define GSP_B 			31			//设置范围0~63   0x28    0x20-0x3ff
#define MODGSP_B 		22		//设置范围0~63
#define MINLEVEL_B  6	    //设置范围0~15     0x18   

//天线校准指令相关
typedef struct
{
	volatile u8 SET_NFC_CALIBRATE;  //天线校准指令 1:有校准指令 1：无校准指令
	volatile u8 NFC_CLA_STATE;      //天线校准状态  返回  
}NFC_SET;

typedef struct
{
    u8 s_upload_timeout;
    u8 s_card_id_len;
    u8 s_send_cmd_id;
    u32 s_card_id;
}NFC_CARD_STRUCT;

struct picc_b_struct
{
unsigned char ATQB[12];
unsigned char	PUPI[4];
unsigned char	APPLICATION_DATA[4];
unsigned char	PROTOCOL_INF[3];
unsigned char ATTRIB[10];
unsigned char UID[8];	
};

extern struct picc_b_struct PICC_B; 

struct picc_a_struct
{
    unsigned char ATQA[2];
    unsigned char CASCADE_LEVEL;
    unsigned char UID[15];
    unsigned char BCC[3];
    unsigned char SAK[3];
    unsigned char Have_get_uid_status;
};

extern struct picc_a_struct PICC_A;
extern struct lpcd_struct Lpcd;
extern unsigned char succeed;
#define FM175XX_SUCCESS				0x00
#define FM175XX_LEARN_SUCCESS 0X01
#define FM175XX_CHECK_ERROR   0X02
#define FM175XX_RESET_ERR			0xF1
#define FM175XX_PARAM_ERR 		0xF2	//输入参数错误
#define FM175XX_TIMER_ERR			0xF3	//接收超时
#define FM175XX_COMM_ERR			0xF4	//
#define FM175XX_COLL_ERR			0xF5	//冲突错误
#define FM175XX_FIFO_ERR			0xF6	//FIFO
#define FM175XX_CRC_ERR				0xF7
#define FM175XX_PARITY_ERR		0xF8
#define FM175XX_PROTOCOL_ERR	0xF9

#define FM175XX_AUTH_ERR	0xE1

#define FM175XX_RATS_ERR 	0xD1
#define FM175XX_PPS_ERR 	0xD2
#define FM175XX_PCB_ERR 	0xD3
#define FM175XX_LEARN_ERR 	0xD4
typedef struct
{
	unsigned char Cmd;                 	// 命令代码
	unsigned char SendCRCEnable;
	unsigned char ReceiveCRCEnable;
	unsigned char nBitsToSend;					//准备发送的位数	
	unsigned char nBytesToSend;        	//准备发送的字节数
	unsigned char nBitsToReceive;					//准备接收的位数	
	unsigned char nBytesToReceive;			//准备接收的字节数	
	unsigned char nBytesReceived;      	// 已接收的字节数
	unsigned char nBitsReceived;       	// 已接收的位数
	unsigned char *pSendBuf;						//发送数据缓冲区
	unsigned char *pReceiveBuf;					//接收数据缓冲区
	unsigned char CollPos;             	// 碰撞位置
	unsigned char Error;								// 错误状态
	unsigned char Timeout;							//超时时间
} command_struct;
struct calibration_struct
{
 unsigned char Reference;//基准电容
 unsigned char Value;    //LPCD校准数值
 unsigned char Threshold_Max;//LPCD阈值
 unsigned char Threshold_Min;//LPCD阈值
 unsigned char Gain_Index;//LPCD校准Gain的序号
 unsigned char Driver_Index;	//LPCD校准Drive的序号
 unsigned char Range_L;	
 unsigned char Range_H;
};

struct calibration_backup_struct
{
 unsigned char Reference;//基准电容
 unsigned char Gain_Index;//LPCD校准Gain的序号
 unsigned char Driver_Index;	//LPCD校准Drive的序号
};

struct lpcd_struct
{
 unsigned char Timer1;//T1休眠时间
 unsigned char Timer2;//T2启动时间
 unsigned char Timer3;//T3发射时间
 unsigned char Timer3_Div ;//T3分频系数
 unsigned char Timer3_Offset;
 unsigned char TimerVmid;//VMID启动时间
 unsigned char Fullscale_Value;	//T3时间的满程数值
 unsigned char Value;	
 unsigned char Irq;//LPCD中断	
 struct calibration_struct Calibration;
 struct calibration_backup_struct Calibration_Backup;
};

extern NFC_SET NFC_set;
extern NFC_CARD_STRUCT  _my_id;
extern volatile u8 NFC_INT;

extern unsigned char SetTimeOut(unsigned int microseconds);
extern unsigned char Command_Execute(command_struct * comm_status);
extern void FM175XX_Initial_ReaderA(void);
extern void FM175XX_Initial_ReaderB(void);
extern unsigned char ReaderA_Halt(void);
extern unsigned char ReaderA_Request(void);
extern unsigned char ReaderA_Wakeup(void);
extern unsigned char s_error_cnt;
extern unsigned char ReaderA_AntiColl(unsigned char size);
extern unsigned char ReaderA_Select(unsigned char size);
extern unsigned char ReaderA_CardActivate(void);
extern void SetRf(unsigned char mode);
extern void SetTxCRC(unsigned char mode);
extern void SetRxCRC(unsigned char mode);
extern unsigned char ReaderB_Request(void);
unsigned char ReaderB_Wakeup(void);
extern unsigned char ReaderB_Attrib(void);
extern unsigned char ReaderB_GetUID(void);
extern unsigned char FM175XX_Polling(unsigned char *polling_card);
unsigned char TYPE_A_EVENT(void);
unsigned char TYPE_B_EVENT(void);
void Lpcd_Set_Mode(unsigned char mode);
unsigned char Lpcd_Init_Register(void);
unsigned char Lpcd_Reset_Status(void);//LPCD功能复位
void Lpcd_IRQ_Event(void);
unsigned char Lpcd_Set_Timer(void);
unsigned char Lpcd_Calibrate_Driver(void);
void Lpcd_Calibration_Restore(void);
unsigned char Lpcd_Calibration_Event(void);
void Lpcd_Calibration_Backup(void);
void LPCD_Read(void);
unsigned char Lpcd_Get_Value(unsigned char *value);
unsigned char Lpcd_Get_Calibration_Value(unsigned char *value);
u8 JUDGE(u8 *dat1,u8 *dat2,u8 len);
#endif


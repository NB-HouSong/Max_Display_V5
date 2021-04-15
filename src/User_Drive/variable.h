/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : variable.h
* Author        : 李美华
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

//内存控制表相关宏定义 需要flash保存的数据
#define NB_CMDMAP_LEN		    16	//内存控制表总长度（int16单位）
#define NB_Virtual_QRcode       0   //NFC虚拟二维码  16byte
#define NB_DIS_PN               8   //PN码 14byte
#define NB_INF_FLASH_CNT_RECORD NB_CMDMAP_LEN-1 //IOT对系统的设置数据16Byte，内存表最后一单元保存flash擦写次数 

//固件版本号
#define FW_VERSION			    (u16)(0x100)

//各模块ID相关宏定义

#define ECU_ID		              0x20   //主控ID
#define DIS_ID	                  0x2B   //仪表ID
#define ID_UART_DEBUGGER		  0x3D

#define MY_ID		              DIS_ID	 //自身ID

//公用指令类型宏定义
#define CMD_RD					  0x01	//读指令
#define CMD_WR					  0x02	//写指令
#define CMD_ACK_RD			      0x04	//读返回指令
#define CMD_ACK_WR			      0x05	//写返回指令

//IAP更新相关指令
#define CMD_IAP_BEGIN			  0x07	//IAP开启指令
#define CMD_IAP_WR				  0x08	//IAP写固件指令
#define CMD_IAP_CRC				  0x09	//IAP校验固件指令
#define CMD_IAP_RESET			  0x0A	//系统复位指令
#define CMD_IAP_ACK				  0x0B	//应答帧

#define CMD_SCO_CTL               0x64  //scooter专用，数据段只含有油门和刹车等控制数据，主控需要返回电量、前灯控制等数据
#define CMD_SCO_CTL_NR            0x65  //scooter专用，数据段只含有油门和刹车等控制数据，无需主控返回信息

//VOI定制功能，获取仪表ID
#define  CMD_IAP_RD               0X79  //Voi定制
#define  CMD_IAP_RD_ACK           0X7A

//NFC数据更新命令
#define CMD_NFC_UPDATE            0xE0  //上载NFC信息




//达等相关宏定义
#define LED_OFF      0
#define LED_ON       1
#define LED_Flash    2
#define LED_Breath   3

#define COM1MSG_Q_NUM           256


//故障码定义
#define MINI_ERROR_MCU_COM	              51   //控制器仪表通信异常


//布尔变量相关宏定义
#define B_LEN				  26  //布尔变量长度
#define B_BGN_LOOP			  0	  //是否进入主循环
//#define B_LOCKED			  1	  //是否锁车
//#define B_VOICE_ING		  2	  //正在播放声音
#define B_SAVE_FDATA		  3	  //是否保存内存表
//#define B_ALARMING		  4	  //是否正在报警
#define B_IN_IAPMODE		  5	  //是否IAP模式
#define B_UPD_IAP			  6	  //是否IAP数据已更新
//#define B_LOWPOWER_MODE	  7	  //是否低功耗模式
//#define B_TEST_MODE		  8	  //是否产线测试模式
#define B_RESET				  9	  //是否重启
//#define B_IS_CHARGING		  10  //是否正在充电
//#define B_LED_CTRL		  11  //仪表灯控制
#define B_ECU_ACK             12  //ECU离线标识
//#define	B_CTL_UNLOCK      13  //解锁语音
//#define	B_CTL_LOCK        14  //解锁语音
//#define	HEADLIGHT_ON      15  //大灯点亮标识
#define	B_NFC_LEARN_BOOL      16  //是否学习
#define	B_WIRELESS_ERR        17  //无线充故障
//#define	B_RUNNING_MODE_SHIFT  18  //助力模式切换
#define	B_NFC_UPDATE_OK       19  //NFC卡片信息上传成功
//#define	B_SLEEP_MODE          20  //是否进入休眠模式
#define	B_TURN_LIGHT_FLASH    21  //转向灯正在闪烁
#define	B_HANDLE_TEMP_STATUS  22  //把套加热状态

//蜂鸣器指令宏定义
#define MINI_BEEP_CMD_ONCE_S	    ((unsigned char)0x01)		//短响
#define MINI_BEEP_CMD_ONCE_L		((unsigned char)0x02)		//长响
#define MINI_BEEP_CMD_DOUBLE_S	    ((unsigned char)0x03)		//两声短响
#define MINI_BEEP_CMD_ONCE_LL	    ((unsigned char)0x04)		//加长响

// 把套加热状态枚举
typedef enum
{
    HANDLE_TEMP_OFF         = 0x00,
    HANDLE_TEMP_ON          = 0x01,
    HANDLE_TEMP_DIFF_LARGE  = 0x02,
    HANDLE_TEMP_LEFT_BROKN  = 0x04,
    HANDLE_TEMP_RIGHT_BROKN = 0x08,
} HANDLE_TEMP_STATUS;

// 无线充状态枚举
typedef enum
{
    WIRELESS_STATUS_OFF     = 0x00,
    WIRELESS_STATUS_ON      = 0x01,
    WIRELESS_STATUS_ERROR   = 0x02,
    WIRELESS_STATUS_DIFF_VOL   = 0x04,
} WIRELESS_STATUS;

// 氛围灯枚举
typedef enum
{
    RGB_AMBIENT_DASHBOARD   = 0x00,
    RGB_AMBIENT_HANDLE      = 0x10,
} RGB_AMBIENT;

#pragma pack(1)
/* Exported types ------------------------------------------------------------*/
//氛围灯参数结构定义
typedef struct 
{
    volatile u8 AmbientLightMode;           //氛围灯工作模式
	volatile u8 AmbientLightColor;          //氛围灯颜色 (1~7 , 分别对应7种颜色)
    volatile u8 AmbientLightLux;            //氛围灯亮度
	volatile u8 AmbientLight_Period;        //氛围灯呼吸频率
    volatile u8 AmbientLight_Off_Intens;    //氛围灯关闭状态
} AMBIENT_LED;

/*********************
//仪表数据相关定义
**********************/
//1.仪表软件版本信信息
typedef struct 
{
	volatile u16 Dis_SoftwareVersion; //仪表软件版本号
	volatile u16 BMS_SoftwareVersion; //BMS软件版本号
}DIS_VERSION_SW_INFO;

//2.仪表硬件版本信信息
typedef struct 
{
	volatile u16 Dis_HardwareVersion; //仪表硬件版本号
	volatile u16 BMS_HardwareVersion; //BMS硬件版本号
}DIS_VERSION_HW_INFO;

//2.仪表芯片唯一CPID
typedef union
{
	u32  Data32[3];
	s16  Data16[6];
}UNIQ_ID;

//3.NFC卡信息
typedef struct 
{
	volatile u8 NFC_Card_Info_L[8];
	volatile u8 NFC_Card_Info_H[8];
}NFC_CARD_INFO;

//4.虚拟二维码信息
typedef struct 
{
	volatile u8 Virtual_QRcode_L[8];
	volatile u8 Virtual_QRcode_H[8];
}VIRTURE_QRCODE_INFO;

//5.转把信息
typedef struct 
{
    volatile u8 FrameLen;                   //帧长度
	volatile u8 GasValue;                   //油门值
	volatile u8 LeftBreakValue;             //左刹值
    volatile u8 Bool;                       //bool值
    
    volatile u8 Beep;                       //蜂鸣器状态
    volatile u8 Version_A;                  //固件版本
    volatile u8 Version_B;                  //固件版本
	volatile u8 HW_Version;                 //硬件版本
    
	volatile u8 RightBreakValue;            //右刹
    volatile WIRELESS_STATUS WirelessChargerStatus;      //无线充状态     (0:关闭 1:打开)
    volatile HANDLE_TEMP_STATUS HandleTempStatus;           //把套加热状态  (参考 HANDLE_TEMP_STATUS定义)
    volatile s8 HandleLeftTemp;             //左把套温度
    
    volatile s8 HandleRightTemp;            //右把套温度
    volatile u8 TurnSignalLightStatus;      //转向灯状态
} HANDLE_BAR_INFO;


//6.滑板车信息
typedef struct 
{
	volatile u8 FrontlightBrightness;       //前灯亮度
	volatile u8 A_LED;                      //A LED状态
	volatile u8 B_LED;                      //B LED状态
	volatile u8 C_LED;                      //C LED状态
    
	volatile u8 D_LED;                      //D LED状态
	volatile u8 AmbientLightMode;           //氛围灯工装状态(前4位氛围灯(0：仪表; 1:左把; 2:右把), 后四位转向灯模式(0：关闭 1：常亮 2：闪烁 3：呼吸) )
	volatile u8 AmbientLightColor;          //氛围灯颜色 (1~7 , 分别对应7种颜色)
    volatile u8 AmbientLightLux;            //氛围灯亮度
    
	volatile u8 AmbientLight_Period;        //氛围灯呼吸频率
    volatile u8 AmbientLight_Off_Intens;    //氛围灯关闭状态
    volatile u8 BeepTime;                   //蜂鸣器工作时间
    volatile u8 TurnSigStatus;              //转向灯工作状态
    
    volatile u8 ControllerStatus;           //主控状态（0:锁车 1:关锁）
    volatile u8 WirelessChargerSet;         //无线充设置（0:关闭 1:打开）
    
    volatile u8 HandleTempMode;             //把套解热模式（0：关闭；1:打开）
    volatile u8 HandleTempSetValue;         //把套加热设定温度值（16~30度）
    
    volatile u8 GpsPowerSet;                //把套加热设定温度值（16~30度）
}SCOOTER_INFO;


//仪表数据结构体定义
typedef struct 
{
    u8 CommuTimeout;                          // 滑板车通信超时  
	DIS_VERSION_SW_INFO DIS_Version_sw;       //1.仪表软件版本信信息
	DIS_VERSION_HW_INFO DIS_Version_hw;       //2.仪表硬件版本信信息
	volatile u8 DIS_PN[14];                   //3.仪表 PN码
	volatile UNIQ_ID DIS_CPID;                //4.仪表芯片唯一CPID
    NFC_CARD_INFO NFC_Card_Info;              //5.NFC卡信息
	VIRTURE_QRCODE_INFO QRCode_Info;          //6.虚拟二维码信息	
	HANDLE_BAR_INFO Handle_Bar_Info;          //7.转把信息
    SCOOTER_INFO    Scooter_Info;             //8.滑板车主控状态
    AMBIENT_LED     Ambient_Light;            //9. 表盘氛围灯
    AMBIENT_LED     RGB_Led;                  //10. 左右转把氛围灯
}MYSELF_DATA;

#pragma pack()

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
extern volatile u8 g_bool[B_LEN];		//布尔变量
extern volatile u16 g_Clear_error_cnt;  //故障清零计数
extern volatile u8 g_ErrorByte;
extern volatile u8 g_wireless_charger_vol; 
extern volatile u8 g_12V_vol;
extern volatile u16 g_leftBreakGnd_vol;
extern volatile u16 g_rightBreakGnd_vol;
extern volatile u16 g_accGnd_vol;
extern QueueHandle_t COM1_Message_Queue;
extern MYSELF_DATA g_myself_data;
//extern MC_RUNNING_DATA MC_Running_data;
extern volatile s16 g_NBCmdMap[NB_CMDMAP_LEN];	//内存控制表
//extern PN_WR_STR PN_Wr;

/* Exported functions ------------------------------------------------------- */
void COM1_Task(void *pvParameters);

#endif 

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

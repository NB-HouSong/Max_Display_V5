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
#define FW_VERSION			    (u16)(0x2300)

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
#define CMD_SCO_CTL               0x64  //scooter专用，数据段只含有油门和刹车等控制数据，主控需要返回电量、前灯控制等数据
#define CMD_SCO_CTL_NR            0x65  //scooter专用，数据段只含有油门和刹车等控制数据，无需主控返回信息

#define  CMD_IAP_RD               0X79  //Voi定制
#define  CMD_IAP_RD_ACK           0X7A

#define CMD_NFC_UPDATE            0xE0  //上载NFC信息

//IAP更新相关指令
#define CMD_IAP_BEGIN			  0x07	//IAP开启指令
#define CMD_IAP_WR				  0x08	//IAP写固件指令
#define CMD_IAP_CRC				  0x09	//IAP校验固件指令
#define CMD_IAP_RESET			  0x0A	//系统复位指令
#define CMD_IAP_ACK				  0x0B	//应答帧


//达等相关宏定义
#define LED_OFF      0
#define LED_ON       1
#define LED_Flash    2
#define LED_Breath   3

//#define Level_0  0
//#define Level_1  1
//#define Level_2  2
//#define Level_3  3

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

/*********************
//仪表自身数据相关宏定义
**********************/
//为了将NFC刷卡的锁车接送指令发送给中继板
#define NFC_VEHICLE_LOCK    0x02    //NFC刷卡锁车
#define NFC_VEHICLE_UNLOCK  0x03    //NFC刷卡解锁

//数据块地址定义
#define DIS_VERSION_SW             0x01   //4字节 软件版本帧 u16*2
#define DIS_VERSION_HW             0x02   //4字节 硬件版本帧 u16*2
#define NFC_CARD_INFO_L            0x04   //8字节 NFC卡信息低8字节
#define NFC_CARD_INFO_H            0x05   //8字节 NFC卡信息高8字节
#define VIRTUAL_QRCODE_L           0x06   //8字节 虚拟二维码信息低8字节
#define VIRTUAL_QRCODE_H           0x07   //8字节 虚拟二维码信息高8字节
#define NFC_MODE_SET               0x08   //6字节 NFC模式设置 0：读卡 1：卡片 +语音设置
#define HANDLEBAR_INFO             0x09   //8字节 转把信息
#define FRONTLIGHT_SET             0x0A   //4字节 前大灯
#define AMBIENTLIGHT_SET           0x0B   //4字节 氛围灯设置
#define AUDIO_SET                  0x0C   //3字节 语音设置
#define PANEL_SET                  0x0D   //5字节 面板显示
#define WIRELESSCHARGER_SET        0x0E   //4字节 无线充设置

#define DIS_PN_L                   0x11   //8字节 仪表PN码低8位
#define DIS_PN_H                   0x12   //6字节 仪表PN码低6位
#define DIS_PN_CRC                 0x13   //4字节 仪表PN码CRC校验码
#define BMS_SN_L                   0x14   //8字节 BMS序列号低8字节
#define BMS_SN_H                   0x15   //6字节 BMS序列号高6字节

#define BMS_DATA                   0x16   //7字节 电池电流电压等信息 s16*3+u8
#define BMS_TEMP                   0x17   //6字节 电池温度信息 s8*6

#define DIS_CPID_L                 0xFA   //8字节 CPID低8字节
#define DIS_CPID_H                 0xFB   //4字节 CPID高4字节
#define PRIVATE_COMMKEY_L          0xFC   //8字节 通讯密钥低8字节
#define PRIVATE_COMMKEY_H          0xFD   //8字节 通讯密钥高8字节
#define PRIVATE_IAPKEY_L           0xFE   //8字节 IAP密钥低8字节
#define PRIVATE_IAPKEY_H           0xFF   //8字节 IAP密钥低8字节

//各数据块长度
#define DIS_VERSION_SW_LENTH       4   //4字节 仪表和BMS软件版本帧 u16*2
#define DIS_VERSION_HW_LENTH       4   //4字节 仪表和BMS硬件版本帧 u16*2
#define NFC_CARD_INFO_L_LENTH      8   //8字节 NFC卡信息低8字节
#define NFC_CARD_INFO_H_LENTH      8   //8字节 NFC卡信息高8字节
#define VIRTUAL_QRCODE_L_LENTH     8   //8字节 虚拟二维码信息低8字节
#define VIRTUAL_QRCODE_H_LENTH     8   //8字节 虚拟二维码信息高8字节
#define NFC_MODE_SET_LENTH         6   //6字节 NFC模式设置+语音设置
#define HANDLEBAR_INFO_LENTH       8   //8字节 转把信息
#define FRONTLIGHT_SET_LENTH       4   //4字节 前大灯
#define AMBIENTLIGHT_SET_LENTH     4   //4字节 氛围灯设置
#define AUDIO_SET_LENTH            3   //3字节 语音设置
#define PANEL_SET_LENTH            5   //5字节 面板显示
#define WIRELESSCHARGER_SET_LENTH  4   //4字节 无线充设置
#define BMS_SN_L_LENTH             8   //8字节 BMS序列号低8字节
#define BMS_SN_H_LENTH             6   //6字节 BMS序列号高6字节

#define DIS_PN_L_LENTH             8   //8字节 仪表PN码低8位
#define DIS_PN_H_LENTH             6   //6字节 仪表PN码低6位
#define DIS_PN_CRC_LENTH           4   //4字节 仪表PN码CRC校验码

#define BMS_DATA_LENTH             7   //7字节 电池电流电压等信息 s16*3+u8
#define BMS_TEMP_LENTH             6   //6字节 电池温度信息 s8*6

#define DIS_CPID_L_LENTH           8   //8字节 CPID低8字节
#define DIS_CPID_H_LENTH           4   //4字节 CPID高4字节
#define PRIVATE_COMMKEY_L_LENTH    8   //8字节 通讯密钥低8字节
#define PRIVATE_COMMKEY_H_LENTH    8   //8字节 通讯密钥高8字节
#define PRIVATE_IAPKEY_L_LENTH     8   //8字节 IAP密钥低8字节
#define PRIVATE_IAPKEY_H_LENTH     8   //8字节 IAP密钥低8字节

/*********************
//控制器数据相关宏定义
**********************/
//控制器数据块地址定义
#define MC_POWER                    0x08   //8字节 控制器及其外设硬件版本 s16*4
#define MC_ERR                      0x09   //2字节 控制器故障码及报警码 u8*2
#define MC_SPEED                    0x0A   //6字节 控制器速度相关信息 s16*2+u8*2
#define MC_STATUS                   0x0B   //4字节 控制器状态信息 u8*4
#define MC_MOTER_TEMP               0x0C   //4字节 控制器及电机温度 s16*2
#define MC_TOTAL_TIME_MILES         0x0F   //8字节 车辆运行总里程和总时间 s16*4

#define MC_SET                      0x20   //8字节 主控参数设置 u16*2+u8*4

#define MC_ANGLE_SET_EN             0x22   //1字节 角度校准使能开关 u8
#define MC_ANGLE_INFO               0x14   //1字节 角度信息 s32*2

#define VEHICLE_CTL_LOCK            0x30   //1字节 锁车 u8
#define VEHICLE_CTL_UNLOCK          0x31   //1字节 解锁 u8
#define VEHICLE_CTL_POWEROFF        0x32   //1字节 关机 u8
#define VEHICLE_CTL_RESET           0x33   //1字节 重启 u8
#define VEHICLE_CTL_SLEEP           0x34   //1字节 休眠 u8

//控制器各数据块长度
#define MC_POWER_LENTH              8   //8字节 控制器及其外设硬件版本 s16*4
#define MC_ERR_LENTH                2   //2字节 控制器故障码及报警码 u8*2
#define MC_SPEED_LENTH              6   //6字节 控制器速度相关信息 s16*2+u8*2
#define MC_STATUS_LENTH             4   //4字节 控制器状态信息 u8*4
#define MC_MOTER_TEMP_LENTH         4   //4字节 控制器及电机温度 s16*2
#define MC_TOTAL_TIME_MILES_LENTH   8   //8字节 车辆运行总里程和总时间 s16*4

#define MC_SET_LENTH                8   //8字节 主控参数设置 u16*2+u8*4

//蜂鸣器指令宏定义
#define MINI_BEEP_CMD_ONCE_S	    ((unsigned char)0x01)		//短响
#define MINI_BEEP_CMD_ONCE_L		((unsigned char)0x02)		//长响
#define MINI_BEEP_CMD_DOUBLE_S	    ((unsigned char)0x03)		//两声短响
#define MINI_BEEP_CMD_ONCE_LL	    ((unsigned char)0x04)		//加长响

typedef enum
{
    HANDLE_TEMP_OFF,
    HANDLE_TEMP_ON,
    HANDLE_TEMP_DIFF_LARGE,
    HANDLE_TEMP_LEFT_BROKN,
    HANDLE_TEMP_RIGHT_BROKN,
} HANDLE_TEMP_STATUS;

#pragma pack(1)
/* Exported types ------------------------------------------------------------*/

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
    volatile u8 WirelessChargerStatus;      //无线充状态     (0:关闭 1:打开)
    volatile HANDLE_TEMP_STATUS HandleTempStatus;           //把套加热状态  (参考 HANDLE_TEMP_STATUS定义)
    volatile u8 HandleLeftTemp;             //左把套温度
    
    volatile u8 HandleRightTemp;            //右把套温度
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
}SCOOTER_INFO;

//14.IAP密钥
typedef struct 
{
	volatile u8 Private_IAPkey_L[8];  //低8字节
	volatile u8 Private_IAPkey_H[8];  //高8字节
}PRIVATE_IAPKEY_INFO;

//typedef struct 
//{
//	volatile u8 VehicleNFCmodeSet;        //NFC模式设置
//	struct Audio_SW AudioEnableConfig;    //Bit[0]:使能总开关，0关闭，1允许播放；Bit[1:31]位地址对应音频list，空余位保留0音频关闭 1允许播放
//	volatile u8 AudioVolumeConfig;        //音量设置
//}NFC_AUDIO_SET;

//仪表数据结构体定义
typedef struct 
{
    u8 CommuTimeout;                          // 滑板车通信超时  
	DIS_VERSION_SW_INFO DIS_Version_sw;       //1.仪表软件版本信信息
	DIS_VERSION_HW_INFO DIS_Version_hw;       //仪表硬件版本信信息
	volatile u8 DIS_PN[14];                   //仪表 PN码
	volatile UNIQ_ID DIS_CPID;                //2.仪表芯片唯一CPID
    NFC_CARD_INFO NFC_Card_Info;              //3.NFC卡信息
	VIRTURE_QRCODE_INFO QRCode_Info;          //4.虚拟二维码信息	
	//NFC_AUDIO_SET NFC_Audio_set;              //NFC模式和语音设置
	HANDLE_BAR_INFO Handle_Bar_Info;          //5.转把信息
    SCOOTER_INFO    Scooter_Info;             //6.滑板车主控状态
    //u8 TurnSignalLightFlashStatus;              //6.转向灯和箭头同步闪烁使用
	//FRONTLIGHT_INFO Frontlight_Ctr;           //6.前大灯
	//AMBIENTLIGHT_INFO Ambientlight_Ctr;       //7.氛围灯设置
	//AUDIOSET_INFO Audio_Set;                  //8.语音控制
	//PANELSET_INFO Panel_Set;                  //9.显示面板控制
	//WIRELESSCHARGER_INFO Wirelesscharger_Set; //10.无线充控制
	//BMS_DATA_INFO BMS_Data;                   //11.电池数据
	//BMS_TEMP_INFO BMS_Temp;                   //12.电池温度数据
	//PRIVATE_COMMKEY_INFO Private_Comkey;      //13.通讯密钥
	//PRIVATE_IAPKEY_INFO Priver_IAPkey;        //14.IAP密钥

}MYSELF_DATA;

///*********************
////控制器状态数据相关定义
//**********************/
////1.控制器功率数据
//typedef struct 
//{
//	volatile s16 MC_Voltage;    //控制器电压 Unit：10mV
//	volatile s16 MC_Current;    //控制器电流 Unit：10mA
//	volatile s16 MC_InputPower; //控制器输入功率 Unit：10mW
//	volatile s16 Motor_Power;   //电机功率 Unit：10mW
//}MC_POWER_INFO;

////2.控制器故障码与报警码
//typedef struct 
//{
//	volatile u8 AlarmCode;    //报警码
//	volatile u8 ErrCode;      //故障码
//}MC_ERRCODE_INFO;

////3.控制器速度数据
//typedef struct 
//{
//	volatile s16 VehicleSpeed;     //车辆速度
//	volatile u8 VehicleSpeedUnit; //车辆速度单位 0: KM/H 1:MPH
//	volatile s16 MotorSpeed;       //电机速度
//	volatile u8 MotorSpeedUnit;   //电机速度单位 0：rpm； 1：elespeed; 2: km/h
//}MC_SPEED_INFO;

////4.控制器状态数据
//typedef struct 
//{
//	volatile u8 Null;                 //充电状态  目前无效
//	volatile u8 CoverlockStaus;       //盖板锁状态
//	volatile u8 LockStatus;           //锁车状态
//	volatile u8 VehicleDrivingMode;   //骑行模式（档位）：0: 纯人力模式 1：ECO 模式 2:  Sport模式 3: 纯电动模式
//}MC_STATUS_INFO;

////5.控制器温度数据
//typedef struct 
//{
//	volatile s16 MCTemp;           //控制器温度
//	volatile s16 MotorTemp;        //电机温度
//}MC_MOTER_TEMP_INFO;

////8.总骑行时间与总骑行里程
//typedef struct 
//{
//	volatile u16 Total_RunningTime_L;   //总骑行时间
//	volatile u16 Total_RunningTime_H;   //总骑行时间
//	volatile u16 TotalMiles_L;          //总骑行里程
//	volatile u16 TotalMiles_H;          //总骑行里程
//}MC_TOTAL_TIME_MILES_INFO;

////8.角度信息
//typedef struct 
//{
//	volatile s32 ANGLE_ROAD;          //处理后的角度
//	volatile s32 ANGLE_REAL;          //真实角度
//}MC_ANGLE1_INFO;

////9.控制器设置信息
//typedef struct 
//{
//	volatile u8 VehicleAlarmSet;      //报警开关设置
//	volatile u8 AlarmSensitivitySet;  //报警灵敏度设置
//	volatile u8 SpeedUnitSet;         //速度单位
//	volatile u16 MaxSpeedSet;         //最大速度设置
//	volatile u16 MaxPowerSet;         //最大功率设置
//	volatile u8 DrivingMode;          //助力模式设置
//}MC_SET_INFO;

////控制器数据结构体定义
//typedef struct 
//{
//	MC_POWER_INFO MC_Power;           //1.控制器功率数据
//	MC_ERRCODE_INFO MC_Errcode;       //2.控制器故障码与报警码
//	MC_SPEED_INFO MC_Speed;           //3.控制器速度数据
//	MC_STATUS_INFO MC_Status;         //4.控制器状态数据
//	MC_MOTER_TEMP_INFO MC_Moter_Temp; //5.控制器温度数据
//	MC_TOTAL_TIME_MILES_INFO MC_Total_Time_Miles;//8.总骑行时间与总骑行里程
//	MC_ANGLE1_INFO MC_Angle;
//	MC_SET_INFO MC_Set;
//}MC_RUNNING_DATA;

//typedef struct 
//{
//	volatile u8 DIS_PNWRBUF[18];                 //PN码暂存
//	volatile u8 SRC_ID;
//	volatile u8 DST_ID;
//	volatile u8 CMD;
//}PN_WR_STR;

#pragma pack()

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
extern volatile u8 g_bool[B_LEN];		//布尔变量
extern volatile u16 g_Clear_error_cnt;  //故障清零计数
extern volatile u8 g_ErrorByte;
extern volatile u8 g_wireless_charger_vol; 
extern volatile u8 g_12V_vol;
extern QueueHandle_t COM1_Message_Queue;
extern MYSELF_DATA g_myself_data;
//extern MC_RUNNING_DATA MC_Running_data;
extern volatile s16 g_NBCmdMap[NB_CMDMAP_LEN];	//内存控制表
//extern PN_WR_STR PN_Wr;

/* Exported functions ------------------------------------------------------- */
void COM1_Task(void *pvParameters);

#endif 

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

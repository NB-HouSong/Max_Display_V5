/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : batterycmd.h
* Author        : Qiqi
* Version       : V1.0.0
* Date          : 2020-9-3
* Description   : 
**********************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  __BATTERYCMD_H__
#define  __BATTERYCMD_H__
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#define    BMS_CMDMAP_LEN     255			//内存控制表总长度
#define    BMS_CMDMAP_INDLEN    8 			//内存控制表主索引
#define    BMS_CMDMAP_SUBLEN   16 			//内存控制表子索引
//0x00 ~ 0x0F 保留PACK厂使用
#define  BAT_RES               0x00
//0x10 ~ 0x16 电芯序列号
#define  BAT_SN                0x10		//电池序列号，14Byte，占7组
//0x17
#define  BAT_SW_VER            0x17		//电池固件版本
#define  BAT_CAPACITY          0x18		//电池设计容量，mAh
#define  BAT_TOTAL_CAPACITY    0x19		//电池完全充电容量，mAh
#define  BAT_DESIGN_VOLTAGE    0x1A		//电池设计电压，10mV
#define  BAT_CYCLE_TIMES       0x1B		//电池循环次数
#define  BAT_CHARGE_TIMES      0x1C		//电池充电次数
#define  BAT_CHARGE_CAP_L      0x1D		//电池累计充电容量低16位，10mAh
#define  BAT_CHARGE_CAP_H      0x1E		//电池累计充电容量高16位，10mAh
#define  BAT_OVER_DISCHARGE_TIMES    0x1F		//电池过流、过放次数，低八位为过流次数，高八位为过放次数
//0x20~0x2F  Ninebot 
//0x30
#define  BAT_FUN_BOOLEAN             0x30		//布尔变量
#define  BAT_REMAINING_CAP           0x31		//当前剩余容量，mAh
#define  BAT_REMAINING_CAP_PERCENT   0x32		//当前剩余容量百分比
#define  BAT_CURRENT_CUR             0x33		//当前电流，10mA
#define  BAT_VOLTAGE_CUR             0x34		//当前电压，10mV
#define  BAT_TEMP_CUR                0x35		//温度，高低八位分别代表两个温度传感器温度，0~119对应-20~99℃
#define  BAT_BALANCE_STATU           0x36		//保护板均衡开启状态
#define  BAT_ODIS_STATE				 0x37		//电芯欠压状态
#define  BAT_OCHG_STATE				 0x38		//电芯过压状态
#define  BAT_CAP_COULO				 0x39		//库仑计容量
#define  BAT_CAP_VOL 				 0x3A		//电压计容量
#define  BAT_HEALTHY                 0x3B		//
//0x3C
//0x3D
#define  BAT_VIRTUAL_CAP_PERCENT     0x3E       //virtual battery capacitance // Herbert 20190726
//0x3B ~ 0x3F Ninebot ??
#define  BAT_CORE_VOLT_1             0x40
#define  BAT_CORE_VOLT_2             0x41
#define  BAT_CORE_VOLT_3             0x42
#define  BAT_CORE_VOLT_4             0x43
#define  BAT_CORE_VOLT_5             0x44
#define  BAT_CORE_VOLT_6             0x45
#define  BAT_CORE_VOLT_7             0x46
#define  BAT_CORE_VOLT_8             0x47
#define  BAT_CORE_VOLT_9             0x48
#define  BAT_CORE_VOLT_10            0x49
#define  BAT_CORE_VOLT_11            0x4A
#define  BAT_CORE_VOLT_12            0x4B
#define  BAT_CORE_VOLT_13            0x4C
#define  BAT_CORE_VOLT_14            0x4D
#define  BAT_CORE_VOLT_15            0x4E
#define  BAT_CORE_VOLT_16            0x4F
#define  BAT_BEEN_ACT 				 0x50		//是否被激活
#define  BAT_HARDWARE_VERSION      	 0x51       //HW Ver
#define  BAT_TEMP1_CUR_MOS   		 0x52  //temp34
#define  BAT_TEMP2_CUR_MOS   		 0x53  //temp56
#define  BAT_CYPTCODE                0x60       //0x60~0x63
#define  BAT_CPID                    0x70       //0x70~0x75
#define  BAT_VERIFY                  0x80       //
//BOOL  Mark
#define BMS_BOOLMARK_PASSWORD		  ((unsigned short)0x0001)		//是否电池校验码正确   //使用
#define BMS_BOOLMARK_ACT		      ((unsigned short)0x0002)		//是否电池已激活
#define BMS_BOOLMARK_CHG_PROTECT	  ((unsigned short)0x0004)		//是否电池充电保护
#define BMS_BOOLMARK_CMOS			  ((unsigned short)0x0008)		    //是否充电MOS管开启
#define BMS_BOOLMARK_WRITE_CMD		  ((unsigned short)0x0010)		//是否写锁定打开
#define BMS_BOOLMARK_DISCHARGE		  ((unsigned short)0x0020)		//是否放电
#define BMS_BOOLMARK_CHARGE			  ((unsigned short)0x0040)		  //是否充电             //使用
#define BMS_BOOLMARK_CHARGERIN			((unsigned short)0x0080)		//是否充电器插上
#define BMS_BOOLMARK_DISOVER		    ((unsigned short)0x0100)		//是否过放
#define BMS_BOOLMARK_CHGOVER		    ((unsigned short)0x0200)		//是否过充             //使用  
#define BMS_BOOLMARK_VOERTEMP		    ((unsigned short)0x0400)		//是否过温
#define BMS_BOOLMARK_TEST_MODE		  ((unsigned short)0x0800)		//是否测试模式
#define BMS_BOOLMARK_MINI_POWERON	  ((unsigned short)0x1000)		//是否车体已经上电
/* Exported functions ------------------------------------------------------- */

#endif
/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/


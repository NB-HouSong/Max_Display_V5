/******************** (C) COPYRIGHT 2018 Ninebot ********************
* File Name     : batmanage.h
* Author        : Qiqi
* Version       : V1.0.0
* Date          : 2020-9-3
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  __BATMANAGE_H__
#define  __BATMANAGE_H__
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "batterycmd.h"
#include "Scooter_decrypt.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define _1_MIN                      (30000/5)
#define _HALF_MIN                   (_1_MIN/2)
#define _1_SECOND                   (_1_MIN/60)
#define _HALF_ONE_SENCOND           (_1_SECOND/2)
#define BMS_OLD_VERSION             0x350
#define BMS_HARD_VERSION_PRO        0x04
#define BMS_HARD_VERSION_NORM       0x03
#define BMS_INT_TIMEOUT 			(4000)
#define BMS_INT_INF_LEN             8
#define BMS_BOOL_INF_LEN            36    //15
#define BMS_CPID_LEN                6
#define FULL_BATTERY_CAPACITY       15300
#define FULL_BATTERY_CAPACITY_PRO   27900

#define  SN_LEN      14
#define  CPID_LEN    12
/* Exported types ------------------------------------------------------------*/
typedef struct{
    u32 s_battery_commu_cnt;
    u32 s_BMS_verify_err_cnt;
	u8  s_BMS_update_flag;
	u8  s_BMS_Init_OK;
	u8  havegetinitinfo_flag;
	u8  repeatreadver_flag;
	u8  s_BMS_SN_Checked;
	u8  s_BMS_OffLine;
	u8  s_BMS_Is_Commu;
	u8  BMS_IAP_IDENT_FLAG;
	u8  BMS_IAP_Update;
	u8  s_BMS_IAP_Mode;
	u8  s_BMS_Test_In;
    u8  s_BMS_selected_flag;
    u8  s_battery_init_status;
    u8  s_battery_info_switch;
    u8  s_identify_array[8];
    u8  s_identify_set_flag;
    void (*s_battery_pro)(void);
}BATTERY_OB;
extern BATTERY_OB 		battery_object;
#pragma pack(1) 
typedef struct
{
	u8  sn[14];
	u32 cpid[3];
	u16 boolean;
	u16 remain_cap;
	u16 remain_cap_percent;
	u16 virtual_cap_percent;
    u16 volt_cur;
	u16 current_cur;
	u16 temperature_12_cur;
	u16 temperature_34_cur;
	u16 temperature_56_cur;
	
} BMS_PARA_TYPE;
#pragma pack()
extern BMS_PARA_TYPE  	g_stBMS_Info;


/* Exported functions ------------------------------------------------------- */
void BatteryCommu_Pro(void);

#endif 

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/


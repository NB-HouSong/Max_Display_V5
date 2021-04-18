/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : StateUpdate.h
* Author        : 李美华
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STATE_UPDATE_H
#define STATE_UPDATE_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
//临时IOT心跳数据
#define HBDATA_LEN              24      //长度
#define LOCK_STATUS             0                //锁车状态                  1byte  
#define CHARGE_STATUS           1                //充电状态                  1byte
#define BAT1_VOLT               2                //系统电压    2~3           2byte
#define BODY_VOLT_SOC           4       //整车电量百分比            1byte
#define CURRENT_SPEED           5       //当前速度                  1byte
#define BAT_CNT                 6       //接入电池数量              1byte
#define BAT1_TEMP               7       //电池1温度                 1byte
#define BAT2_TEMP               8       //电池2温度                 1byte
#define BAT3_TEMP               9       //电池3温度                 1byte
#define MOPED_MILEAGE           10      //总里程   单位：m  10~13   4byte 
#define MOPED_RATED_MIL         14      //剩余里程 单位：0.1km      2byte
#define MOPED_ERR               16      //错误码                    1byte
#define MOPED_ALARM             17      //报警码                    1byte
//Reserved                      18~23   //  

/* Exported types ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Check_IAP_Mode(void);
void Query_Send_Data_Pro(void);
void MasterPowerCtrl(void);

#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

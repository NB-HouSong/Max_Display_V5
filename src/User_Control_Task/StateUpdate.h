/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : StateUpdate.h
* Author        : ������
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
//��ʱIOT��������
#define HBDATA_LEN              24      //����
#define LOCK_STATUS             0                //����״̬                  1byte  
#define CHARGE_STATUS           1                //���״̬                  1byte
#define BAT1_VOLT               2                //ϵͳ��ѹ    2~3           2byte
#define BODY_VOLT_SOC           4       //���������ٷֱ�            1byte
#define CURRENT_SPEED           5       //��ǰ�ٶ�                  1byte
#define BAT_CNT                 6       //����������              1byte
#define BAT1_TEMP               7       //���1�¶�                 1byte
#define BAT2_TEMP               8       //���2�¶�                 1byte
#define BAT3_TEMP               9       //���3�¶�                 1byte
#define MOPED_MILEAGE           10      //�����   ��λ��m  10~13   4byte 
#define MOPED_RATED_MIL         14      //ʣ����� ��λ��0.1km      2byte
#define MOPED_ERR               16      //������                    1byte
#define MOPED_ALARM             17      //������                    1byte
//Reserved                      18~23   //  

/* Exported types ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Check_IAP_Mode(void);
void Query_Send_Data_Pro(void);
void MasterPowerCtrl(void);

#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

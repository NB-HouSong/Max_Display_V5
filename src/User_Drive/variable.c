/******************** (C) COPYRIGHT 2020 Ninebot *******************************
* File Name     : variable.c
* Author        : meihua
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
********************************************************************************/
#include "variable.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
volatile s16 g_NBCmdMap[NB_CMDMAP_LEN] = {0};	//�ڴ���Ʊ�

MYSELF_DATA g_myself_data;
//MC_RUNNING_DATA MC_Running_data;

volatile u8 g_bool[B_LEN];			
volatile u16 g_Clear_error_cnt = 0;     //�����������

volatile u8 g_ErrorByte = 0;			//�����ֽ�
volatile u8 g_wireless_charger_vol = 0; //���߳乩���ѹ
volatile u8 g_12V_vol = 0;              //12V��ơ����乩���ѹ
volatile u16 g_leftBreakGnd_vol = 0;    //��ɲ���߼��
volatile u16 g_rightBreakGnd_vol = 0;   //��ɲ���߼��
volatile u16 g_accGnd_vol = 0;          //acc ���ߵ�ѹ���
//PN_WR_STR PN_Wr;

/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/



//==================================== The End===============================================/



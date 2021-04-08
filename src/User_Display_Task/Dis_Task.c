/******************** (C) COPYRIGHT 2020 Ninebot *******************************
* File Name     : Dis_Task.c
* Author        : meihua
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "tm1650.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ERROR_TIME2	67		 //��ʼ�����ʱ��2s
#define ERROR_TIME	16//33	 //������ʱ��1s�����Ʊ���Ƶ�ʣ�

/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/* @ Define variables of Display object*/
const u8 CODE[17] = {0x3f,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7F,0x39,0x3F,0x79,0x71,0x00};
	
static u8 TM1652_Dig1 = 0;   //����1
static u8 TM1652_Dig2 = 0;   //����1
static u8 TM1652_Dig3 = 0;   //����2
static u8 TM1652_Dig4 = 0;   //����2
static u8 TM1652_Dig5 = 0;   //ͼ��  
static u8 TM1652_Dig6 = 0;   //ͼ��
	
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/*****************************************************************
* Function Name : Digital_Display
* Description   : �����������ʾ����
* Input         : data��Ҫ��ʾ������ addr��Ҫ��ʾ���ֵ������
* Output        : None
* Notes         :
******************************************************************/	
void Digital_Display(u8 data, u8 addr)
{
	switch(addr)
	{
	case Digital_1:
	  TM1652_Dig3 = CODE[data];
	  TM1652_Dig4 = CODE[data];			
	  break;
	case Digital_2:
	  TM1652_Dig1 = CODE[data];
	  TM1652_Dig2 = CODE[data];
	  break;
	
	default:
		break;	
	}
}
 
/*****************************************************************
* Function Name : Digital_Display_Off
* Description   : ��������ʾ
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void Digital_Display_Off(void)
{
	TM1652_Dig1 = CODE[16];
	TM1652_Dig2 = CODE[16];
	TM1652_Dig3 = CODE[16];
	TM1652_Dig4 = CODE[16];	
}

/*****************************************************************
* Function Name : Display_Handler
* Description   : ��������
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void Display_Handler(void)
{
	static u8 send_data[9];
	send_data[0] = DIS_ADDRESS_COM;
	send_data[1] = TM1652_Dig1;
	send_data[2] = TM1652_Dig2;
	send_data[3] = TM1652_Dig3;
	send_data[4] = TM1652_Dig4;
	send_data[5] = TM1652_Dig5;
	send_data[6] = TM1652_Dig6;
	send_data[7] = DIS_CONTROL_COM;
	send_data[8] = DIS_CONTROL_ADJ_COM;
	UART_PushFrame(9, send_data);
}

/*****************************************************************
* Function Name : Dis_Speed
* Description   : ��ʾָ��������
* Input         : None
* Output        : u8:����ͬ����˸��־�������ֺ�ͼ��ͬ����˸
* Notes         : ���ƹ����롢�����ٶȡ����ʱ�ĵ�����ʾ
******************************************************************/	
u8 Dis_4LED(void) //30ms
{
	static u8 s_cnt = 0;
	static u8 flag_error_state = 0;
	static u8 flag_flash_state = 0;
//	static s16 running_speed = 0;
	u8 ge,shi;
	s_cnt ++;
#ifdef Test_Mode
	if(MC_Running_data.MC_Status.LockStatus == LOCKSTATE_LOCK)
	{
		Digital_Display(16, Digital_1);
		Digital_Display(16, Digital_2);
		
		TM1652_Dig5 = 0;
		TM1652_Dig6 = 0;
	}
	else
	{
		Digital_Display(8, Digital_1);
		Digital_Display(8, Digital_2);
		
		TM1652_Dig5 = 0xFF;
		TM1652_Dig6 = 0xFF;
	}
#else
//	if(s_cnt > ERROR_TIME2 + ERROR_TIME) //���Ʊ���Ƶ��
//	{
//		s_cnt = ERROR_TIME2;             //���ƿ�ʼ����ʱ��
//		flag_error_state = !flag_error_state;
//		flag_flash_state = !flag_flash_state;
//	}
	//�޹��ϻ�IAPģʽ�� �ع�����ʾ
//	if(g_ErrorByte == 0 || g_myself_data.Panel_Set.IOT_ERR == 1 || g_bool[B_IN_IAPMODE] == 1 \
//		|| MC_Running_data.MC_Status.LockStatus == LOCKSTATE_LOCK)
//	{
//		flag_error_state = 0;
//		ERROE_OFF;
//	}
//	
//	if(g_myself_data.Panel_Set.IOT_ERR == 1)
//	{
//		shi = 8;
//		ge = 8;
//		ERROE_OFF;
//	}
//	else 
//	{
//		if(flag_error_state) //��ʾ������
//		{
//			flag_flash_state = flag_error_state;
//			KM_H_OFF;
//			MPH_OFF;
//			
//			Digital_Display_Off();
//			ERROE_ON;
//			if(g_ErrorByte < 0xE0)
//			{
//				shi = g_ErrorByte / 10;
//				ge = g_ErrorByte % 10;
//			}
//			else
//			{
//				shi = g_ErrorByte / 16;
//				ge = g_ErrorByte % 16;
//				ERROE_OFF;
//			}
//		}
//		else  //��ʾ�ٶ� �ͳ������еĵ���
//		{
//			ERROE_OFF;
//			KM_H_OFF;
//			MPH_OFF;
//			Digital_Display_Off();
//			if(g_myself_data.BMS_Data.Charging_State) //�ڳ�磬��ʾ����
//			{
//				shi = g_stBMS_Info.virtual_cap_percent / 10;
//				ge = g_stBMS_Info.virtual_cap_percent % 10;
//			}
//			else //���ڳ�磬��ʾ�ٶ�
//			{
//				if(MC_Running_data.MC_Status.LockStatus == LOCKSTATE_LOCK)
//				{
//					shi = 16;
//					ge =  16;
//				}
//				else
//				{
//					running_speed = MC_Running_data.MC_Speed.VehicleSpeed; //ֻ��ʾ�ٶȾ���ֵ
//					
//					shi = running_speed / 10;
//					ge = running_speed % 10;
//					
//					if(MC_Running_data.MC_Speed.VehicleSpeedUnit == KMPH) //������ʾ
//					{
//						KM_H_ON;
//						MPH_OFF;
//					}
//					else //Ӣ����ʾ
//					{
//						KM_H_OFF;
//						MPH_ON;
//					}
//				}
//			}
//			if(MC_Running_data.MC_Status.LockStatus == LOCKSTATE_LOCK)
//			{
//				MPH_OFF;
//				KM_H_OFF;
//			}
//		}
//	}
//	if(shi != 0)
//	{
//		Digital_Display(shi, Digital_2); //
//	}
//	Digital_Display(ge, Digital_1);
#endif

//	Display_Handler(); //��������

	return flag_flash_state; //Ϊ��ʹ���������Ǳ��ϵ�ͼ��ͬ����˸
}

/*****************************************************************
* Function Name : Init_Show_Process
* Description   : ������ʼ����ʾ��ͼ��ȫ��2s
* Input         : None
* Output        : u8:���ؿ������沥����ɽ��������ʼ����ɷ���1
* Notes         : ��ʼ��ʱ��2s�����ݼ�ͼ��ȫ����ʾ����Χ�Ʋ���
******************************************************************/	
static u8 Init_Show_Process(void )
{
	static u8 s_timer_cn = 0;
	s_timer_cn++;
	if(s_timer_cn == 1)
	{
		TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);          //������ʱ��2�ж�
	    TIM_Cmd(TIM2, ENABLE);  	                        //ʹ��TIM2
		//USART_ITConfig(TM1652_UART, USART_IT_TC, ENABLE);   //���������ж�
#ifndef Test_Mode		
		PlayVoice(VOICE_BATIN);	
#endif
	}
#ifdef Test_Mode
	return 1;
#else
	if(s_timer_cn < 199) //2s
	{
		Digital_Display(8, Digital_1);
		Digital_Display(8, Digital_2);
		
		TM1652_Dig5 = 0xFF;
		TM1652_Dig6 = 0xFF;

		ERROE_OFF;  
//		BAT_SOC_RED_OFF;
		PowerON_RGB();  //����RGB
	}
	else
	{
		Digital_Display_Off(); //����ʾ
		TM1652_Dig5 = 0x00;
		TM1652_Dig6 = 0x00;
		
		s_timer_cn = 0;
 #ifdef IOT_METER
//		g_bool[B_LOCKED] = 1; //������ʼ����ɺ�����
 #endif
		ALL_RGB_OFF();
		g_bool[B_INIT_SHOW_OK] = 1; //���������������
		Display_Handler(); //��������
		return 1;
	}
	Display_Handler(); //��������
#endif
	return 0;
}

/*****************************************************************
* Function Name : BAT_SOC_Display
* Description   : ������ʾ����
* Input         : Flash_state����˸���Ʊ�־��������ͼ��ͬ����˸
* Output        : None
* Notes         : 
******************************************************************/	
static void BAT_SOC_Display(u8 Flash_state)
{
//	u8 SOC_Level = 0;
//	static u8 LED_num = 0;
//	static u8 Last_Flash_state = 0;

//	if(g_myself_data.BMS_Data.Charging_State) //�������ڳ��
//	{
//        //��������˸
//		if(g_stBMS_Info.virtual_cap_percent < 20)
//			SOC_Level = 0;
//		else if(g_stBMS_Info.virtual_cap_percent < 40)
//			SOC_Level = 1;
//		else if(g_stBMS_Info.virtual_cap_percent < 60)
//			SOC_Level = 2;
//		else if(g_stBMS_Info.virtual_cap_percent < 80)
//			SOC_Level = 3;
//		else
//			SOC_Level = 4;
//		
//		if(g_stBMS_Info.virtual_cap_percent <= 98)
//		{
//			if(Flash_state != Last_Flash_state)
//			{
//				if(LED_num >= SOC_Level && LED_num < 5)
//				{
//					LED_num++;
//				}
//				else
//				{
//					LED_num = SOC_Level;
//					BAT_SOC_LEVEL0;
//				}
//				
//				if(LED_num == 0)
//					BAT_SOC_LEVEL0;
//				else if(LED_num == 1)
//					BAT_SOC_LEVEL1;
//				else if(LED_num == 2)
//					BAT_SOC_LEVEL2;
//				else if(LED_num == 3)
//					BAT_SOC_LEVEL3;
//				else if(LED_num == 4)
//					BAT_SOC_LEVEL4;
//				else if(LED_num == 5)
//					BAT_SOC_LEVEL5;
//			}
//			Last_Flash_state = Flash_state;
//		}
//		else
//		    BAT_SOC_LEVEL5;
//	}
//	else					//������ʾ
//	{
//		BAT_SOC_LEVEL0;	
//		BAT_SOC_RED_OFF;	
//		if(g_stBMS_Info.virtual_cap_percent < 10) //С��10%
//		{
//			if(Flash_state)
//			{
////				BAT_SOC_LEVEL1;
//				BAT_SOC_RED_ON;
//			}
//			else
//			{
//				BAT_SOC_LEVEL0;
//				BAT_SOC_RED_OFF;
//			}
//		}
//		else if(g_stBMS_Info.virtual_cap_percent < 20) //С��20%
//			BAT_SOC_RED_ON;
//		else if(g_stBMS_Info.virtual_cap_percent < 30) //С��30%
//		{
//			if(Flash_state)
//				BAT_SOC_LEVEL2;
//			else
//				BAT_SOC_LEVEL1;
//		}
//		else if(g_stBMS_Info.virtual_cap_percent < 40) //С��40%
//			BAT_SOC_LEVEL2;
//		else if(g_stBMS_Info.virtual_cap_percent < 50) //С��50%
//		{
//			if(Flash_state)
//				BAT_SOC_LEVEL3;
//			else
//				BAT_SOC_LEVEL2;
//		}
//		else if(g_stBMS_Info.virtual_cap_percent < 60) //С��60%
//			BAT_SOC_LEVEL3;
//		else if(g_stBMS_Info.virtual_cap_percent < 70) //С��70%
//		{
//			if(Flash_state)
//				BAT_SOC_LEVEL4;
//			else
//				BAT_SOC_LEVEL3;
//		}
//		else if(g_stBMS_Info.virtual_cap_percent < 80) //С��80%
//			BAT_SOC_LEVEL4;
//		else if(g_stBMS_Info.virtual_cap_percent < 90) //С��90%
//		{
//			if(Flash_state)
//				BAT_SOC_LEVEL5;
//			else
//				BAT_SOC_LEVEL4;
//		}
//		else//С�ڵ���100%
//			BAT_SOC_LEVEL5;
//	}
}

/*****************************************************************
* Function Name : Othermarks_Display_Process
* Description   : �������������ͼ����ʾ
* Input         : Flash_flage1����˸���Ʊ�־��������ͼ��ͬ����˸
* Output        : None
* Notes         : 
******************************************************************/	
//static void Othermarks_Display_Process(u8 Flash_flage1)
//{
//#ifdef Test_Mode
//	
//	if(MC_Running_data.MC_Status.LockStatus == LOCKSTATE_LOCK)
//	{
//		Digital_Display(16, Digital_1);
//		Digital_Display(16, Digital_2);
//		
//		TM1652_Dig5 = 0;
//		TM1652_Dig6 = 0;
//	}
//	else
//	{
//		Digital_Display(8, Digital_1);
//		Digital_Display(8, Digital_2);
//		
//		TM1652_Dig5 = 0xFF;
//		TM1652_Dig6 = 0xFF;
//	}
//#else	
//	if(MC_Running_data.MC_Status.LockStatus == LOCKSTATE_UNLOCK)//��������״̬��
//	{
//		BAT_SOC_Display(Flash_flage1);  //������ʾ
//		
//		if(g_bool[HEADLIGHT_ON])
//		{
//			LAMP_ON;
//		}
//		else
//			LAMP_OFF;
//	}
//	else //����״̬����ʾ
//	{
//		TM1652_Dig5 = 0x00;     
//		TM1652_Dig6 = 0x00;
//		ERROE_ON;
//	}
//#endif
//}

/*****************************************************************
* Function Name : Display_Task
* Description   : ��ʾ��������
* Input         : None
* Output        : None
* Notes         : ������ʱʱ��10ms
******************************************************************/	
void Display_Task(void *pvParameters)
{
	static u8 Switch_step = INIT_SHOW_MODE;             //INIT_SHOW_MODE;
	static u8 flash_flage = 0;
	while(1)
	{
		switch(Switch_step)
		{
		case INIT_SHOW_MODE:
			if(Init_Show_Process())                     // running only first time   
				Switch_step = NUM_SEGMENT_MODE; 	
			break;

		case NUM_SEGMENT_MODE:
			//flash_flage = Dis_4LED();                  //�������ʾ
			Switch_step = RGB_MODE;
			break;

		case RGB_MODE:
			Handle_RGB_Control(flash_flage);                   //RGB�ƿ���
			Switch_step = AMBIENTLIGHT_MODE; 
			break;

        case AMBIENTLIGHT_MODE:
			Ambient_Light_Set();                        //��Χ�ƿ���
			Switch_step = NUM_SEGMENT_MODE; 
			break;
                
		default:
			break;
		}				 

		Check_IAP_Mode();
		vTaskDelay(10);      
	}
}

//==================================== The End===============================================/

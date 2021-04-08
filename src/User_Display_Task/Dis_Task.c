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
#define ERROR_TIME2	67		 //开始报错的时间2s
#define ERROR_TIME	16//33	 //报错间隔时间1s（控制报错频率）

/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/* @ Define variables of Display object*/
const u8 CODE[17] = {0x3f,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7F,0x39,0x3F,0x79,0x71,0x00};
	
static u8 TM1652_Dig1 = 0;   //数字1
static u8 TM1652_Dig2 = 0;   //数字1
static u8 TM1652_Dig3 = 0;   //数字2
static u8 TM1652_Dig4 = 0;   //数字2
static u8 TM1652_Dig5 = 0;   //图标  
static u8 TM1652_Dig6 = 0;   //图标
	
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/*****************************************************************
* Function Name : Digital_Display
* Description   : 控制数码管显示数字
* Input         : data：要显示的数字 addr：要显示数字的数码管
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
* Description   : 关数字显示
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
* Description   : 数据送显
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
* Description   : 显示指定的数字
* Input         : None
* Output        : u8:返回同步闪烁标志，是数字和图标同步闪烁
* Notes         : 控制故障码、车辆速度、充电时的电量显示
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
//	if(s_cnt > ERROR_TIME2 + ERROR_TIME) //控制报错频率
//	{
//		s_cnt = ERROR_TIME2;             //控制开始报错时间
//		flag_error_state = !flag_error_state;
//		flag_flash_state = !flag_flash_state;
//	}
	//无故障或IAP模式下 关故障显示
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
//		if(flag_error_state) //显示故障码
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
//		else  //显示速度 和充电过程中的电量
//		{
//			ERROE_OFF;
//			KM_H_OFF;
//			MPH_OFF;
//			Digital_Display_Off();
//			if(g_myself_data.BMS_Data.Charging_State) //在充电，显示电量
//			{
//				shi = g_stBMS_Info.virtual_cap_percent / 10;
//				ge = g_stBMS_Info.virtual_cap_percent % 10;
//			}
//			else //不在充电，显示速度
//			{
//				if(MC_Running_data.MC_Status.LockStatus == LOCKSTATE_LOCK)
//				{
//					shi = 16;
//					ge =  16;
//				}
//				else
//				{
//					running_speed = MC_Running_data.MC_Speed.VehicleSpeed; //只显示速度绝对值
//					
//					shi = running_speed / 10;
//					ge = running_speed % 10;
//					
//					if(MC_Running_data.MC_Speed.VehicleSpeedUnit == KMPH) //公制显示
//					{
//						KM_H_ON;
//						MPH_OFF;
//					}
//					else //英制显示
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

//	Display_Handler(); //数据送显

	return flag_flash_state; //为了使故障码与仪表上的图标同步闪烁
}

/*****************************************************************
* Function Name : Init_Show_Process
* Description   : 开机初始化显示：图标全亮2s
* Input         : None
* Output        : u8:返回开机画面播放完成结果，当初始化完成返回1
* Notes         : 初始化时间2s，数据及图标全部显示，氛围灯不亮
******************************************************************/	
static u8 Init_Show_Process(void )
{
	static u8 s_timer_cn = 0;
	s_timer_cn++;
	if(s_timer_cn == 1)
	{
		TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);          //开启定时器2中断
	    TIM_Cmd(TIM2, ENABLE);  	                        //使能TIM2
		//USART_ITConfig(TM1652_UART, USART_IT_TC, ENABLE);   //开启串口中断
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
		PowerON_RGB();  //开机RGB
	}
	else
	{
		Digital_Display_Off(); //关显示
		TM1652_Dig5 = 0x00;
		TM1652_Dig6 = 0x00;
		
		s_timer_cn = 0;
 #ifdef IOT_METER
//		g_bool[B_LOCKED] = 1; //开机初始化完成后锁车
 #endif
		ALL_RGB_OFF();
		g_bool[B_INIT_SHOW_OK] = 1; //开机动画播放完成
		Display_Handler(); //数据送显
		return 1;
	}
	Display_Handler(); //数据送显
#endif
	return 0;
}

/*****************************************************************
* Function Name : BAT_SOC_Display
* Description   : 电量显示控制
* Input         : Flash_state：闪烁控制标志，与其他图标同步闪烁
* Output        : None
* Notes         : 
******************************************************************/	
static void BAT_SOC_Display(u8 Flash_state)
{
//	u8 SOC_Level = 0;
//	static u8 LED_num = 0;
//	static u8 Last_Flash_state = 0;

//	if(g_myself_data.BMS_Data.Charging_State) //车辆正在充电
//	{
//        //电量灯闪烁
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
//	else					//电量显示
//	{
//		BAT_SOC_LEVEL0;	
//		BAT_SOC_RED_OFF;	
//		if(g_stBMS_Info.virtual_cap_percent < 10) //小于10%
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
//		else if(g_stBMS_Info.virtual_cap_percent < 20) //小于20%
//			BAT_SOC_RED_ON;
//		else if(g_stBMS_Info.virtual_cap_percent < 30) //小于30%
//		{
//			if(Flash_state)
//				BAT_SOC_LEVEL2;
//			else
//				BAT_SOC_LEVEL1;
//		}
//		else if(g_stBMS_Info.virtual_cap_percent < 40) //小于40%
//			BAT_SOC_LEVEL2;
//		else if(g_stBMS_Info.virtual_cap_percent < 50) //小于50%
//		{
//			if(Flash_state)
//				BAT_SOC_LEVEL3;
//			else
//				BAT_SOC_LEVEL2;
//		}
//		else if(g_stBMS_Info.virtual_cap_percent < 60) //小于60%
//			BAT_SOC_LEVEL3;
//		else if(g_stBMS_Info.virtual_cap_percent < 70) //小于70%
//		{
//			if(Flash_state)
//				BAT_SOC_LEVEL4;
//			else
//				BAT_SOC_LEVEL3;
//		}
//		else if(g_stBMS_Info.virtual_cap_percent < 80) //小于80%
//			BAT_SOC_LEVEL4;
//		else if(g_stBMS_Info.virtual_cap_percent < 90) //小于90%
//		{
//			if(Flash_state)
//				BAT_SOC_LEVEL5;
//			else
//				BAT_SOC_LEVEL4;
//		}
//		else//小于等于100%
//			BAT_SOC_LEVEL5;
//	}
}

/*****************************************************************
* Function Name : Othermarks_Display_Process
* Description   : 数字以外的其他图标显示
* Input         : Flash_flage1：闪烁控制标志，与其他图标同步闪烁
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
//	if(MC_Running_data.MC_Status.LockStatus == LOCKSTATE_UNLOCK)//车辆解锁状态下
//	{
//		BAT_SOC_Display(Flash_flage1);  //电量显示
//		
//		if(g_bool[HEADLIGHT_ON])
//		{
//			LAMP_ON;
//		}
//		else
//			LAMP_OFF;
//	}
//	else //锁车状态关显示
//	{
//		TM1652_Dig5 = 0x00;     
//		TM1652_Dig6 = 0x00;
//		ERROE_ON;
//	}
//#endif
//}

/*****************************************************************
* Function Name : Display_Task
* Description   : 显示控制任务
* Input         : None
* Output        : None
* Notes         : 任务延时时间10ms
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
			//flash_flage = Dis_4LED();                  //数码管显示
			Switch_step = RGB_MODE;
			break;

		case RGB_MODE:
			Handle_RGB_Control(flash_flage);                   //RGB灯控制
			Switch_step = AMBIENTLIGHT_MODE; 
			break;

        case AMBIENTLIGHT_MODE:
			Ambient_Light_Set();                        //氛围灯控制
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

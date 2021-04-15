/* Includes ------------------------------------------------------------------*/
#include "Control.h"
#include "HandleTemperature.h"
#include "TurnLightProcess.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/



/*****************************************************************
* Function Name : COM_Overtime_Check_Pro
* Description   : 通讯超时检测
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
static void COM_Overtime_Check_Pro(void)
{
	static u16 m_ErrCnt = 0;         //通讯超时计数

	m_ErrCnt++;
	if(g_bool[B_ECU_ACK] == 1 || g_bool[B_IN_IAPMODE] == 1)  //有数据返回，或IAP，清通讯报警
	{
		m_ErrCnt = 0;
		ClearErrorByte(MINI_ERROR_MCU_COM);                  //清仪表自身报的通信故障
        g_myself_data.CommuTimeout = 0;
	}
	if(m_ErrCnt > 400)  //1s超时
	{
		m_ErrCnt = 500; //500ms
		SetErrorByte(MINI_ERROR_MCU_COM);            //与主控通信故障
        g_myself_data.CommuTimeout = 1;
	}
}

/*****************************************************************
* Function Name : Wireless_Power_Ctr_Handler
* Description   : 无线充电器开关控制
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void Wireless_Power_Ctr_Handler(void)
{
	static u8 Pulse_num = 0;                     //无线充故障检测脚输入脉冲计数
	static u16 Time_cnt = 0;                     //无线充故障检测计时
	static u16 Time_cnt1 = 0;                    //无线充无故障计时
	static u8 Last_input_state = 0;              //无限充故障检测脚上次输入状态
	static u8 Wireless_Charge_Vol_TSD = 210;    //无线充基准电压21V
	static u8 Wireless_Charge_Vol_Offset = 30;  //无线充电压最大偏移量3V
	static u16 s_cnt = 0;

#ifdef DEBUG_1
    g_myself_data.Scooter_Info.WirelessChargerSet = ON;
#endif
    g_myself_data.Handle_Bar_Info.WirelessChargerStatus = WIRELESS_STATUS_OFF;
    
	//无线充故障检测
	if(Last_input_state == 1 && gpioGetInput(GpioInWirelessFB) == 0)
	{
		Pulse_num++;
	}
	if(Pulse_num > 0)
	{
		Time_cnt1 = 0;
		Time_cnt++;
		if(Time_cnt > 600) //3s，抓到3个以上上升沿，认为无线充在报错
		{
			if(Pulse_num >= 3)
			{
				g_bool[B_WIRELESS_ERR] = 1;
                g_myself_data.Handle_Bar_Info.WirelessChargerStatus |= WIRELESS_STATUS_ERROR;
			}
			Time_cnt = 0;
			Pulse_num = 0;
		}
	}
	else
	{
		Time_cnt = 0;
		Time_cnt1++;
		if(Time_cnt1 >600) //连续3s没有捕捉到上升沿，认为无线充无故障
		{
			Time_cnt1 = 0;
			g_bool[B_WIRELESS_ERR] = 0;
		}
	}
	Last_input_state = gpioGetInput(GpioInWirelessFB);
	
	//无线充电源开关逻辑
	if(g_myself_data.Scooter_Info.WirelessChargerSet == OFF || g_myself_data.Scooter_Info.ControllerStatus == LOCK)
    {
        gpioSetOutput(GpioOutWirelessChargePowCtrl, false); //关闭电源
    }
	else
	{
		 if(g_bool[B_WIRELESS_ERR])
         {
             gpioSetOutput(GpioOutWirelessChargePowCtrl, false); //关闭电源
         } 
		 else
		 {
			 gpioSetOutput(GpioOutWirelessChargePowCtrl, true); //打开电源
			 s_cnt++;
             g_myself_data.Handle_Bar_Info.WirelessChargerStatus |= WIRELESS_STATUS_ON;
		 }
	}
    
	//无线充供电电压异常，关无线充供电电源
	if(s_cnt >400)
	{
		s_cnt = 200;
		if(ABS(g_wireless_charger_vol - Wireless_Charge_Vol_TSD) > Wireless_Charge_Vol_Offset)
        {
            gpioSetOutput(GpioOutWirelessChargePowCtrl, false); //关闭电源
            g_myself_data.Handle_Bar_Info.WirelessChargerStatus |= WIRELESS_STATUS_DIFF_VOL;
        }
	}
}

/*****************************************************************
* Function Name : NFC_Card_ID_Pro
* Description   : 上传NFC卡号
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
static void NFC_Card_ID_Pro(void)
{
//    static u8 timeOut = 0, sendFlag = 0;
//    
//    if(++timeOut > 150)       // 150 * 20 = 1000
//    {
//        timeOut = 200;
//        sendFlag = 0;
//    }
//    
    if(PICC_A.Have_get_uid_status)
    {
//        sendFlag = 1;
//        timeOut = 0;
//        
		PICC_A.Have_get_uid_status =0;
        _my_id.s_upload_timeout = 5;
        PushMiniFrame(MY_ID, ECU_ID, _my_id.s_card_id_len, CMD_NFC_UPDATE, 0, (u8*)&_my_id.s_card_id, COM_SCOOTER); 
        return;
    }
    else
    {
		if(g_bool[B_NFC_UPDATE_OK] == 1)
		{
			g_bool[B_NFC_UPDATE_OK] = 0;
			_my_id.s_upload_timeout = 0;
		}
        if( _my_id.s_upload_timeout > 0)
        {
            _my_id.s_upload_timeout --;
            PushMiniFrame(MY_ID, ECU_ID, _my_id.s_card_id_len, CMD_NFC_UPDATE, 0, (u8*)&_my_id.s_card_id, COM_SCOOTER);  
            return;
        }
        _my_id.s_card_id = 0;
    }
}

/*****************************************************************
* Function Name : Beep_Ctrl_Pro
* Description   : 蜂鸣器控制函数
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void Beep_Ctrl_Pro(void)
{
    static u8 s_oldBeepCmd = 0;
    static u8 s_beepRepeat = 0;
    static u8 s_beepOnCnt = 0;
    static u8 s_beepOffCnt = 0;

    if(g_myself_data.Scooter_Info.BeepTime != 0 && s_oldBeepCmd == 0)
    {
        g_myself_data.Handle_Bar_Info.Beep = 1;
        switch(g_myself_data.Scooter_Info.BeepTime)
        {
        case MINI_BEEP_CMD_ONCE_S:
            s_beepOnCnt = (80/5);
            break;
        case MINI_BEEP_CMD_ONCE_L:
            s_beepOnCnt = (400/5);
            break;
        case MINI_BEEP_CMD_ONCE_LL:
            s_beepOnCnt = (1000/5);
            break;
        case MINI_BEEP_CMD_DOUBLE_S:
            s_beepRepeat = 1;
            s_beepOnCnt = (30/5);
            break;
        default:
            break;
        }
    }
    s_oldBeepCmd = g_myself_data.Scooter_Info.BeepTime;

    if((s_beepRepeat || s_beepOnCnt) && s_beepOffCnt == 0)
    {
        gpioSetOutput(GpioOutBeep, true);

        if(s_beepOnCnt != 0)
        {
            s_beepOnCnt--;
        }
        else if(s_beepRepeat != 0)
        {
            s_beepRepeat--;
            s_beepOffCnt = (40/5);
            s_beepOnCnt = (30/5);
        }
    }
    else
    {
        gpioSetOutput(GpioOutBeep, false);
        if(s_beepOffCnt != 0)
        {
            s_beepOffCnt--;
        }
        else
        {
            g_myself_data.Scooter_Info.BeepTime = 0;
            g_myself_data.Handle_Bar_Info.Beep = 0;
        }
    }
}


/*****************************************************************
* Function Name : HeadLedProcess
* Description   : 前灯控制
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void FrontLightProcess(void)
{
#ifdef DEBUG_1
    gpioSetOutput(GpioOutFrontLedCtrl, true);
    return;
#endif
    if(g_myself_data.Scooter_Info.FrontlightBrightness == 0)
    {
        gpioSetOutput(GpioOutFrontLedCtrl, false);
    }
    else
    {
        gpioSetOutput(GpioOutFrontLedCtrl, true);
    }
}


/*****************************************************************
* Function Name : NFC_Card_ID_Pro
* Description   : 上传NFC卡号
* Input         : None
* Output        : None
* Notes         :
******************************************************************/

/**@brief Time slice distribution*/
#define NORM_TASK_NUM  10
static TASK_COMPONENTS TaskComps[] =
{
	{0, 20, 20,  Query_Send_Data_Pro},      //仪表主动发送数据：油门刹车、查询、NFC解锁锁车数据 周期20ms
	{0, 10, 10,  Get_RealVaule},            //获取油门刹车霍尔值
	{0, 5, 5,    COM_Overtime_Check_Pro},   //控制器、中继板与仪表通讯超时检测
    {0, 5, 5,    Handle_Temp_Ctr},          //把套加热控制函数

	{0, 5, 5,    LPCD_Read},                //读卡 NFC
	{0,20,20,    NFC_Card_ID_Pro},          //上传NFC卡号
	{0, 5, 5,    Wireless_Power_Ctr_Handler}, //无线充控制
	{0, 5, 5,    Beep_Ctrl_Pro},            //蜂鸣器控制函数
    
    {0, 5, 5,    turnLightProcess},         //转向灯进程
    {0, 50, 50,  FrontLightProcess},           //大灯控制
};

/*****************************************************************
* Function Name : Normal_Tasks_Process
* Description   : 时间片控制函数调用
* Input         : None
* Output        : None
* Notes         : Function for act normal tasks
******************************************************************/
static void Normal_Tasks_Process(void)
{
    u8 i;
    for (i=0; i < NORM_TASK_NUM; i++)
    {
        if (TaskComps[i].running_state)
        {
            TaskComps[i].running_taskhook();
            TaskComps[i].running_state = 0;
        }
    }
}

/*****************************************************************
* Function Name : Normal_Tasks_Remarks
* Description   : 时间片带你用函数状态更新
* Input         : None
* Output        : None
* Notes         : Function for update normal tasks rotation status
******************************************************************/
void Normal_Tasks_Remarks(void)
{
    u8 i;
    for (i=0; i < NORM_TASK_NUM; i++)
    {
        if (TaskComps[i].runing_time_cnt)
        {
            TaskComps[i].runing_time_cnt--;
            if (TaskComps[i].runing_time_cnt == 0)
            {
                TaskComps[i].runing_time_cnt = TaskComps[i].runing_period;
                TaskComps[i].running_state = 1;
            }
        }
    }
}

/*****************************************************************
* Function Name : Control_Task
* Description   : 控制任务
* Input         : None
* Output        : None
* Notes         : Function for update normal tasks rotation status
******************************************************************/
void Control_Task(void *pvParameters)
{
	while(1)
	{
		 Normal_Tasks_Remarks();
		 Normal_Tasks_Process();
		 vTaskDelay(1);      
	}
}

//====================================================================================//

//==================================== The End===============================================/

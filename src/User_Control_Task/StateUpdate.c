/* Includes ------------------------------------------------------------------*/
#include "StateUpdate.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

static uint8_t powerCtrl(u8 status)
{
    uint8_t rec = 0;

    gpioSetOutput(GpioOutPowCtrl, status);
    gpioSetOutput(GpioOutGPSPowCtrl, status);
    gpioSetOutput(GpioOut5VSysPowCtrl, status);

    return rec;
}

/*****************************************************************
* Function Name : Query_Send_Data_Pro
* Description   : 数据发送（油门刹车、查询、NFC解锁锁车数据）
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void Query_Send_Data_Pro(void) //20ms
{
    static uint8_t send100MsCnt = 0;
    
	if(!g_bool[B_IN_IAPMODE])
	{
        if(++send100MsCnt >= 5)
        {
            send100MsCnt = 0;

            memcpy((uint8_t *)&g_myself_data.Handle_Bar_Info.Version_A, (uint8_t *)&g_myself_data.DIS_Version_sw.Dis_SoftwareVersion, 2);
            g_myself_data.Handle_Bar_Info.FrameLen = sizeof(HANDLE_BAR_INFO) - 1;

            //发送油门数据
            PushMiniFrame(MY_ID, ECU_ID, sizeof(HANDLE_BAR_INFO), CMD_SCO_CTL, 0, (u8*)&g_myself_data.Handle_Bar_Info, 0);	
        }
        else
        {
            g_myself_data.Handle_Bar_Info.Version_A = 0;
            g_myself_data.Handle_Bar_Info.Version_B = 0;//版本号占两个字节;
            g_myself_data.Handle_Bar_Info.FrameLen = sizeof(HANDLE_BAR_INFO) - 1;
            
            //发送油门数据
            PushMiniFrame(MY_ID, ECU_ID, sizeof(HANDLE_BAR_INFO), CMD_SCO_CTL_NR, 0, (u8*)&g_myself_data.Handle_Bar_Info, 0);	
        }

        //TODO:test
        g_myself_data.Scooter_Info.ControllerStatus = UNLOCK;
        g_myself_data.CommuTimeout = 0;
        //TODO:end test
        if(g_myself_data.Scooter_Info.ControllerStatus == LOCK || g_myself_data.CommuTimeout == 1)
        {
            powerCtrl(OFF);//关锁或者通信超时，关闭仪表主电
        }
        else
        {
            powerCtrl(ON);//打开设备主电
        }
	}
    else
    {
        send100MsCnt = 100;

        powerCtrl(OFF);//升级过程关闭外设电源
    }
}

/*****************************************************************
* Function Name : Check_IAP_Mode
* Description   : 检测是否IAP模式
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void Check_IAP_Mode(void)
{
	static u16 s_cntIAP = 0;
	
	if(g_bool[B_UPD_IAP])
	{
		g_bool[B_IN_IAPMODE] = 1;
		s_cntIAP = 3000;		 //3s
	}
	else
	{
		if(s_cntIAP > 0)
			s_cntIAP--;
		else
		{
			g_bool[B_IN_IAPMODE] = 0;
		}
	}
	g_bool[B_UPD_IAP] = 0;
}

//====================================================================================//

//==================================== The End===============================================/

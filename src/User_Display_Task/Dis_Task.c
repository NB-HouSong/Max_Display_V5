/******************** (C) COPYRIGHT 2020 Ninebot *******************************
* File Name     : Dis_Task.c
* Author        : meihua
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "tm1650.h"
#include "iic.h"
#include "TurnLightProcess.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
#define Turn_Sig_light  0x08
#define Green_LED                      0x01    //�̵�
#define Red_LED                        0x02    //���
/*****************************************************************
* Function Name : Dis_4LED
* Description   : ��ʾָ�� LED ��
* Input         : mode 0:���� 1������
* Output        : u8:����ͬ����˸��־�������ֺ�ͼ��ͬ����˸
******************************************************************/	
u8 Dis_4LED(u8 mode) //30ms
{
    static u8 leftBroknCnt = 0, rightBroknCnt = 0;
	u8 led_on_buf[7]={0};
    u8 flash_flage = 0;

    memset(led_on_buf, 0, 6);
    
    if(mode == 1)
    {
        memset(led_on_buf, 0xff, 7);
        IIC_PushFrame(led_on_buf);
        return 0;
    }

    memcpy((u8*)led_on_buf,(u8*)&g_myself_data.Scooter_Info.A_LED, 4);
    
    /**
    ԭʼЭ����ɫΪ 1����ɫ 2����ɫ 4����ɫ
    V5 Ӳ����λΪ��1����ɫ 2����ɫ 4����ɫ 
    ������Ϊ������µ�Ӳ������ת��
    */
    for(int i = 0; i < 4; i++)
    {
        if((led_on_buf[i] & 0x04) == 0x04)
        {
            led_on_buf[i] = ((led_on_buf[i] & 0x03) << 1) + 1;
        }
        else
        {
            led_on_buf[i] = ((led_on_buf[i] & 0x03) << 1);
        }
    }

    if((g_myself_data.Handle_Bar_Info.TurnSignalLightStatus & LED_LEFT_BROKEN) == LED_LEFT_BROKEN)
    {
        if(++leftBroknCnt > 10)
        {
            led_on_buf[4] |= Turn_Sig_light;
            flash_flage = 1;
        }
        else if(++leftBroknCnt > 20)
        {
            leftBroknCnt = 0;
            led_on_buf[4] &= ~Turn_Sig_light;
        }
    }
    else if( (g_myself_data.Handle_Bar_Info.TurnSignalLightStatus & LED_LEFT_ON) == LED_LEFT_ON && 
             (g_bool[B_TURN_LIGHT_FLASH] == ON) )

    {
        led_on_buf[4] |= Turn_Sig_light;
        leftBroknCnt = 0;
        flash_flage = 1;
    }
    else
    {
        leftBroknCnt = 0;
    }
    
    if((g_myself_data.Handle_Bar_Info.TurnSignalLightStatus & LED_RIGHT_BROKEN) == LED_RIGHT_BROKEN)
    {
        if(++rightBroknCnt > 10)
        {
            led_on_buf[5] |= Turn_Sig_light;
            flash_flage = 1;
        }
        else if(++rightBroknCnt > 20)
        {
            rightBroknCnt = 0;
            led_on_buf[5] &= ~Turn_Sig_light;
        }
    }
    else if( (g_myself_data.Handle_Bar_Info.TurnSignalLightStatus & LED_RIGHT_ON) == LED_RIGHT_ON &&
             (g_bool[B_TURN_LIGHT_FLASH] == ON) )
    {
        led_on_buf[5] |= Turn_Sig_light;
        rightBroknCnt = 0;
        flash_flage = 1;
    }
    else
    {
        rightBroknCnt = 0;
    }

    IIC_PushFrame(led_on_buf);			//����

    return flash_flage;
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

	if(s_timer_cn < 49) //0.5
	{
		Dis_4LED(1);

		PowerON_RGB();  //����RGB
	}
	else
	{
		Dis_4LED(0);

		ALL_RGB_OFF();
		return 1;
	}

	return 0;
}

/*****************************************************************
* Function Name : Display_Task
* Description   : ��ʾ��������
* Input         : None
* Output        : None
* Notes         : ������ʱʱ��10ms
******************************************************************/	
void Display_Task(void *pvParameters)
{
//	static u8 Switch_step = INIT_SHOW_MODE;             //INIT_SHOW_MODE;
	static u8 flash_flage = 0;
    static u8 dis_init = 0;
    
	while(1)
	{
//		switch(Switch_step)
//		{
//		case INIT_SHOW_MODE:
//			if(Init_Show_Process())                     // running only first time   
//				Switch_step = NUM_SEGMENT_MODE; 	
//			break;

//		case NUM_SEGMENT_MODE:
//			flash_flage = Dis_4LED(0);                  //�������ʾ
//			Switch_step = RGB_MODE;
//			break;

//		case RGB_MODE:
//			Handle_RGB_Control(flash_flage);                   //RGB�ƿ���
//			Switch_step = AMBIENTLIGHT_MODE; 
//			break;

//        case AMBIENTLIGHT_MODE:
//			Ambient_Light_Set();                        //��Χ�ƿ���
//			Switch_step = NUM_SEGMENT_MODE; 
//			break;
//                
//		default:
//			break;
//		}

//        //TODO :TEST
//        g_myself_data.Ambient_Light.AmbientLightMode = BREATHE_LED;
//        Ambient_light_object.running_color = 7;
//        //g_myself_data.Ambient_Light.AmbientLightColor = 7;
//        g_myself_data.Ambient_Light.AmbientLightLux = 100;
//        //END TODO :TEST

#ifdef DEBUG_1
        g_myself_data.Ambient_Light.AmbientLightMode = BREATHE_LED;
        Ambient_light_object.running_color = 7;
        g_myself_data.Ambient_Light.AmbientLightLux = 100;
        
        g_myself_data.Scooter_Info.A_LED = 1;
        g_myself_data.Scooter_Info.B_LED = 1;
        g_myself_data.Scooter_Info.C_LED = 1;
        g_myself_data.Scooter_Info.D_LED = 1;
        
#endif

        if(dis_init == 0)
        {
            dis_init = Init_Show_Process();         // running only first time   
        }

        flash_flage = Dis_4LED(0);                  //�������ʾ
        Handle_RGB_Control(flash_flage);            //RGB�ƿ���
        //Ambient_Light_Set();                        //��Χ�ƿ���

		Check_IAP_Mode();
		vTaskDelay(20);
	}
}

//==================================== The End===============================================/

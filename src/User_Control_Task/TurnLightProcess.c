#include "TurnLightProcess.h"

#define LED_STATE_HIGHT_LIMIT  2000     // 电压值大于此值认为转向灯短路  大约 1.2V，   正常值在 0.7V
#define LED_STATE_LOW_LIMIT    500      // 电压值小于此值认为转向灯开路  大约 0.4V   正常值在 0.7V

typedef enum E_LIGHT_STATE
{
    LIGHT_STATE_IDLE,
    LIGHT_STATE_LEFT_ON,
    LIGHT_STATE_RIGHT_ON,
    LIGHT_STATE_BROKEN
} LIGHT_STATE;

static uint8_t   g_turnLightStatus = 0;

void turnLightProcess(void)
{
    static uint16_t s_cnt_L = 0;
    static uint16_t s_cnt_R = 0;
    static uint16_t s_cnt_adc_start = 0;
    static uint8_t statr_cnt = 0;

    static LIGHT_STATE lightState = LIGHT_STATE_IDLE;

    //判断车辆状态和通信状态
    if(g_myself_data.Scooter_Info.ControllerStatus == UNLOCK && g_myself_data.CommuTimeout == 0)
    {
        switch(lightState)
        {
        case LIGHT_STATE_IDLE:
            gpioSetOutput(GpioOutRightLightCtrl, false);
            gpioSetOutput(GpioOutLeftLightCtrl, false);
        
            g_turnLightStatus &= ~LED_LEFT_ON;
            g_turnLightStatus &= ~LED_RIGHT_ON;
            g_bool[B_TURN_LIGHT_FLASH] = OFF;

            if( gpioGetInput(GpioInLeftButton) == true )
            {
                if(statr_cnt++> 4)
                {
                    statr_cnt = 5;
                    lightState = LIGHT_STATE_LEFT_ON;
                }
            }
            else if( gpioGetInput(GpioInRightButton) == true )
            {
                if(statr_cnt++> 4)
                {
                    statr_cnt = 5;
                    lightState = LIGHT_STATE_RIGHT_ON;
                }
            }
            else
            {
                statr_cnt = 0;
            }
            break;
        case LIGHT_STATE_LEFT_ON:
            if( gpioGetInput(GpioInLeftButton) == true )
            {
                g_turnLightStatus |= LED_LEFT_ON;

                if(++s_cnt_L % 100 == 0)
                {
                    gpioSetToggle(GpioOutLeftLightCtrl);
                    s_cnt_adc_start = 0;                                    //adc 开始采样计时清零
                }
                if(gpioGetOutput(GpioOutLeftLightCtrl) == true)             //判断转向灯控制引脚是否被置位
                {
                    g_bool[B_TURN_LIGHT_FLASH] = ON;
//                    if(s_cnt_adc_start++ > 4)           //adc 延时20ms 取值
//                    {
//                        s_cnt_adc_start = 5;
//                        //读取 右转向灯电压
//                        if( ADC_Get_ConversionValue(AdcChanelLeftLightOverCur) > LED_STATE_HIGHT_LIMIT ||
//                            ADC_Get_ConversionValue(AdcChanelLeftLightOverCur) < LED_STATE_LOW_LIMIT )
//                        {
//                            g_turnLightStatus |= LED_LEFT_BROKEN;

//                            lightState = LIGHT_STATE_BROKEN;
//                        }
//                        else
//                        {
//                            g_turnLightStatus &= ~LED_LEFT_BROKEN;
//                        }
//                    }
                }
                else
                {
                    g_bool[B_TURN_LIGHT_FLASH] = OFF;
                }
            }
            else
            {
                lightState = LIGHT_STATE_IDLE;
            }
            break;
        case LIGHT_STATE_RIGHT_ON:
            if( gpioGetInput(GpioInRightButton) == true )
            {
                g_turnLightStatus |= LED_RIGHT_ON;
                if(++s_cnt_R % 100 == 0)
                {
                    gpioSetToggle(GpioOutRightLightCtrl);
                    s_cnt_adc_start = 0;                    //adc 开始采样计时清零
                }

                if(gpioGetOutput(GpioOutRightLightCtrl) == true)                //判断转向灯控制引脚是否被置位
                {
                    g_bool[B_TURN_LIGHT_FLASH] = ON;
//                    if(s_cnt_adc_start++ > 4)               //adc 延时20ms 取值
//                    {
//                        s_cnt_adc_start = 5;
//                        //读取 右转向灯电压
//                        if( ADC_Get_ConversionValue(AdcChanelRightLightOverCur) > LED_STATE_HIGHT_LIMIT ||
//                            ADC_Get_ConversionValue(AdcChanelRightLightOverCur) < LED_STATE_LOW_LIMIT )
//                        {
//                            g_turnLightStatus |= LED_RIGHT_BROKEN;

//                            lightState = LIGHT_STATE_BROKEN;
//                        }
//                        else
//                        {
//                            g_turnLightStatus &= ~LED_RIGHT_BROKEN;
//                        }
//                    }
                }
                else
                {
                    g_bool[B_TURN_LIGHT_FLASH] = OFF;
                }
            }
            else
            {
                lightState = LIGHT_STATE_IDLE;
            }
            break;
        case LIGHT_STATE_BROKEN:
            // 转向灯异常，关闭转向灯控制引脚
            gpioSetOutput(GpioOutRightLightCtrl, false);
            gpioSetOutput(GpioOutLeftLightCtrl, false);
            
            if(gpioGetInput(GpioInLeftButton) == false && gpioGetInput(GpioInRightButton) == false)
            {
                lightState = LIGHT_STATE_IDLE;
            }
            break;
        default:break;
        }
    }
    else
    {
        lightState = LIGHT_STATE_IDLE;
        g_turnLightStatus &= (~LED_ALL_ON);
    }
    
    g_myself_data.Handle_Bar_Info.TurnSignalLightStatus = g_turnLightStatus;
}

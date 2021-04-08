#include "TurnLightProcess.h"

#define LED_STATE_HIGHT_LIMIT  1489     // ��ѹֵ���ڴ�ֵ��Ϊת��ƶ�·  ��Լ 1.2V��   ����ֵ�� 0.7V
#define LED_STATE_LOW_LIMIT    496      // ��ѹֵС�ڴ�ֵ��Ϊת��ƿ�·  ��Լ 0.4V   ����ֵ�� 0.7V

typedef enum E_LIGHT_STATE
{
    LIGHT_STATE_IDLE,
    LIGHT_STATE_LEFT_ON,
    LIGHT_STATE_RIGHT_ON,
    LIGHT_STATE_BROKEN
} LIGHT_STATE;

typedef enum LED_STATUS_enum
{
    LED_ALL_OFF         = 0x00,
    LED_ALL_ON          = 0x03,
    LED_LEFT_ON         = 0x01,
    LED_RIGHT_ON        = 0x02,
    LED_LEFT_BROKEN     = 0x04,
    LED_RIGHT_BROKEN    = 0x08
} LED_STATUS;

static uint8_t   g_turnLightStatus = 0;

void turnLightProcess(void)
{
    static uint16_t s_cnt_L = 0;
    static uint16_t s_cnt_R = 0;
    static uint16_t s_cnt_adc_start = 0;
    static uint8_t statr_cnt = 0;

    static LIGHT_STATE lightState = LIGHT_STATE_IDLE;


    //�жϳ���״̬��ͨ��״̬
    if(g_myself_data.Scooter_Info.ControllerStatus == UNLOCK && g_myself_data.CommuTimeout == 0)
    {
        switch(lightState)
        {
        case LIGHT_STATE_IDLE:
            gpioSetOutput(GpioOutRightLightCtrl, false);
            gpioSetOutput(GpioOutLeftLightCtrl, false);
        
            g_turnLightStatus &= ~LED_LEFT_ON;
            g_turnLightStatus &= ~LED_RIGHT_ON;
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
                    s_cnt_adc_start = 0;                //adc ��ʼ������ʱ����
                }
                if(gpioGetOutput(GpioOutLeftLightCtrl) == true)             //�ж�ת��ƿ��������Ƿ���λ
                {
                    if(s_cnt_adc_start++ > 4)           //adc ��ʱ20ms ȡֵ
                    {
                        s_cnt_adc_start = 5;
                        //��ȡ ��ת��Ƶ�ѹ
                        if( ADC_Get_ConversionValue(AdcChanelLeftLightOverCur) > LED_STATE_HIGHT_LIMIT ||
                            ADC_Get_ConversionValue(AdcChanelLeftLightOverCur) < LED_STATE_LOW_LIMIT )
                        {
                            g_turnLightStatus |= LED_LEFT_BROKEN;

                            lightState = LIGHT_STATE_BROKEN;
                        }
                        else
                        {
                            g_turnLightStatus &= ~LED_LEFT_BROKEN;
                        }
                    }
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
                    s_cnt_adc_start = 0;                    //adc ��ʼ������ʱ����
                }

                if(gpioGetOutput(GpioOutRightLightCtrl) == true)                //�ж�ת��ƿ��������Ƿ���λ
                {
                    if(s_cnt_adc_start++ > 4)               //adc ��ʱ20ms ȡֵ
                    {
                        s_cnt_adc_start = 5;
                        //��ȡ ��ת��Ƶ�ѹ
                        if( ADC_Get_ConversionValue(AdcChanelRightLightOverCur) > LED_STATE_HIGHT_LIMIT ||
                            ADC_Get_ConversionValue(AdcChanelRightLightOverCur) < LED_STATE_LOW_LIMIT )
                        {
                            g_turnLightStatus |= LED_RIGHT_BROKEN;

                            lightState = LIGHT_STATE_BROKEN;
                        }
                        else
                        {
                            g_turnLightStatus &= ~LED_RIGHT_BROKEN;
                        }
                    }
                }
            }
            else
            {
                lightState = LIGHT_STATE_IDLE;
            }
            break;
        case LIGHT_STATE_BROKEN:
            // ת����쳣���ر�ת��ƿ�������
            gpioSetOutput(GpioOutRightLightCtrl, false);
            gpioSetOutput(GpioOutLeftLightCtrl, false);

            if(gpioGetInput(GpioInLeftButton) == true && gpioGetInput(GpioInRightButton) == true)
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
}

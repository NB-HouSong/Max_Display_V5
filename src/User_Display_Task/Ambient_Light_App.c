/***********************************************************************************************************
  * @file     Ambient_Light_App.c
  * @author   qiqi.zhao
  * @version  V1.0
  * @date     2020-3-10
  * @Modifyby meihua.le
  * @brief    Ambient light process
  **********************************************************************************************************/
#include "Ambient_Light_App.h"

/**@brief    ambient light process object
*@details
*/
_AMBIENT_LIGHT_OB  Ambient_light_object;

void Ambient_Light_Breathe_Pro(_AMBIENT_LIGHT_OB *p_this)
{
    static u16 s_period_cnt = 0;
    static u16 delay_cnt = 0;
    if( ++ s_period_cnt % p_this->running_period == OFF) //running period n*5ms
    {
        switch(p_this->running_polar)
        {
        case LOWEST_POLAR:
            if(++delay_cnt > 5)
            {
                delay_cnt = 0;
                p_this->running_polar = INCREASE_POLAR;
            }
            break;

        case HIGHTST_POLAR:
            if(++delay_cnt > 5)
            {
                delay_cnt = 0;
                p_this->running_polar = DECREASE_POLAR;
            }
            break;

        case INCREASE_POLAR:
            p_this->running_cnt += AMBIENT_ADJUST_STEP;
            if(p_this->running_cnt >= p_this->running_lux)
            {
                if(p_this->running_mode == BREATHE_LED)
                    p_this->running_polar = HIGHTST_POLAR;
                else
                {
                    p_this->running_color ++;
                    if(p_this->running_color > 7 )
                    {
                        p_this->running_color = 1;
                    }
                    p_this->running_polar = DECREASE_POLAR;
                }
                p_this->running_cnt = p_this->running_lux;
            }
            break;

        case DECREASE_POLAR:
            p_this->running_cnt -= AMBIENT_ADJUST_STEP;
            if(p_this->running_cnt <= 2)
            {
                if(p_this->running_mode == BREATHE_LED)
                    p_this->running_polar = LOWEST_POLAR;
                else p_this->running_polar = INCREASE_POLAR;
                p_this->running_cnt = 2;
            }
            break;

        default:
            break;
        }
        p_this->control_data.s_rled_value = ((p_this->running_color&0x02)>>1)*p_this->control_data.s_light_freq*p_this->running_cnt/100;
        p_this->control_data.s_gled_value = ((p_this->running_color&0x01))*p_this->control_data.s_light_freq*p_this->running_cnt/100;
        p_this->control_data.s_bled_value = ((p_this->running_color&0x04)>>2)*p_this->control_data.s_light_freq*p_this->running_cnt/100;
        s_period_cnt = 0;
    }
}

void Ambient_Light_ALL_Pro(_AMBIENT_LIGHT_OB *p_this)
{
    static u16 s_period_cnt = 0;
    static u16 delay_cnt = 0;
    static u16 s_color = 0;

    if( ++s_period_cnt % p_this->running_period == OFF)//running period n*5ms
    {
        switch(p_this->running_polar)
        {
        case LOWEST_POLAR:
            if(++delay_cnt > 5)
            {
                delay_cnt = 0;
                p_this->running_polar = INCREASE_POLAR;
            }
            break;

        case HIGHTST_POLAR:
            if(++delay_cnt > 5)
            {
                delay_cnt = 0;
                p_this->running_polar = DECREASE_POLAR;
            }
            break;

        case INCREASE_POLAR:
            p_this->running_cnt += AMBIENT_ADJUST_STEP;
            if(p_this->running_cnt >= p_this->running_lux)
            {
                p_this->running_polar = HIGHTST_POLAR;
                p_this->running_cnt = p_this->running_lux;
            }
            break;

        case DECREASE_POLAR:
            p_this->running_cnt -= AMBIENT_ADJUST_STEP;
            if(p_this->running_cnt <= 2)
            {
                s_color++;
                if(s_color >= 8)
                {
                    s_color = 1;
                }
                p_this->running_polar = LOWEST_POLAR;
                p_this->running_cnt = 2;
            }
            break;

        default:
            break;
        }

        p_this->control_data.s_rled_value = ((s_color&0x02)>>1)*p_this->control_data.s_light_freq*p_this->running_cnt/100;
        p_this->control_data.s_gled_value = ((s_color&0x01))*p_this->control_data.s_light_freq*p_this->running_cnt/100;
        p_this->control_data.s_bled_value = ((s_color&0x04)>>2)*p_this->control_data.s_light_freq*p_this->running_cnt/100;

        s_period_cnt = 0;
    }
}

void Ambient_Light_Changliang(_AMBIENT_LIGHT_OB *p_this)
{
    p_this->control_data.s_bled_value = ((g_myself_data.Scooter_Info.AmbientLightColor&0x04)>>2) * p_this->control_data.s_light_freq * g_myself_data.Scooter_Info.AmbientLightLux/100;
    p_this->control_data.s_rled_value = ((g_myself_data.Scooter_Info.AmbientLightColor&0x02 )>>1)* p_this->control_data.s_light_freq * g_myself_data.Scooter_Info.AmbientLightLux/100;
    p_this->control_data.s_gled_value = (g_myself_data.Scooter_Info.AmbientLightColor&0x01)* p_this->control_data.s_light_freq * g_myself_data.Scooter_Info.AmbientLightLux/100;
}

void Ambient_ClearColor(_AMBIENT_LIGHT_OB *p_this)
{
    p_this->control_data.s_rled_value = 0;
    p_this->control_data.s_gled_value = 0;
    p_this->control_data.s_bled_value = 0;
}

void Ambient_Timer_Cmp_Update(_AMBIENT_LIGHT_OB *p_this)
{
    TIM_SetCompare1(Ambient_TIMER,p_this->control_data.s_rled_value);
    TIM_SetCompare2(Ambient_TIMER,p_this->control_data.s_gled_value);
    TIM_SetCompare3(Ambient_TIMER,p_this->control_data.s_bled_value);

}

void Ambient_Light_Set(void)
{
    // 程序现在30mS运行一次，时间可能需要调整
    switch(Ambient_light_object.running_mode)
    {
    case STOP_LED://停止
        Ambient_ClearColor(&Ambient_light_object);
        break;

    case CHANGLIANG_LED://常亮
        Ambient_Light_Changliang(&Ambient_light_object);
        break;

    case BREATHE_LED://呼吸
        Ambient_Light_Breathe_Pro(&Ambient_light_object);
        break;
    case ALL_COLOR_LED://全彩

        Ambient_Light_ALL_Pro(&Ambient_light_object);
        break;
    default:
        break;
    }
    Ambient_Timer_Cmp_Update(&Ambient_light_object);

}

/**@brief    ambient light process init
*@details
*@param[in]  *p_this: ambient light object
*/
void Ambient_Light_Malloc_Init(_AMBIENT_LIGHT_OB *p_this )
{
    memset(&p_this->control_data,0,sizeof(p_this->control_data));
    p_this->control_data.s_rled_value = 0;
    p_this->control_data.s_gled_value = 0;
    p_this->control_data.s_bled_value = 0;

    p_this->running_polar = 0;
    p_this->running_cnt = 5;
    p_this->control_data.s_light_freq = Ambient_TIMER_PERIOD;//default freq
    p_this->running_mode = STOP_LED;//STOP_LED, CHANGLIANG_LED,BREATHE_LED
    p_this->running_color = g_myself_data.Scooter_Info.AmbientLightColor = 7;  // Amblient Clolor 1.蓝 2. 绿 3.青 4.红 5.? 6.? 7. ?
    p_this->running_lux = g_myself_data.Scooter_Info.AmbientLightLux = 100;
    p_this->running_period = 5;

    p_this->ambient_light_device_init = Ambient_Timer_Init;
    //p_this->ambient_light_switch_init = Ambient_Light_Switch_Init;
}


/**@brief    ambient light process start
*@details
*@param[in]  *p_this: ambient light object
*/
void Ambient_light_Start(_AMBIENT_LIGHT_OB *p_this )
{
    p_this->ambient_light_device_init();
    //p_this->ambient_light_switch_init();
}



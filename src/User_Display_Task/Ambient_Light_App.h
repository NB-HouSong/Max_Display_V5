#ifndef _AMBIENT_LIGHT_APP_H_
#define _AMBIENT_LIGHT_APP_H_

#include "main.h"

#define _1KHZ_PWM    31
#define _200HZ_PWM   5*31
#define _50HZ_PWM    20*31

#define  ON   1
#define  OFF  0

//#define AMBIENT_SWITCH_PIN    	GPIO_Pin_7
//#define AMBIENT_SWITCH_GPIO   	GPIOA

//#define AMBIENT_LIGHT_SWITCH_ON()      (AMBIENT_SWITCH_GPIO->BSRR = AMBIENT_SWITCH_PIN)
//#define AMBIENT_LIGHT_SWITCH_OFF()     (AMBIENT_SWITCH_GPIO->BRR = AMBIENT_SWITCH_PIN)
#define AMBIENT_ADJUST_STEP 2  

typedef enum{
    LOWEST_POLAR,
    INCREASE_POLAR,
    HIGHTST_POLAR,
    DECREASE_POLAR,
}_AMBIENT_LIGHT_POLAR;

typedef enum
{
    STOP_LED = 0,
    CHANGLIANG_LED,
    BREATHE_LED,
    ALL_COLOR_LED,
}_AMBIENT_LIGHT_MODE;

typedef struct {
    u32 s_rled_value;
    u32 s_gled_value;
    u32 s_bled_value;
    u32 s_light_freq;
    u8 s_switch_status;
    u8 s_reverse_red_flag;
    u8 s_reverse_green_flag;
    u8 s_reverse_blue_flag;
}_AMBIENT_LIGHT_DATASTR;

typedef struct
{
    _AMBIENT_LIGHT_DATASTR control_data;
    u8 running_mode;//0-OFF 1-ON  2-BRATHE
    u16 running_period;//ÖÜÆÚ
    u8 running_lux;
    u8 running_cnt;
    u8 running_polar;
    u8 running_color;
	u8 running_off_intens;
    void (*ambient_light_device_init)(void);
	void (*ambient_light_switch_init)(void);
	
}_AMBIENT_LIGHT_OB;

extern _AMBIENT_LIGHT_OB  Ambient_light_object;
void Ambient_Light_Malloc_Init(_AMBIENT_LIGHT_OB *p_this );
void Ambient_light_Start(_AMBIENT_LIGHT_OB *p_this );
void Ambient_Light_Set(void);
void Ambient_Light_Breathe_Pro(_AMBIENT_LIGHT_OB *p_this);

#endif


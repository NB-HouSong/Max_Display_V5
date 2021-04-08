#ifndef  __POWERKEY_H__
#define  __POWERKEY_H__

#include "main.h"

//转把和刹把采样5V对应235.5;1V对应47.1
#define ZB_AD_MAX			175		//转把霍尔AD采样的最大值
#define ZB_AD_MIN		    55		//转把霍尔AD采样的最小值
#define ZB_AD_MIN_F			30		//当小于该值时认为转把霍尔错误
#define ZB_AD_MAX_F			220		//当大于该值时认为转把霍尔错误
#define ZB_AD_MIN_F_ST	    10		//当小于该值时认为转把霍尔错误初始化过程时

#define EBS_AD_MAX			175		//EBS霍尔AD采样的最大值  理论值40~210
#define EBS_AD_MIN			55		//EBS霍尔AD采样的最小值
#define EBS_AD_MIN_F		30		//当小于该值时认为EBS霍尔错误
#define EBS_AD_MAX_F		220		//当大于该值时认为EBS霍尔错误
#define EBS_AD_MIN_F_ST	    10		//当小于该值时认为EBS霍尔错误初始化过程时

typedef struct
{
    u8 running_state;                
    u8 runing_time_cnt;             
    u8 runing_period;             
    void (*running_taskhook)(void);    
}TASK_COMPONENTS;


void ACC_DET(void);
void Led_Init(void);
//void AutoFLED(void);
//void FLED_Handle(void);

void DRIVER_CTRL(void);

//void PwrLedInit(void);
void PwrCtrlInit(void);
void CheckPwrKey(void);
void PowerDown(void);
void PrePowerDown(void);
void PowCtrl_Reset(void);
//void PowCtrl_PowerOff(void);
void ACC_OUT_CTRL(void);//ACC控制

//u8 Level_Matching(u8 selected_level);
#endif



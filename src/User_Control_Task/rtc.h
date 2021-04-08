#ifndef __RTC_H
#define	__RTC_H
#include "stm32f0xx.h"

/* 如果定义了下面这个宏的话,PC13就会输出频率为RTC Clock/64的时钟 */   
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

void RTC_Config(void);
void RTC_TimeRegulate(void);
void RTC_TimeShow(void);
void RTC_AlarmShow(void);
//uint8_t USART_Scanf(uint32_t value);
void RTC_INIT(void);
void set_Alarm(void);

extern RTC_TimeTypeDef RTC_TimeStructure;

#endif /* __XXX_H */

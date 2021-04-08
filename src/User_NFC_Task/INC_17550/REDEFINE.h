#ifndef _REDEFINE_H
#define _REDEFINE_H
#include "stm32f0xx.h"

#define PORT_NRST					        GPIOC
#define PIN_NRST					        GPIO_Pin_12		// NPDÒý½Å


//#define H_light_OFF   GPIOA->BSRR = GPIO_Pin_1//high
//#define H_light_ON	  GPIOA->BRR = GPIO_Pin_1//low

//#define P_light_ON	    P_READY_GPIO->BRR = P_READY_PIN
//#define P_light_OFF		  P_READY_GPIO->BSRR = P_READY_PIN

//#define ACCOUT_PIN								GPIO_Pin_4
//#define ACCCHG_PIN								GPIO_Pin_1
//#define ACCOUT_GPIO								GPIOC
//#define ACCCHG_GPIO								GPIOF

//#define FANGDAO_PIN								GPIO_Pin_13
//#define FANGDAO_GPIO							GPIOC

//#define POWER_WAKEUP_PIN							GPIO_Pin_2
//#define POWER_WAKEUP_GPIO							GPIOB

//#define P_READY_PIN								GPIO_Pin_14
//#define P_READY_GPIO							GPIOC

//#define HALL_PIN								  GPIO_Pin_0
//#define HALL_GPIO							    GPIOF
//#define HALLIN() 		              ((HALL_GPIO->IDR & HALL_PIN)?1:0)

//#define BREAK_PIN								  GPIO_Pin_11
//#define BREAK_GPIO							  GPIOB
//#define BREAKIN() 		            ((GPIOB->IDR & GPIO_Pin_11)?1:0)

//#define CHARGE_PIN								GPIO_Pin_2
//#define CHARGE_GPIO						    GPIOC

//#define LIMIT_SPEED_PIN						GPIO_Pin_3
//#define LIMIT_SPEED_GPIO					GPIOC

//#define LIGHT_PIN								  GPIO_Pin_1
//#define LIGHT_GPIO							  GPIOA

//#define SEAT_CTL_GPIO					    GPIOA
//#define SEAT_CTL_PIN					    GPIO_Pin_15			// 

//#define POWER_PIN								  GPIO_Pin_11
//#define POWER_GPIO							  GPIOA
//#define POWERKEYIN() 		          ((GPIOA->IDR & GPIO_Pin_11)?1:0)

//#define FANGDAO_OFF	(FANGDAO_GPIO->BRR |= FANGDAO_PIN)
//#define FANGDAO_ON	(FANGDAO_GPIO->BSRR |= FANGDAO_PIN)

//#define ACCIN() 		  ((ABS(g_PowerVoltage - g_ACCVoltage) < 20000)?1:0)  //????? ????
//#define _12V_IN() 		((ABS(g_12VVoltage - 12000) <2000)?1:0)  //??12v??????

//#define ACC_OFF			(ACCOUT_GPIO->BRR |= ACCOUT_PIN)
//#define ACC_ON			(ACCOUT_GPIO->BSRR |= ACCOUT_PIN)
//#define ACCOUT() 		((ACCOUT_GPIO->IDR & ACCOUT_PIN)?1:0)

//#define ACCCHG_OFF	(ACCCHG_GPIO->BRR = ACCCHG_PIN)
//#define ACCCHG_ON		(ACCCHG_GPIO->BSRR = ACCCHG_PIN)
//#define ACCCHG() 		((ACCCHG_GPIO->IDR & ACCCHG_PIN)?1:0)

//#define P_READY_L	(P_READY_GPIO->BRR  |= P_READY_PIN)
//#define P_READY_H	(P_READY_GPIO->BSRR |= P_READY_PIN)

//#define NO_CHARGING	(CHARGE_GPIO->BRR  |= CHARGE_PIN)//L
//#define IN_CHARGING	(CHARGE_GPIO->BSRR |= CHARGE_PIN)//H

//#define SEAT_L	(SEAT_CTL_GPIO->BRR  |= SEAT_CTL_PIN)//L
//#define SEAT_H	(SEAT_CTL_GPIO->BSRR |= SEAT_CTL_PIN)//H

//#define HIGH_SPEED  (LIMIT_SPEED_GPIO->BRR  |= LIMIT_SPEED_PIN)//L
//#define LIMIT_SPEED	(LIMIT_SPEED_GPIO->BSRR |= LIMIT_SPEED_PIN)//H
////////////////motor control///////////////
//#define MOTOR_PORT_SLEEP					      GPIOA
//#define MOTOR_PIN_SLEEP					        GPIO_Pin_2			// SLEEPÒý½Å
//#define MOTOR_PIN_SLEEP_HIGH	                MOTOR_PORT_SLEEP->BSRR = MOTOR_PIN_SLEEP
//#define MOTOR_PIN__SLEEPLOW                   MOTOR_PORT_SLEEP->BRR = MOTOR_PIN_SLEEP

//#define MOTORA1_PORT					          GPIOB
//#define MOTORA1_PIN					         GPIO_Pin_5			// A+
//#define MOTORA1_PIN_HIGH	                MOTORA1_PORT->BSRR = MOTORA1_PIN
//#define MOTORA1_PIN_LOW                  MOTORA1_PORT->BRR = MOTORA1_PIN

//#define MOTORA2_PORT					      GPIOB
//#define MOTORA2_PIN					      GPIO_Pin_3			// A-
//#define MOTORA2_PIN_HIGH	                MOTORA2_PORT->BSRR = MOTORA2_PIN
//#define MOTORA2_PIN_LOW                   MOTORA2_PORT->BRR = MOTORA2_PIN

//#define MOTORB1_PORT					      GPIOB
//#define MOTORB1_PIN					        GPIO_Pin_8			// B+
//#define MOTORB1_PIN_HIGH	                MOTORB1_PORT->BSRR = MOTORB1_PIN
//#define MOTORB1_PIN_LOW                  MOTORB1_PORT->BRR = MOTORB1_PIN

//#define MOTORB2_PORT					      GPIOB
//#define MOTORB2_PIN					        GPIO_Pin_9			// B-
//#define MOTORB2_PIN_HIGH	                MOTORB2_PORT->BSRR = MOTORB2_PIN
//#define MOTORB2_PIN_LOW                  MOTORB2_PORT->BRR = MOTORB2_PIN

//#define VDD5V_ON	GPIOB->BRR = GPIO_Pin_2
//#define VDD5V_OFF	GPIOB->BSRR = GPIO_Pin_2

//#define AMP_EN								    GPIO_Pin_1
//#define AMP_GPIO							    GPIOB


#endif

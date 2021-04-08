/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : main.h
* Author        : 李美华
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/#ifndef _AUDIO_DRIVER_H_
#define _AUDIO_DRIVER_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define GF_EN_GPIO  GPIOC
#define GF_EN_PIN   GPIO_Pin_6       //功放使能脚

#define GF_ENABLE()   GPIO_ResetBits(GF_EN_GPIO,GF_EN_PIN)
#define GF_DISABLE()  GPIO_SetBits(GF_EN_GPIO,GF_EN_PIN)

#define DAC_OUT_GPIO  GPIOA
#define DAC_OUT_PIN   GPIO_Pin_4   //功放使能脚

#define  ON   1
#define  OFF  0

#define  PLAY_A    0
#define  PLAY_B    1

#define AUDIO_BUFFER_LEN                256                 //1k

/* Exported functions ------------------------------------------------------- */
void Audio_driver_configrate(uint16_t pwm_period, uint8_t config_mode);
void Audio_DMA_Refresh(uint32_t *DMAbuff,uint32_t Bufflen);
void PWM_init(void);
#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

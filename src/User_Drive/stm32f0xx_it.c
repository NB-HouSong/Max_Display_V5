#include "stm32f0xx_it.h"
#include "main.h"
/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/
void SystemCoreClockConfigure(void);
void SystemCoreClockUpdate (void);
void Peripherals_Clock_Init(void);
void NVIC_Configuration(void);
//static void SYSCLKConfig_STOP(void);
/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

#ifndef FreeRTOS
/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    
}

#endif


void TIM17_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM17,TIM_IT_Update) != RESET)
        /* Clear TIM1 Capture compare interrupt pending bit */
    {
        TIM_ClearITPendingBit(TIM17,TIM_IT_Update);
        //g_NBCmdMap[NB_INF_SPEED]++;  //数脉冲计数
        //g_MainTick = 1;//
    }
}


void TIM14_IRQHandler(void)		//5ms
{
    if(TIM_GetITStatus(TIM14,TIM_IT_Update) != RESET)
    {        
        //Normal_Tasks_Remarks();       
        TIM_ClearITPendingBit(TIM14,TIM_IT_Update);
    }
}

void TIM16_IRQHandler(void)
{
    
    
}



/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */
#if 0
static void SYSCLKConfig_STOP(void)
{
    /* After wake-up from STOP reconfigure the system clock */
    /* Enable HSE */
// RCC_HSEConfig(RCC_HSE_ON);
    RCC_HSICmd(ENABLE);
//  RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource);
    /* Wait till HSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
    {}

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {}

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08)
    {}
}

#endif
void RTC_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line17);
    if(RTC_GetITStatus(RTC_IT_ALRA) ==RESET)
    {
//		LED1_TOGGLE;
        RTC_ClearITPendingBit(RTC_IT_ALRA);
    }
}

void EXTI4_15_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line12) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line12);
    }


    if(EXTI_GetITStatus(EXTI_Line9) != RESET)
    {

        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}
void EXTI0_1_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line1);
    }

}
void EXTI2_3_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
    {

    }
    EXTI_ClearITPendingBit(EXTI_Line2);
}

void ADC1_IRQHandler(void)
{
    if(ADC_GetITStatus(ADC1, ADC_IT_AWD) != RESET)
    {
        /* Clear ADC1 AWD pending interrupt bit */
        ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

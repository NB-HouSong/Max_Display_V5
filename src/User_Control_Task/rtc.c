//#include "rtc.h"
//#include "Global.h"
//#include <stdio.h>
//#include "UART.h"
//#include "LED.h"

//#include "MC20cmd.h"
///* Private typedef -----------------------------------------------------------*/
///* Private define ------------------------------------------------------------*/
///* Uncomment the corresponding line to select the RTC Clock source */
////#define RTC_CLOCK_SOURCE_LSE   /* 选择LSE  外部低速时钟*/
//#define RTC_CLOCK_SOURCE_LSI  // LSI 也可以
// 
////#define INDIA
//#define CHINA

//#define BKP_VALUE    0x32F0

//RTC_TimeTypeDef RTC_TimeStructure;
//RTC_InitTypeDef RTC_InitStructure;
//RTC_AlarmTypeDef  RTC_AlarmStructure;

//__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;//同步分频值和非同步分频值



//void RTC_INIT(void)
//{  
//  EXTI_InitTypeDef  EXTI_InitStructure;
//  if (RTC_ReadBackupRegister(RTC_BKP_DR0) != BKP_VALUE)
//  {  
//    /* RTC 配置  */
//    RTC_Config();

//    /*分频设置 */
//    RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
//    RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;
//    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
//   
//    /* 检查是否已经初始化 */
//    if (RTC_Init(&RTC_InitStructure) == ERROR)
//    {
////      printf("\n\r        /!\\***** RTC Prescaler Config failed ********/!\\ \n\r");
//			;
//    }

//    /*设置寄存器 */
////    RTC_TimeRegulate(); 
//  }
//  else
//  {
//    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
//    {
////      printf("\r\n Power On Reset occurred....\n\r");
//			;
//    }
// 
//    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
//    {
////      printf("\r\n External Reset occurred....\n\r");
//			;
//    }

////    printf("\n\r No need to configure RTC....\n\r");
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
//    PWR_BackupAccessCmd(ENABLE);

// #ifdef RTC_CLOCK_SOURCE_LSI
//    /* Enable the LSI OSC */ 
//    RCC_LSICmd(ENABLE);
//#endif /* RTC_CLOCK_SOURCE_LSI */

//    /* Wait for RTC APB registers synchronisation */
//    RTC_WaitForSynchro();

//    /* Clear the RTC Alarm Flag */
//    RTC_ClearFlag(RTC_FLAG_ALRAF);


//    /* Display the RTC Time and Alarm */
//    RTC_TimeShow();
//    RTC_AlarmShow();
//  }
//	
//  /* RTC Alarm A Interrupt Configuration */
//  /* EXTI configuration *********************************************************/
//  EXTI_ClearITPendingBit(EXTI_Line17);
//  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
//}

///**
//  * @brief  Configure the RTC peripheral by selecting the clock source.
//  * @param  None
//  * @retval None
//  */
//void RTC_Config(void)
//{
//   /* 使能 PWR 时钟 */
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

//  /* 允许访问RTC */
//  PWR_BackupAccessCmd(ENABLE);
//    
//#if defined (RTC_CLOCK_SOURCE_LSI)  /* 当使用LSI 作为 RTC 时钟源*/
///* The RTC Clock may varies due to LSI frequency dispersion. */
//  /* 使能 LSI 振荡 */ 
//  RCC_LSICmd(ENABLE);

//  /* 等待到 LSI 预备*/
//  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
//  {
//  }

//  /* 把RTC 时钟源配置为LSI */
//  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
//   /* 定襎同步分频值和异步分频值 */
//  SynchPrediv = 0x18F;
//  AsynchPrediv = 0x63;

//#elif defined (RTC_CLOCK_SOURCE_LSE) /* 当使用LSE 作为 RTC 时钟源 */
//  /*使能 LSE 振荡 */
//  RCC_LSEConfig(RCC_LSE_ON);

//  /*等待 LSE 预备 */  
//  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
//  {
//  }

//  /* 把RTC 时钟源配置为使用LSE */
//  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
//     /* 定襎同步分频值和异步分频值 */
//  SynchPrediv = 0xFF;
//  AsynchPrediv = 0x7F;

//#else
//  #error Please select the RTC Clock source inside the main.c file
//#endif /* RTC_CLOCK_SOURCE_LSI */
//  
//  /* 使能RTC时钟 */
//  RCC_RTCCLKCmd(ENABLE);

//  /* 等待 RTC APB 寄存器同步 */
//  RTC_WaitForSynchro();
//}

///**
//  * @brief  Returns the time entered by user, using Hyperterminal.
//  * @param  None
//  * @retval None
//  */
//void RTC_TimeRegulate(void)
//{
//	u8 temp8_h;
//	u8 temp8_m;
//  if(g_bool[B_MC20_COMMU] == 0 || (g_MC20CmdMap[MC20_INF_ECU_BOOL] & MC20_BOOLMARK_TIME) == 0 || g_MC20CmdMap[MC20_INF_M_Y] == 0 || g_bool[B_TIME] == 0)
//    return;
//#ifdef CHINA
//	temp8_h = (u8)((g_MC20CmdMap[MC20_INF_H_D] & 0xFF00) >> 8);
//	temp8_h += 8;
//	temp8_m = (u8)(g_MC20CmdMap[MC20_INF_S_M] & 0x00FF);
//#endif
//#ifdef INDIA
//	temp8_h = (u8)((g_MC20CmdMap[MC20_INF_H_D] & 0xFF00) >> 8);
//	temp8_h += 5;
//	
//	temp8_m = (u8)(g_MC20CmdMap[MC20_INF_S_M] & 0x00FF) + 30;
//	if(temp8_m >= 60)
//	{
//		temp8_m -= 60;
//		temp8_h ++;
//	}	
//#endif	
//	g_MC20CmdMap[MC20_INF_ECU_BOOL] &= ~MC20_BOOLMARK_TIME;
//	if(temp8_h >= 24)
//		temp8_h -= 24;
//  RTC_TimeStructure.RTC_Hours = temp8_h;
//  RTC_TimeStructure.RTC_Minutes = temp8_m;
//  RTC_TimeStructure.RTC_Seconds = (u8)((g_MC20CmdMap[MC20_INF_S_M] & 0xFF00) >> 8);

//  /* Configure the RTC time register */
//  if(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) == ERROR)
//  {
////    printf("\n\r>> !! RTC Set Time failed. !! <<\n\r");
////		LED1_TOGGLE;
//  }
//  else
//  {
////    printf("\n\r>> !! RTC Set Time success. !! <<\n\r");
//    RTC_TimeShow();
//    /* Indicator for the RTC configuration */
//    RTC_WriteBackupRegister(RTC_BKP_DR0, BKP_VALUE);
//		g_bool[B_TIME] &= ~TIME_SET;
//  }
//	if(g_bool[B_TIME] & TIME_ALARM)
//	{
//		g_bool[B_TIME] &= ~TIME_ALARM;
//		set_Alarm();
//	}
//}

//void set_Alarm(void)
//{
//	u8 tmp_hh = 0x00, tmp_mm = 0x00, tmp_ss = 0x00;

////	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
////	tmp_hh = RTC_TimeStructure.RTC_Hours;
////	tmp_mm = RTC_TimeStructure.RTC_Minutes;
////	tmp_ss = RTC_TimeStructure.RTC_Seconds;
////	tmp_hh += 24;
////	if(tmp_hh >= 24)
////	{
////		tmp_hh -= 24;
////	}


//  /* Disable the Alarm A */
//  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

////  printf("\n\r==============Alarm A Settings=====================================\n\r");
//  RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;			//24小时制
////  printf("  Please Set Alarm Hours:\n\r");
//  RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = tmp_hh;
//	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = tmp_mm;
//	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = tmp_ss;
// 
//  /* Set the Alarm A */
//  RTC_AlarmStructure.RTC_AlarmDateWeekDay = RTC_Weekday_Monday;		//日期					
//  RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;		//按日期闹   RTC_AlarmDateWeekDaySel_WeekDay //按星期闹，上一个参数1-7
//  RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;     //屏蔽日期和星期，= RTC_AlarmMask_None打开

//  /* Configure the RTC Alarm A register */
//  RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
////  printf("\n\r>> !! RTC Set Alarm success. !! <<\n\r");
//  RTC_AlarmShow();
//	
//  /* Enable the RTC Alarm A Interrupt */
//  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
//   
//  /* Enable the alarm  A */
//  RTC_AlarmCmd(RTC_Alarm_A, ENABLE);	
//}

///**
//  * @brief  Display the current time on the Hyperterminal.
//  * @param  None
//  * @retval None
//  */
//void RTC_TimeShow(void)
//{
//  /* Get the current Time */
//  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
////	g_NBCmdMap[NB_INF_RTC_HH] = RTC_TimeStructure.RTC_Hours;
////	g_NBCmdMap[NB_INF_RTC_MM] = RTC_TimeStructure.RTC_Minutes;
////	g_NBCmdMap[NB_INF_RTC_SS] = RTC_TimeStructure.RTC_Seconds;
////  printf("\n\r  The current time is :  %0.2d:%0.2d:%0.2d \n\r", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
//}

///**
//  * @brief  Display the current time on the Hyperterminal.
//  * @param  None
//  * @retval None
//  */
//void RTC_AlarmShow(void)
//{
//  /* Get the current Alarm */
//  RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
////  printf("\n\r  The current alarm is :  %0.2d:%0.2d:%0.2d \n\r", RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours, RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes, RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds);
//}



//#ifdef  USE_FULL_ASSERT

///**
//  * @brief  Reports the name of the source file and the source line number
//  *         where the assert_param error has occurred.
//  * @param  file: pointer to the source file name
//  * @param  line: assert_param error line source number
//  * @retval None
//  */
//void assert_failed(uint8_t* file, uint32_t line)
//{ 
//  /* User can add his own implementation to report the file name and line number,
//     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

//  /* Infinite loop */
//  while (1)
//  {
//  }
//}
//#endif

///**
//  * @}
//  */

///**
//  * @}
//  */

///************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

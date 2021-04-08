#include "main.h"
#include "startup.h"
#include "UART.h"
#include "UART_2.h"
#include "UART_3.h"
#include "UART_4.h"
#include "Control.h"
#include "Flash.h"
#include "IAP.h"
#include "LED_Others.h"
#include "StateUpdate.h"
#include "rtc.h"
#include "ParseFrame.h"
#include "decode.h"
#include "SPI.h"

void NVIC_Configuration(void);
void Peripherals_Clock_Init(void);
u8 RCC_Configuration(void);

volatile s32 m_PowerVoltage1 = 0;

void InitErrorPro(void)
{
	static u16 s_cnt = 0;
	static u16 s_BLECnt = 0;
	
	s_cnt++;
	UART_CheckReceive();
	UART_CheckSend();
	if(!g_bool[B_IN_IAPMODE])		//非IAP模式
	{
		s_BLECnt++;
		if(s_BLECnt >= 200/5)
		{
			s_BLECnt = 0;
//			g_bool[B_BMS1_OFFLINE] = 1;
//			g_BatteryLeval = PowCtrl_GetBatLevel();		//电量格数
//			g_MiniUpData[MINI_UPDATA_POWER] = g_BatteryLeval;
//			UART_PushFrame(V6_MINI_DIS_ID, 2, CMD_WR, 0, (u8*)g_MiniUpData);		//给BLE发数据
		}
//		if( (!LED_Beep_InitErrorPro() && s_cnt >= 200*30))		//报警循环结束，并且持续报警了20s，或者按下关机键
//		{
//			delay_ms(200);
//			PowerDown();
//			delay_ms(500);
//			NVIC_SystemReset();				//如果1s后仍没有关机，复位
//		}
	}
	else
	{
		s_cnt = 0;
	}
//	BLED_FLED_Pro_200Hz();
	Check_IAP_Mode();
}


//void InitErrorLoop(void)
//{
//	while(1)
//	{
//		g_NBCmdMap[NB_INF_ERROR] = g_ErrorByte;		//错误字
//		g_NBCmdMap[NB_QUK_ERROR_ALARM] &= ~0x00FF;
//		g_NBCmdMap[NB_QUK_ERROR_ALARM] |= g_ErrorByte & 0x00FF;
//		InitErrorPro();
//		delay_ms_CheckUART(5);
//		if(g_bool[B_ALLOW_ACT_DELAY] > 0)  //激活延时限制
//				g_bool[B_ALLOW_ACT_DELAY]--;
//		if(g_bool[B_SAVE_FDATA]) 	
//		{
//			SaveFDatamap();
//			g_bool[B_SAVE_FDATA] = 0;	
//		}
//	}
//}


void CheckPCBVersion(void)
{

}


void SysLockProtectedProc(u8 nType)
{
	//设置读保护:
	if (nType == 0x01)
	{
		if (FLASH_OB_GetRDP() != SET)
		{
			FLASH_Unlock();

			FLASH_OB_Unlock();

			FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

			FLASH_OB_RDPConfig(OB_RDP_Level_1);
			FLASH_OB_Lock();
		}
	}
	//解除读保护
	else
	{
		if (FLASH_OB_GetRDP()  == SET)
		{
			FLASH_Unlock();

			FLASH_OB_Unlock();

			FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

			FLASH_OB_RDPConfig(OB_RDP_Level_0);
			FLASH_OB_Lock();
		}
	}
}

//外设Clock初始化
void Peripherals_Clock_Init(void)
{
	//打开时钟需要注意，如果两个外设有一个公用的引脚，则同时打开两个外设的时钟有可能会使该引脚输出不可控
	//例如，使能了USART3会使PB10自动拉低（如果在其他初始化时设为推挽复用输出的话）
	//使能端口时钟
  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	//进行端口重映射
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//JTAG调试端口不可用
  /* DAC Periph clock enable */
	
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART4,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	  
  /* Enable the SPI periph */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  
//	SysLockProtectedProc(1);
// */
}   

//中断初始化
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
  NVIC_InitStructure.NVIC_IRQChannel = PVD_VDDIO2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
//	//ADC中断
//  NVIC_InitStructure.NVIC_IRQChannel = ADC1_COMP_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel =  TIM3_IRQn;;  
//  NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel =  TIM14_IRQn;;  
  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel = USART3_4_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPriority = 4;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPriority = 4;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
  /* Enable RTC_IRQn */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

u16 crc;

//初始化所有外设
void InitAllPeripherals( void )
{
	u8 temp_buf[12] = {0};
	IAP_Init();
	NVIC_Configuration();	//初始化中断源（须放在前面）
	Peripherals_Clock_Init();		//使能端口时钟
//	CheckPCBVersion();		//判断PCB版本号
//	Led_Init();						//初始化LED
//	CRC_Config(0x4C11DB7);		//标准 CRC32
	CRC_Config(0x4E494E45);
	memcpy((u8*)temp_buf, (u8*)(&g_NBCmdMap[NB_INF_SN]) + 2, 12);
	g_ComputedCRC = 0xFFFFFFFF ^ CRC_CalcBlockCRC((uint32_t *)temp_buf, 3);		//输入16bit
	
	CRC_Config(0x04C11DB7);
	memcpy((u8*)temp_buf, (u8*)(&g_NBCmdMap[NB_INF_IAP_KEY]), 16);
	g_RAND_ENCRYPT_CRC = 0xFFFFFFFF ^ CRC_CalcBlockCRC((uint32_t *)temp_buf, 4);		//输入16bit
	
	crc = (u16)g_ComputedCRC;
	crc += (u16)(g_ComputedCRC >> 16);
	
//	ev1527_Init();
	UART_Int();						//USART1
	UART2_Int();					//USART2
	UART3_Int();					//USART3
//	UART4_Int();					//USART4
//	ParseInit();
	ADC_Int();						//其他ADC初始化
//	PwrCtrlInit();				//电源开关初始化
	LED_Time_Init();

	TIM3_Init();          //大灯
	TIM14_Init();         //
	BLED_Init();          //尾灯
//	RTC_INIT();
//	I2C_GPIO_Config();
//	Imu_Int();						//三轴陀螺仪初始化，包括spi初始化
//  DecodeInit();
	
//	RunRec_Init();	//同步行驶数据记录位置
//	ErrRec_Init();				//错误记录器初始化
//	ErrRec_Sync_Time(*((u32*)(&g_NBCmdMap[NB_INF_RUN_TIM_L])));
//	if(g_NBCmdMap[NB_INF_FW_VERSION] != FW_VERSION)	//固件版本号改变记录一条黑匣子数据（这里必须等到其他东西全部初始化且中断开启前进行）
//	{
//		g_NBCmdMap[NB_INF_FW_VERSION] = FW_VERSION;
////		if(g_NBCmdMap[NB_INF_FW_VERSION] < 0x0100)		//内测版固件，清除黑匣子数据
////			ErrRec_ClearData();
//		ErrRec_Push_Error(MINI_BB_ERR_FW_VERSION, (FW_VERSION>>8)*100 + ((FW_VERSION>>4)&0x0F)*10 + ((FW_VERSION)&0x0F));
//	}
//	
//	RunRec_Init();				//行驶数据记录器初始化
	
#ifndef SMT_TEST_MODE
  /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
     dispersion) */
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: LSI/32 */
  IWDG_SetPrescaler(IWDG_Prescaler_32);

  /* Set counter reload value to obtain 250ms IWDG TimeOut.
     Counter Reload Value = 500ms/IWDG counter clock period
                          = 500ms / (LSI/32)
                          = 0.5s / (LsiFreq/32)
                          = LsiFreq/(32 * 2)
                          = LsiFreq/64
   */
  IWDG_SetReload(40000/16);		//LsiFreq = 40k   //2s
  /* Reload IWDG counter */
  IWDG_ReloadCounter();

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
//  IWDG_Enable();
//	delay_ms_CheckUART(50);
#endif
}



/******************** (C) COPYRIGHT 2020 Ninebot *******************************
* File Name     : Universal_Init.c
* Author        : meihua
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "Universal_Init.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
u16 m_ADCValue[ADC_BUF_LEN] = {0};
s16 cal_gain[AdcChanelMaxNumber] = //ADC转换系数
{
	89,  //12V大灯、灯箱供电电压
	13,  //2 油门
	0,   //油门地线断开检测
//	89   //无线充供电电压检测   
};

/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/*****************************************************************
* Function Name : TIM3_Init
* Description   : 仪表氛围灯初始化
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void Ambient_Timer_Init(void)  //仪表氛围灯
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	//氛围灯引脚初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	TIM_DeInit(Ambient_TIMER);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = Ambient_TIMER_PERIOD;            //1;					    //PWM频率1M/1000=1KHz
    TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1;					    //48M
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(Ambient_TIMER, &TIM_TimeBaseStructure);

    TIM_ITConfig(Ambient_TIMER, TIM_IT_Update, ENABLE);

    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    
    TIM_OC1Init(Ambient_TIMER, &TIM_OCInitStructure);
    TIM_OC2Init(Ambient_TIMER, &TIM_OCInitStructure);
    TIM_OC3Init(Ambient_TIMER, &TIM_OCInitStructure);
    TIM_CtrlPWMOutputs(Ambient_TIMER, ENABLE);
		
    //使能预装载
    TIM_OC1PreloadConfig(Ambient_TIMER, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(Ambient_TIMER, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(Ambient_TIMER, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(Ambient_TIMER, ENABLE);

    TIM_Cmd(Ambient_TIMER, ENABLE);
}

/*****************************************************************
* Function Name : Handle_TIM17_Init
* Description   : 把套加热PWM初始化
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void Handle_Timer_Init(void)
{
	TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
	TIM_OCInitTypeDef           TIM_OCInitStructure;
	GPIO_InitTypeDef            GPIO_InitStructure;

	//把套加热引脚初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

    //GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_0);

	TIM_DeInit(HANDLE_TIMER);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = HANDLE_TIMER_PERIOD;             //1;					    //PWM频率1M/1000=1KHz
    TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1;					    //48M
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(HANDLE_TIMER, &TIM_TimeBaseStructure);

    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;


    TIM_OC1Init(HANDLE_TIMER, &TIM_OCInitStructure);

    //使能预装载
    TIM_OC1PreloadConfig(HANDLE_TIMER, TIM_OCPreload_Enable);
    //TIM_ARRPreloadConfig(HANDLE_TIMER, ENABLE);
    TIM_CtrlPWMOutputs(HANDLE_TIMER, ENABLE);
    
    TIM_Cmd(HANDLE_TIMER, ENABLE);
    
}

/*****************************************************************
* Function Name : Handle_Temp_OFF
* Description   : 关闭前大灯
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void Handle_Temp_OFF(void)
{
    TIM_SetCompare1(HANDLE_TIMER, 0);
}

/*****************************************************************
* Function Name : Handle_Temp_ON
* Description   : 前大灯全亮
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void Handle_Temp_ON(void)
{
    TIM_SetCompare1(HANDLE_TIMER, 99);
}

/*****************************************************************
* Function Name : Handle_Temp_Set
* Description   : 把套加热控制函数
* Input         : 加热功率：0~100 
* Output        : None
* Notes         :
******************************************************************/	
void Handle_Temp_Set(u8 pow)
{
    TIM_SetCompare1(HANDLE_TIMER, (pow * 99/100));
}

/*****************************************************************
* Function Name : ADCDMAConfiguration
* Description   : ADCDMA的配置
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void ADCDMAConfiguration(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(ADC_DMA_CH);
	DMA_InitStructure.DMA_PeripheralBaseAddr =(u32)( &(ADC_USE->DR));			//ADC1数据寄存器
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)m_ADCValue;				        //获取ADC的数组
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;						    //片内外设作源头
	DMA_InitStructure.DMA_BufferSize = ADC_BUF_LEN;								//每次DMA32个数据
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		    //外设地址不增加
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					    //内存地址增加
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//半字
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;							    //循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;					        //高优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							    //非内存到内存
	DMA_Init(ADC_DMA_CH, &DMA_InitStructure);
	
	DMA_Cmd(ADC_DMA_CH, ENABLE);
}

/*****************************************************************
* Function Name : ADC_Int
* Description   : ADC相关初始化
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void ADC_Int(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	//端口配置
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//50M时钟速度
	
    /**ADC1 GPIO Configuration
    PA0     ------> ADC1_IN0    ------> wirelss charger voltage
    //PA1     ------> ADC1_IN1    ------> wirelss charger FB
    PA2     ------> ADC1_IN2    ------> heated fault
    PA3     ------> ADC1_IN3    ------> acc current
    
    PA4     ------> ADC1_IN4    ------> acc in
    PA5     ------> ADC1_IN5    ------> right break current
    PA6     ------> ADC1_IN6    ------> right break
    PA7     ------> ADC1_IN7    ------> left break current
    
    PC0     ------> ADC1_IN10   ------> left temp
    PC1     ------> ADC1_IN11   ------> right temp 
    PC2     ------> ADC1_IN12   ------> right light Over Current
    PC3     ------> ADC1_IN13   ------> left light Over Current

    PC4     ------> ADC1_IN14   ------> left break
    */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;  //left temp
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | /*GPIO_Pin_1 |*/ GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);
	// ADCCLK = PCLK2/4 = 12M
	RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div4);	
	ADCDMAConfiguration();
	/* ADC DMA request in circular mode */
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
    /* Initialize ADC structure */
    ADC_StructInit(&ADC_InitStructure);
	// ADC
	ADC_InitStructure.ADC_Resolution  = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode  = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge  = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign  = ADC_DataAlign_Right;
//	ADC_InitStructure.ADC_ScanDirection  = ADC_ScanDirection_Backward;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;	//向上扫描
	ADC_Init(ADC_USE, &ADC_InitStructure);

	//配置采样通道
	ADC_ChannelConfig(ADC1, ADC_Channel_0, ADC_SampleTime_55_5Cycles);
	//ADC_ChannelConfig(ADC1, ADC_Channel_1, ADC_SampleTime_55_5Cycles);
    ADC_ChannelConfig(ADC1, ADC_Channel_2, ADC_SampleTime_55_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_3, ADC_SampleTime_55_5Cycles);

    ADC_ChannelConfig(ADC1, ADC_Channel_4, ADC_SampleTime_55_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_5, ADC_SampleTime_55_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_6, ADC_SampleTime_55_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_7, ADC_SampleTime_55_5Cycles);

    ADC_ChannelConfig(ADC1, ADC_Channel_10, ADC_SampleTime_55_5Cycles);
    ADC_ChannelConfig(ADC1, ADC_Channel_11, ADC_SampleTime_55_5Cycles);
    ADC_ChannelConfig(ADC1, ADC_Channel_12, ADC_SampleTime_55_5Cycles);
    ADC_ChannelConfig(ADC1, ADC_Channel_13, ADC_SampleTime_55_5Cycles);
    
    ADC_ChannelConfig(ADC1, ADC_Channel_14, ADC_SampleTime_55_5Cycles);

	//重置校准寄存器
	ADC_GetCalibrationFactor(ADC_USE);

	ADC_DMACmd(ADC_USE, ENABLE);	//DMA和adc要同时打开
	ADC_Cmd(ADC_USE, ENABLE);

	while(!ADC_GetFlagStatus(ADC_USE, ADC_FLAG_ADRDY));
	//开始校准状态
	ADC_StartOfConversion(ADC_USE);
}

/*****************************************************************
* Function Name : ADC_Get_ConversionValue
* Description   : 8次采样的平均值
* Input         : Channel：采样通道
* Output        : None
* Notes         :
******************************************************************/	
u16 ADC_Get_ConversionValue(u8 Channel)
{
	s32 temp32 = 0;
	u8 i = 0;
	
	for(i = 0; i < 8; i++)
	{
		temp32 += m_ADCValue[i * AdcChanelMaxNumber + Channel];
	}
	
	return temp32>>3;
}

/*****************************************************************
* Function Name : Get_RealVaule
* Description   : 获取油门刹车值
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
#define EBS_AD_TSD	 175					//EBS霍尔AD采样的最大值  理论值40~210
void Get_RealVaule(void)
{
	g_myself_data.Handle_Bar_Info.GasValue = ADC_Get_ConversionValue(AdcChanelAcc);//*cal_gain[ADC_CH_Oil]/100/2;  //电压放大50倍
    g_myself_data.Handle_Bar_Info.LeftBreakValue = ADC_Get_ConversionValue(AdcChanelLeftBreak);//*cal_gain[ADC_CH_Oil]/100/2;  //电压放大50倍
    g_myself_data.Handle_Bar_Info.RightBreakValue = ADC_Get_ConversionValue(AdcChanelRightBreak);//*cal_gain[ADC_CH_Oil]/100/2;  //电压放大50倍
    
//    if()
//    {
//        g_myself_data.Handle_Bar_Info.WirelessChargerStatus = ADC_Get_ConversionValue(AdcChanelAcc);//*cal_gain[ADC_CH_Oil]/100/2;  //电压放大50倍
//    }
//    
//    g_myself_data.Handle_Bar_Info.GasValue = ADC_Get_ConversionValue(AdcChanelAcc);//*cal_gain[ADC_CH_Oil]/100/2;  //电压放大50倍
//    g_myself_data.Handle_Bar_Info.GasValue = ADC_Get_ConversionValue(AdcChanelAcc);//*cal_gain[ADC_CH_Oil]/100/2;  //电压放大50倍
//	g_wireless_charger_vol = ADC_Get_ConversionValue(AdcChanelWirelessChargVoltage);//*cal_gain[ADC_CH_WAIRLESS_CHARGE]/1000;

//	g_12V_vol = ADC_Get_ConversionValue(ADC_CH_12V_COL);//*cal_gain[ADC_CH_12V_COL]/1000;

//	g_myself_data.Handle_Bar_Info.LeftBreakValue = ADC_Get_ConversionValue(ADC_CH_BREAK1);//*cal_gain[ADC_CH_BREAK1]/100/2; 
//	g_myself_data.Handle_Bar_Info.Break2Value = ADC_Get_ConversionValue(ADC_CH_BREAK2);//*cal_gain[ADC_CH_BREAK2]/100/2; 
////	if(ADC_Get_ConversionValue(ADC_CH_BREAK_GND) < 1000) //值需要标定
////	{
////		g_myself_data.Handle_Bar_Info.Brake1_GND = 1; //有至少1个刹车地线断开
////	}
////	else
////	{
////		g_myself_data.Handle_Bar_Info.Brake1_GND = 0;
////	}



//	if(ADC_Get_ConversionValue(ADC_CH_Oil_GND) < 200) 
//		g_myself_data.Handle_Bar_Info.Gas_GND = 1;
//	else
//		g_myself_data.Handle_Bar_Info.Gas_GND = 0;
}

/*****************************************************************
* Function Name : Timer2Init
* Description   : Timer2初始化
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void Timer2Init(void)      //Timer2,控制采样率
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	//使能TIM2时钟

	//定时器定时时间T计算公式：T=(TIM_Period+1)*(TIM_Prescaler+1)/TIMxCLK
	TIM_TimeBaseStructure.TIM_Period = 999;
    //自动重装载值10--定时时间(10*4800/48M)s  16kHz
	TIM_TimeBaseStructure.TIM_Prescaler =47;//预分频值，+1为分频系数 
	TIM_TimeBaseStructure.TIM_ClockDivision =0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
//  TIM_SetAutoreload(TIM2, 999);

	TIM_ClearFlag(TIM2, TIM_IT_Update);//清标志
//	  /* TIM2触发输出模式选择  */
//  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
//	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM2, ENABLE);  	//使能TIMx外设
}

/*****************************************************************
* Function Name : TIM2_IRQHandler
* Description   : Timer2的中断函数
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void TIM2_IRQHandler(void)
{
//	static u8 Test_ON = 0;
//	static u8 Test_NUM = 0;
//	static u16 m_time_cnt = 0;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		Check_Read_Voice_Data();	
		
//		if(Test_ON == 1) //音频测试
//		{
//			Test_ON = 0;
//	//		Test_NUM++;
//	//		if(Test_NUM >3)
//	//			Test_NUM =1;
//			PlayVoice(8);
//		}

//		if(++m_time_cnt >= 1000)
//		{
//			Test_ON = 1;
//			m_time_cnt = 0;
//		}
	}
}

//====================================================================================//

//==================================== The End===============================================/

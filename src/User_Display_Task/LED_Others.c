/******************** (C) COPYRIGHT 2020 Ninebot *******************************
* File Name     : LED_Others.c
* Author        : meihua
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LED_Others.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define	LED_PERIOD			59			//周期，1.25us
#define	LED_CCR_H   	 	40			//1对应的占空比，0.85us
#define	LED_CCR_L    	 	19			//0对应的占空比，0.4us
#define	LED_CCR_RESET  	    0			//Reset对应的占空比

#define LED_DMA_Channel		DMA1_Channel5
#define RGB_NUM             1
#define LED_NUM             RGB_NUM * 3			//12 * 4 = 48位  
#define	LEDBUFF_LEN         (LED_NUM * 8 + 20)	//每个out口对应8位的寄存器

/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
static u16 m_LED_DMABuff[LEDBUFF_LEN] = {0};
static u8 RGB_brightness = 0xFF*100/100;         //50%亮度
//DIS_COLOR_INFO DIS_Color_Set;
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/*****************************************************************
* Function Name : RGB_Buff_Init
* Description   : DMABuffer 初始化 
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void RGB_Buff_Init(void)
{
	m_LED_DMABuff[0] = LED_CCR_RESET;
	m_LED_DMABuff[1] = LED_CCR_RESET;
	
	for(u8 i = 2; i < LED_NUM * 8+2; i++)
	{
		m_LED_DMABuff[i] = LED_CCR_L;
	}
	for(u16 j = LED_NUM * 8+2; j < LEDBUFF_LEN; j++)
	{
		m_LED_DMABuff[j] = LED_CCR_RESET; //填充复位数据
	}
}

/*****************************************************************
* Function Name : RGB_Time_Init
* Description   : 氛围灯初始化
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void RGB_Time_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	
	RGB_Buff_Init();
	
	//进行端口重映射
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_2); 

	//GPIO设置
	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			
	GPIO_Init(LED_GPIO, &GPIO_InitStructure);

	//DMA配置
	DMA_DeInit(LED_DMA_Channel);
	DMA_StructInit(&DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(LED_TIM->CCR3));
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)(m_LED_DMABuff);
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = LEDBUFF_LEN;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(LED_DMA_Channel, &DMA_InitStructure);

	//定时器基本设置
	TIM_DeInit(LED_TIM);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0;					//48M
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = LED_PERIOD;				//1.25us周期
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(LED_TIM, &TIM_TimeBaseStructure);
	
    //比较通道设置
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	 		  //PWM1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //正向输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //
	TIM_OCInitStructure.TIM_Pulse = LED_CCR_RESET;

    TIM_OC3Init(LED_TIM, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(LED_TIM, TIM_OCPreload_Enable);		  //使能预装载
	
	//使能预装载
	TIM_ARRPreloadConfig(LED_TIM, ENABLE);
	//DMA使能
	TIM_DMACmd(LED_TIM, TIM_DMA_CC3, ENABLE);			          //这里表示每次溢出后进行一次DMA传输
	//TIM1计数使能
	TIM_Cmd(LED_TIM, ENABLE);
	//PWM输出使能
	TIM_CtrlPWMOutputs(LED_TIM, ENABLE);
    
    //DMA_ITConfig(LED_DMA_Channel, DMA_IT_TC, ENABLE);							        //DMA传输完成中断
}

/*****************************************************************
* Function Name : LED_DMA_Refresh
* Description   : DMA Buffer 数据更新
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void LED_DMA_Refresh()
{
	LED_DMA_Channel->CCR &= ~DMA_CCR_EN;
	LED_TIM->CR1 &= ~TIM_CR1_CEN;

	LED_DMA_Channel->CMAR = (u32)(m_LED_DMABuff);
	LED_DMA_Channel->CNDTR = LEDBUFF_LEN;
	LED_DMA_Channel->CCR |= DMA_CCR_EN;

	LED_TIM->CNT = 0;
	LED_TIM->CR1 |= TIM_CR1_CEN;// */
}

/*****************************************************************
* Function Name : RGB_Bit_Set
* Description   : 24bit数据更新(每次更新1个RGB的24个位)
* Input         : num:RGB编号 1~2 1:左把转向灯 2:右把转向灯
                :color:要显示的颜色0~7
                : brightness:显示亮度0~0xFF
* Output        : None
* Notes         :
******************************************************************/ 
void RGB_Bit_Set(u8 num, u8 color, u8 brightness)
{
	static u8 RGB_pare[LED_NUM];
	static u8 i = 0;
	static u8 j = 0;

	for(i=0;i<3;i++) //每个RGB三个灯
	{
		if((color & (0x01<<i)) == (0x01<<i))
			RGB_pare[(num-1)*3 + i] = brightness;
		else
			RGB_pare[(num-1)*3 + i] = 0x00;
		
		for(j=0;j<8;j++)
		{
			if((RGB_pare[(num-1)*3 + i]& 0x80) == 0x80)
				m_LED_DMABuff[((num-1)*3 + i)*8+j+2] = LED_CCR_H;
			else
				m_LED_DMABuff[((num-1)*3 + i)*8+j+2] = LED_CCR_L;
			RGB_pare[(num-1)*3 + i] <<= 1;
		}
	}
}

/*****************************************************************
* Function Name : Ambientlight_breath
* Description   : 氛围灯呼吸（调用周期30ms）
* Input         : period:呼吸一次的时间s color：颜色
* Output        : None
* Notes         :
******************************************************************/ 
void Ambientlight_breath(u8 num, u8 period, u8 color)
{
	static u16 s_cnt = 0;
	static u8 brightness = 0;
	static u8 s_dir = 0;
	
	brightness = s_cnt*0xFF/(period*1000/20);
	if(s_dir == 0)
	{
		s_cnt++;
		if(s_cnt > period*1000/20)  //periods呼吸
		{
			s_cnt = period*1000/20;
			s_dir = 1;
		}
	}
	else
	{
		s_cnt--;
		if(s_cnt <= 1)  //periods呼吸
		{
			s_cnt = 1;
			s_dir = 0;
		}
	}
	
	RGB_Bit_Set(num, color, brightness);
}

/*****************************************************************
* Function Name : ALL_RGB_OFF
* Description   : 关闭所有RGB（氛围灯和3个RGB图标）
* Input         : None
* Output        : None
* Notes         :
******************************************************************/ 
void ALL_RGB_OFF(void)
{
	RGB_Bit_Set(RGB_NUM_RGB, RGB_OFF, 0x00);
	//RGB_Bit_Set(RGB_NUM_RIGHT_Ambientlight, RGB_OFF, 0x00);
}

/*****************************************************************
* Function Name : PowerON_RGB
* Description   : RGB（氛围灯和3个RGB图标）开机动画
* Input         : None
* Output        : None
* Notes         :
******************************************************************/ 
void PowerON_RGB(void)
{
	static u8 s_cnt = 0;
	s_cnt++;
	
	if(s_cnt < 2) //500ms
	{
		RGB_Bit_Set(RGB_NUM_RGB, 1/*DIS_Color_Set.PowerOn_color*/, RGB_brightness);
		//RGB_Bit_Set(RGB_NUM_RIGHT_Ambientlight, 1/*DIS_Color_Set.PowerOn_color*/, RGB_brightness);
	}
	else if(s_cnt < 5) 
	{
		RGB_Bit_Set(RGB_NUM_RGB, 2/*DIS_Color_Set.PowerOn_color*/, RGB_brightness);
		//RGB_Bit_Set(RGB_NUM_RIGHT_Ambientlight, 2/*DIS_Color_Set.PowerOn_color*/, RGB_brightness);
		
	}
	else if(s_cnt < 7) 
	{
		RGB_Bit_Set(RGB_NUM_RGB, 4/*DIS_Color_Set.PowerOn_color*/, RGB_brightness);
		//RGB_Bit_Set(RGB_NUM_RIGHT_Ambientlight, 4/*DIS_Color_Set.PowerOn_color*/, RGB_brightness);
	}
	else
	{
		s_cnt = 7;
		RGB_Bit_Set(RGB_NUM_RGB, 8/*DIS_Color_Set.PowerOn_color*/, RGB_brightness);
		//RGB_Bit_Set(RGB_NUM_RIGHT_Ambientlight, 8/*DIS_Color_Set.PowerOn_color*/, RGB_brightness);
	}
	LED_DMA_Refresh();
}


/*****************************************************************
* Function Name : Handle_RGB_Control
* Description   : 氛围灯控制
* Input         : Flash_flage:闪烁控制标志，与其他图标同步闪烁
* Output        : None
* Notes         :
******************************************************************/ 
void Handle_RGB_Control(u8 Flash_flage)  //30ms
{
    u8 s_brightness = 0;
	
#ifdef DEBUG_1
        g_myself_data.RGB_Led.AmbientLightMode = BREATHE_LED;
        g_myself_data.RGB_Led.AmbientLightColor = 7;
        g_myself_data.RGB_Led.AmbientLightLux = 100;
        g_myself_data.RGB_Led.AmbientLight_Period  = 2;
#endif

	switch(g_myself_data.RGB_Led.AmbientLightMode)
	{
	case STOP_LED:          //氛围灯关闭
        RGB_Bit_Set(RGB_NUM_RGB, RGB_OFF, 0x00);
		break;
	case CHANGLIANG_LED:    //常亮
		s_brightness = g_myself_data.RGB_Led.AmbientLightLux * 0xFF / 100;
		RGB_Bit_Set(RGB_NUM_RGB, g_myself_data.RGB_Led.AmbientLightColor, s_brightness);
		break;
//	case 2:  //闪烁
//		s_brightness = g_myself_data.RGB_Led.AmbientLightLux * 0xFF / 100;
//		if(Flash_flage)
//		  RGB_Bit_Set(ambientlight, g_myself_data.RGB_Led.AmbientLightColor, s_brightness);
//		else
//		  RGB_Bit_Set(ambientlight, RGB_OFF, 0x00);
//		break;
	case BREATHE_LED:       //呼吸 
		Ambientlight_breath(RGB_NUM_RGB, g_myself_data.RGB_Led.AmbientLight_Period , g_myself_data.RGB_Led.AmbientLightColor);
		break;
	default:
		break;
	}

	LED_DMA_Refresh(); //DMA Buffer数据更新
}

//void DMA1_Channel4_5_6_7_IRQHandler(void)
//{
//    if (DMA_GetITStatus(DMA1_IT_TC5))
//    {
//        DMA_ClearFlag(DMA1_FLAG_TC5);
//        DMA_ClearITPendingBit(DMA1_IT_GL5);
//    }
//}
//===================================================================================//

//==================================== The End===============================================/


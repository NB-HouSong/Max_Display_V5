/******************** (C) COPYRIGHT 2020 Ninebot *******************************
* File Name     : System_Config.c
* Author        : meihua
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "System_Config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define VECTOR_SIZE   0xC0

/*****************************************************************
* Function Name : SystemCoreClockConfigure
* Description   : configure SystemCoreClock using HSI 
* Input         : None
* Output        : 
* Notes         :
******************************************************************/
void SystemCoreClockConfigure(void)
{
    RCC->CR |= ((uint32_t)RCC_CR_HSION);                     // Enable HSI

    RCC->CR &= ~((uint32_t)RCC_CR_HSEON);                    // Disable HSE

    while ((RCC->CR & RCC_CR_HSIRDY) == 0);                  // Wait for HSI Ready

    RCC->CFGR = RCC_CFGR_SW_HSI;                             // HSI is system clock
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);  // Wait for HSI used as system clock

    FLASH->ACR  = FLASH_ACR_PRFTBE;                          // Enable Prefetch Buffer
    FLASH->ACR |= FLASH_ACR_LATENCY;                         // Flash 1 wait state

    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;                         // HCLK = SYSCLK
    RCC->CFGR |= RCC_CFGR_PPRE_DIV1;                         // PCLK = HCLK

    RCC->CR &= ~RCC_CR_PLLON;                                // Disable PLL

    //  PLL configuration:  = HSI/2 * 12 = 48 MHz
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMUL);
    RCC->CFGR |=  (RCC_CFGR_PLLSRC_HSI_DIV2 | RCC_CFGR_PLLMUL12);

    RCC->CR |= RCC_CR_PLLON;                                 // Enable PLL
    while((RCC->CR & RCC_CR_PLLRDY) == 0) __NOP();           // Wait till PLL is ready

    RCC->CFGR &= ~RCC_CFGR_SW;                               // Select PLL as system clock source
    RCC->CFGR |=  RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  // Wait till PLL is system clock src
}

/*****************************************************************
* Function Name : Peripherals_Clock_Init
* Description   : ����Clock��ʼ��
* Input         : None
* Output        : 
* Notes         :
******************************************************************/
void Peripherals_Clock_Init(FunctionalState NewState)
{
    //��ʱ����Ҫע�⣬�������������һ�����õ����ţ���ͬʱ�����������ʱ���п��ܻ�ʹ������������ɿ�
    //���磬ʹ����USART3��ʹPB10�Զ����ͣ������������ʼ��ʱ��Ϊ���츴������Ļ���
    //ʹ�ܶ˿�ʱ��
	
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, NewState);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, NewState);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, NewState);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, NewState);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, NewState);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, NewState);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, NewState);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, NewState);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, NewState);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,NewState);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17,NewState);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, NewState);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, NewState);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, NewState);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, NewState);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, NewState);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, NewState);
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,NewState);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,NewState);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART4,NewState);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN,NewState);
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, NewState);
//		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, NewState);		
	/* Enable the SPI periph */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, NewState);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, NewState);
}

/*****************************************************************
* Function Name : NVIC_Configuration
* Description   : �жϳ�ʼ��
* Input         : None
* Output        : 
* Notes         :
******************************************************************/
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel =  TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_3_IRQn;
    NVIC_Init(&NVIC_InitStructure);
}

/*****************************************************************
* Function Name : InitAllPeripherals
* Description   : �����ʼ��
* Input         : None
* Output        : 
* Notes         :
******************************************************************/
void InitAllPeripherals( void )
{
	static u8 First_in_flage = 0;
	if(First_in_flage == 0) //ֻ���ϵ��ʼ��ʱ���ã��˳��͹���ʱ��������
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		memcpy((void*)0x20000000, (void*)FADD_FORMAL_SW, VECTOR_SIZE);
		SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);
		IAP_Init();
	}

    NVIC_Configuration();		
    Peripherals_Clock_Init(ENABLE);
	//POWER_CTR_IO_INIT();
    delay_init();  

    Handle_Timer_Init();
    RGB_Time_Init();                                //����ת���Χ��
    Ambient_light_Start(&Ambient_light_object);     //���̷�Χ��
	
    // TM1637 ��ʾ��ʼ��
    Timer2Init();
    IIC_Init();
    
    //����ͨ�ų�ʼ��
	UART3_Int();            //���ڳ�ʼ�� ��˫������ͨ��
	ADC_Int();			    //ADC
    gpioInitialize();
    
    /*NFC ��ʼ������*/
    SPI1_Configuration();   //NFC 17550ͨѶ����		
	FM175XX_HardReset();
    if(Lpcd_Calibration_Event()== SUCCESS)
        Lpcd_Calibration_Backup();
    Lpcd_Set_Mode(1);       //����LPCDģʽ
 
    EXTI_Config();          //�������棬���������ж�
}

/*****************************************************************
* Function Name : ReadUniqueID
* Description   : ��ȡCPUΨһID
* Input         : None
* Output        : 
* Notes         :
******************************************************************/
void ReadUniqueID(void)
{
	//��ȡCPUΨһID
	g_myself_data.DIS_CPID.Data32[0] = *(vu32*)(0x1ffff7ac);
	g_myself_data.DIS_CPID.Data32[1] = *(vu32*)(0x1ffff7B0);
	g_myself_data.DIS_CPID.Data32[2] = *(vu32*)(0x1ffff7B4);
}

/*****************************************************************
* Function Name : Variables_Init
* Description   : ������ʼ��
* Input         : None
* Output        : 
* Notes         :
******************************************************************/
void Variables_Init(void)
{
    //��������
	memset((u8*)g_bool, 0, B_LEN);
	
	Flash_Read(FADD_FDATA_MAP, (u8*)g_NBCmdMap, ((NB_CMDMAP_LEN)<<1));  //ȡ�����������
	memcpy((u8*)&g_myself_data.QRCode_Info.Virtual_QRcode_L, (u8*)&g_NBCmdMap[0], 16);
	memcpy((u8*)&g_myself_data.DIS_PN, (u8*)&g_NBCmdMap[NB_DIS_PN], 14);

	//��ȡоƬID
	ReadUniqueID();	
  
    g_myself_data.DIS_Version_sw.Dis_SoftwareVersion = FW_VERSION;
    
    //g_myself_data.Handle_Bar_Info.HW_Version = ( (gpioGetInput(GpioVsersionB) << 1) | gpioGetInput(GpioVsersionA) );//Ĭ��V5�Ǳ�Ӳ���汾Ϊ3������ 0x01˫ɲ
    g_myself_data.Handle_Bar_Info.HW_Version = 3;       //Ĭ��V5�Ǳ�Ӳ���汾Ϊ3������ 0x01˫ɲ

	NFC_set.NFC_CLA_STATE = 0;      //Ĭ������У׼״̬Ϊ0
	NFC_set.SET_NFC_CALIBRATE = 0;  //Ĭ��������У׼ָ��

    Ambient_Light_Malloc_Init(&Ambient_light_object);
}

/*****************************************************************
* Function Name : Disable_Interrupt
* Description   : �����ж� 
* Input         : None
* Output        : 
* Notes         :
******************************************************************/
void Disable_Interrupt(void)
{
	__disable_irq();   //�ر����ж�
}

/*****************************************************************
* Function Name : Enable_Interrupt
* Description   : �������ж� 
* Input         : None
* Output        : 
* Notes         :
******************************************************************/
void Enable_Interrupt(void)
{
	__enable_irq();		//�������ж�
}

/*****************************************************************
* Function Name : SetErrorByte
* Description   : ���ù����� 
* Input         : None
* Output        : 
* Notes         :
******************************************************************/
void SetErrorByte(u8 ErrorByte)
{
    if(g_ErrorByte >= ErrorByte || g_ErrorByte == 0)
    {
        g_ErrorByte = ErrorByte;
    }
}

void ClearErrorByte(u8 ErrorByte)  //ֱ�������������
{
    if(g_ErrorByte == ErrorByte)
    {
        g_ErrorByte = 0;
    }
}

/*****************************************************************
* Function Name : delay_init
* Description   : ��ʱ������ʼ�� 
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
static u8  fac_us=0;							//us��ʱ������			   
static u16 fac_ms=0;							//ms��ʱ������,��ucos��,����ÿ�����ĵ�ms��	   
void delay_init(void)
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	fac_us= SystemCoreClock/1000000;				//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
    
	reload= SystemCoreClock/1000000;				//ÿ���ӵļ������� ��λΪM  
	reload *=1000000/configTICK_RATE_HZ;			//����configTICK_RATE_HZ�趨���ʱ��
												    //reloadΪ24λ�Ĵ���,���ֵ:16777216
	fac_ms=1000/configTICK_RATE_HZ;				    //����OS������ʱ�����ٵ�λ	   

	//SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD =  reload; 						//ÿ1/configTICK_RATE_HZ���ж�һ��	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK   
//	SysTick->VAL = 0xFFFFFFFF; 	
}								     

/*****************************************************************
* Function Name : delay_us
* Description   : ��ʱnus
* Input         : nus:Ҫ��ʱ��us��. 
                : nus:0~204522252(���ֵ��2^32/fac_us@fac_us=168)	
* Output        : None
* Notes         :
******************************************************************/  			    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD��ֵ	    	 
	ticks=nus*fac_us; 						  //��Ҫ�Ľ����� 
	told=SysTick->VAL;        			//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)
                break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};										    
}  

/*****************************************************************
* Function Name : delay_ms
* Description   : ��ʱnms
* Input         : nms:Ҫ��ʱ��ms��
                : nms:0~65535
* Output        : None
* Notes         :
******************************************************************/  	
void delay_ms(u16 nms)
{	
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//ϵͳ�Ѿ�����
	{		
		if(nms>=fac_ms)						//��ʱ��ʱ�����OS������ʱ������ 
		{ 
   			vTaskDelay(nms/fac_ms);	 		//FreeRTOS��ʱ
		}
		nms%=fac_ms;						//OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
	}
	delay_us((u32)(nms*1000));				//��ͨ��ʽ��ʱ
}

/*****************************************************************
* Function Name : SaveFDatamap
* Description   : ���ݵ��籣��
* Input         : None
* Output        : ���ݱ���Ľ��
* Notes         :
******************************************************************/  	
s32 SaveFDatamap(void)
{
    s32 result = 0;
    u32 dispose_flash_status = FLASH_ERASE;
    u32 failure_Cnt =0;
    u32 failure_category =0;
    u32 erase_result =0;
    u32 write_result =0;
    u16 *p_flash_cnt = 0;
	
    if(g_NBCmdMap[NB_INF_FLASH_CNT_RECORD] >= B_FLASH_CATCH)
    {
        g_NBCmdMap[NB_INF_FLASH_CNT_RECORD] = 0;
    }
    else if(g_NBCmdMap[NB_INF_FLASH_CNT_RECORD] > B_FLASH_MAX_WR_CNT)//??6000?
    {
        result = FLASH_DAMAGE;
        return result;
    }
		
    Disable_Interrupt();
    while(dispose_flash_status!= FLASH_SUCCESS && dispose_flash_status != FLASH_DAMAGE)//???3?
    {
        switch(dispose_flash_status)
        {
        case FLASH_ERASE:

            erase_result = Flash_Prepared(FADD_FDATA_MAP, ((NB_CMDMAP_LEN)<<1));
            if(erase_result == 1)
            {
                dispose_flash_status = FLASH_WRITE;
                if(failure_category == ERASE_FAILURE)
                {
                    failure_Cnt =0;
                }
            }
            else if(erase_result == 0)
            {
                dispose_flash_status = FLASH_FAILURE;
                failure_category = ERASE_FAILURE;
            }
            break;

        case FLASH_WRITE:
			memcpy((u8*)&g_NBCmdMap[0], (u8*)&g_myself_data.QRCode_Info.Virtual_QRcode_L, 16);
			write_result = Flash_Write(FADD_FDATA_MAP, (const u8*)g_NBCmdMap, ((NB_CMDMAP_LEN)<<1));
            p_flash_cnt = (u16*)&g_NBCmdMap[NB_INF_FLASH_CNT_RECORD];
            if(*p_flash_cnt < 0xFFFF)
            {
                (*p_flash_cnt)++;
            }

            if(write_result ==1)
            {
                dispose_flash_status = FLASH_SUCCESS;//??
                failure_Cnt =0;
                result = FLASH_SUCCESS;
                break;
            }
            else if(write_result ==0)
            {
                dispose_flash_status = FLASH_FAILURE;
                failure_category = WRITE_FAILURE;
            }

            break;

        case FLASH_FAILURE:

            if(failure_Cnt < 2)//
            {
                failure_Cnt++;
                dispose_flash_status = FLASH_ERASE;
            }
            else
            {
                failure_Cnt =0;
                dispose_flash_status = FLASH_TIMOUT;//
            }
            break;

        case FLASH_TIMOUT:

            dispose_flash_status = FLASH_DAMAGE;
            result = FLASH_DAMAGE;

            break;

        default:
            break;
        }
    }

    Enable_Interrupt();

    return result;
}

#if 0
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->ISR&0X40)==0);//ѭ������,ֱ���������   
    USART1->TDR = (u8) ch;      
	return ch;
}
#endif 

//==================================== The End===============================================/

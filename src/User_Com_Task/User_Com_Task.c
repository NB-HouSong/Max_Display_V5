/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "UartProtocol.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
QueueHandle_t COM1_Message_Queue;	//信息队列句柄

/*****************************************************************
* Function Name : COM1_Task
* Description   : 通讯任务
* Input         : None
* Output        : None
* Notes         :
******************************************************************/	
void COM1_Task(void *pvParameters)
{
	static u8 s_reset_delay = 0;

    while(1)
    {
		//是否保存Flash
		if(g_bool[B_SAVE_FDATA])
		{
			if(SaveFDatamap() == FLASH_SUCCESS)
				g_bool[B_SAVE_FDATA] = 0;
			s_reset_delay = 255;
		}
		if(s_reset_delay > 0)
			s_reset_delay --;

		//是否重启
		if(g_bool[B_RESET] && s_reset_delay <= 0)
		{
			NVIC_SystemReset();
		}

		UART3_CheckSend();
		UART3_CheckReceive();

        vTaskDelay(1); 
    }
}


//==================================== The End===============================================/

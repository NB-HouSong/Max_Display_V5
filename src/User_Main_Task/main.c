/******************** (C) COPYRIGHT 2020 Ninebot *******************************
* File Name     : main.c
* Author        : meihua
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//�������ȼ�
#define START_TASK_PRIO			1
#define DISPLAY_TASK_PRIO		2
#define CTRL_TASK_PRIO			3
#define COM1_TASK_PRIO			4

//��ջ��С
#define START_STK_SIZE 			128
#define DISPLAY_STK_SIZE 		128  
#define CTRL_STK_SIZE 			128  
#define COM1_STK_SIZE 			128 

#define DEBUG

/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
//������
TaskHandle_t StartTask_Handler;
TaskHandle_t Task1Task_Handler;
TaskHandle_t Task2Task_Handler;
TaskHandle_t Task3Task_Handler;

/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/*****************************************************************
* Function Name : start_task
* Description   : ��ʼ�������񴴽� 
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    COM1_Message_Queue= xQueueCreate(COM1MSG_Q_NUM,sizeof(u8));
	
    xTaskCreate((TaskFunction_t )Display_Task,             
                (const char*    )"task1_task",           
                (uint16_t       )DISPLAY_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )DISPLAY_TASK_PRIO,        
                (TaskHandle_t*  )&Task1Task_Handler);   

    xTaskCreate((TaskFunction_t )Control_Task,     
                (const char*    )"task2_task",   
                (uint16_t       )CTRL_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )CTRL_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler); 
								
	xTaskCreate((TaskFunction_t )COM1_Task,     
                (const char*    )"task3_task",   
                (uint16_t       )COM1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )COM1_TASK_PRIO,
                (TaskHandle_t*  )&Task3Task_Handler); 

    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

/*****************************************************************
* Function Name : main
* Description   : ��ʼ�������񴴽�
* Input         : None
* Output        : int
* Notes         :
******************************************************************/
int main(void)
{
	SystemCoreClockConfigure();
    SystemCoreClockUpdate();

    Variables_Init();
    InitAllPeripherals();

#ifndef DEBUG
	if( FLASH_OB_GetRDP()!= SET )  //������
	{
		FLASH_Unlock();
		FLASH_OB_Unlock();
		FLASH_OB_RDPConfig(OB_RDP_Level_1);
		FLASH_OB_Launch();
		FLASH_OB_Lock();
		FLASH_Lock();
	}
#endif 

    xTaskCreate((TaskFunction_t )start_task,           
                (const char*    )"start_task",          
                (uint16_t       )START_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )START_TASK_PRIO,       
                (TaskHandle_t*  )&StartTask_Handler);
				
    vTaskStartScheduler();  
    
    while(1);
}


//====================================================================================//

//==================================== The End===============================================/




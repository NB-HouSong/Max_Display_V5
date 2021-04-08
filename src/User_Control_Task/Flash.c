/******************** (C) COPYRIGHT 2020 Ninebot *******************************
* File Name     : Flash.c
* Author        : meihua
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "Flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/*****************************************************************
* Function Name : Flash_PagesMask
* Description   : 计算将会用到多少页Flash（Size字节为单位）
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
u32 Flash_PagesMask(vu32 Size)
{
    u32 pagenumber = 0x0;
    u32 size = Size;

    if ((size % PAGE_SIZE) != 0)
    {
        pagenumber = (size / PAGE_SIZE) + 1;
    }
    else
    {
        pagenumber = size / PAGE_SIZE;
    }
    return pagenumber;

}

/*****************************************************************
* Function Name : Flash_Prepared
* Description   : 准备flash空间，擦除需要编程的页，Len以字节为单位
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
s32 Flash_Prepared(u32 Address, u32 Len)
{
    u32 NbrOfPage = 0;
    s32 i;

    FLASH_Status FLASHStatus = FLASH_COMPLETE;
    u32 FlashDestination = Address;

    FLASH_Unlock();
    NbrOfPage = Flash_PagesMask(Len);
    //擦除需要编程的页
    for (i=0; i<NbrOfPage; i++)
    {
        FLASHStatus = FLASH_ErasePage(FlashDestination + (PAGE_SIZE * i));
        if(FLASHStatus != FLASH_COMPLETE)
            break;
    }
    FLASH_Lock();
    if(i != NbrOfPage)
        return 0;
    return 1;
}

/*****************************************************************
* Function Name : Flash_Write
* Description   : 以字节为单位将数据写入Flash
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
s32 Flash_Write(u32 Address, const u8* pData, u32 Len)
{
    s32 i;
    u32 FlashDestination = Address;
    u32 FlashSource;
    FLASH_Status FLASHStatus = FLASH_COMPLETE;

    FLASH_Unlock();
    //开始编程
    FlashSource = (u32)pData;
    for (i=0; i<Len; i+=2)
    {
        FLASHStatus =  FLASH_ProgramHalfWord(FlashDestination, *(u16*)FlashSource);
        if(FLASHStatus != FLASH_COMPLETE)
        {
            break;
        }
        if (*(u16*)FlashDestination != *(u16*)FlashSource)
        {
            break;
        }
        FlashDestination += 2;
        FlashSource += 2;
    }
    FLASH_Lock();
    if(i < Len)
        return 0;
    return 1;
}

/*****************************************************************
* Function Name : Flash_Write_Word
* Description   : 以字为单位将数据写入Flash
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
s32 Flash_Write_Word(u32 Address, const u8* pData, u32 Len)
{
    s32 i;
	u32 FlashSource_data = 0;
	u32 FlashDestination_data = 0;
    FLASH_Status FLASHStatus = FLASH_COMPLETE;

    FLASH_Unlock();
    //开始编程

    for (i=0; i<Len; i+=4)
    {
		memcpy(&FlashSource_data, &pData[i], 4);
        FLASHStatus =  FLASH_ProgramWord(Address+i, FlashSource_data);
        if(FLASHStatus != FLASH_COMPLETE)
        {
            break;
        }
		memcpy(&FlashDestination_data, (u8*)(Address+i), 4);
        if (FlashDestination_data != FlashSource_data)
        {
            break;
        }
    }
    FLASH_Lock();
    if(i < Len)
        return 0;
    return 1;
}

/*****************************************************************
* Function Name : Flash_Read
* Description   : 以字节单位读取Flash中的数据
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void Flash_Read(u32 Address, u8* Readbuff, u32 Len)
{
    s32 i;
    for(i=0; i<Len; i++)
    {
        Readbuff[i] = ((vu8*)Address)[i];
    }
} // */


//====================================================================================//

//==================================== The End===============================================/

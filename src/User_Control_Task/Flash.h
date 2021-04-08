/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : Flash.h
* Author        : 李美华
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FLASH_H
#define FLASH_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "ParseFrame.h"


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define PAGE_SIZE   (0x800)	   		//STM32内部Flash每页大小，注意V8是中等Flash，每页1K，VC是每页2K

//flash地址宏定义
#define FADD_BASE			    0x08000000 		                //Flash基地址
#define FADD_FORMAL_SW		    0x08001000 		                //正式程序地址          //4K  //基地址设置按照4的倍数（按页烧写）
#define FADD_TEMP_SW			(FADD_FORMAL_SW + 0xF000) 	    //临时程序地址	        //60K+4K = 64k   0x0800D800
#define FADD_FDATA_MAP		    (FADD_TEMP_SW + 0xF000) 	    //flash数据表起始地址	//64K+60K = 124k      	0x0801A000		
#define FADD_FCTRL_MAP		    (FADD_FDATA_MAP + PAGE_SIZE)    //IAP控制表起始地址	    //124+2k 126k

#define Flash_Inited(Address)	((uint32_t)( *(volatile uint32_t*)(Address) ) != 0xFFFFFFFF)
	
/* Exported types ------------------------------------------------------------*/
typedef enum
{
    FLASH_ERASE=0,
    FLASH_WRITE,
    FLASH_FAILURE,
    FLASH_SUCCESS,
    FLASH_TIMOUT,
    FLASH_DAMAGE,
}FLASH_STATUS;

/* Exported functions ------------------------------------------------------- */
s32 Flash_Prepared(u32 Address, u32 Len);
s32 Flash_Write(u32 Address, const u8* pData, u32 Len);
void Flash_Read(u32 Address, u8* Readbuff, u32 Len);
s32 Flash_Write_Word(u32 Address, const u8* pData, u32 Len);

#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

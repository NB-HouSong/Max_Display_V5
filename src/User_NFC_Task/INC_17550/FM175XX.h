/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : FM175XX.h
* Author        : 李美华
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _FM175XX_H
#define _FM175XX_H

/* Includes ------------------------------------------------------------------*/
#include "DEFINE.h"
#include "System_Config.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void FM175XX_HardReset(void);
unsigned char FM175XX_SoftReset(void);
void FM175XX_HPD(unsigned char mode);
void GetReg(unsigned char addr,unsigned char* regdata);
void SetReg(unsigned char addr,unsigned char regdata);
void ModifyReg(unsigned char addr,unsigned char mask,unsigned char set);
void SPI_Write_FIFO(unsigned char reglen,unsigned char* regbuf); //SPI接口连续写FIFO 
void SPI_Read_FIFO(unsigned char reglen,unsigned char* regbuf);  //SPI接口连续读FIFO

void Clear_FIFO(void);
void GetReg_Ext(unsigned char ExtRegAddr,unsigned char* ExtRegData);
void SetReg_Ext(unsigned char ExtRegAddr,unsigned char ExtRegData);
void ModifyReg_Ext(unsigned char ExtRegAddr,unsigned char mask,unsigned char set);
#endif
/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

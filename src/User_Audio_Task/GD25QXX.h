/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : main.h
* Author        : 李美华
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _GD25QXX_H
#define _GD25QXX_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#define GD25Q32BSIGDeviceID				    0x15
#define GD25Q32BSIGManufactureID			0xC815
#define GD25Q32BSIGIdentificationID			0xC84016//0xC84017//0xC84016

#define GD25Q32_BASE_ADDR						0x000000U
#define GD25Q32_CHIP_BYTE_SIZE					0x400000U	//4*1024 *1024 bytes = 64blocks												// 1 M
#define GD25Q32_BLOCK_BYTE_SIZE					0x010000U	//65536 = 4096*16 bytes = 16 sector												//64 K
#define GD25Q32_SECTOR_BYTE_SIZE				0x001000U	//4096 = 256*16 bytes = 16 pages												// 4 K
#define GD25Q32_PAGE_BYTE_SIZE					0x000100U	//256 bytes												//256Byte

//#define GD25Q32_BLOCK_ADDRESS15				0x0F0000U
#define GD25Q32_BLOCK_ADDRESS(nBlock)			(GD25Q32_BASE_ADDR + (GD25Q32_BLOCK_BYTE_SIZE * nBlock))
#define GD25Q32_SECTOR_ADDRESS(nSector)		(GD25Q32_BASE_ADDR + (GD25Q32_SECTOR_BYTE_SIZE * nSector))
#define GD25Q32_PAGE_ADDRESS(nPage)				(GD25Q32_BASE_ADDR + (GD25Q32_PAGE_BYTE_SIZE * nPage))
#define GD25Q32_BLOCK_NUM						  64
#define GD25Q32_SECTOR_NUM						1024
#define GD25Q32_PAGE_NUM						  16384

#define WAVE_FILE_LEN									    (uint32_t)(200*1024)
#define _0_START_ADDR									    (uint32_t)0x00001000
#define _1_START_ADDR									    (_0_START_ADDR + WAVE_FILE_LEN)
#define _2_START_ADDR									    (_1_START_ADDR + WAVE_FILE_LEN)
#define _3_START_ADDR									    (_2_START_ADDR + WAVE_FILE_LEN)
#define _4_START_ADDR									    (_3_START_ADDR + WAVE_FILE_LEN)
#define _5_START_ADDR									    (_4_START_ADDR + WAVE_FILE_LEN)
#define _6_START_ADDR									    (_5_START_ADDR + WAVE_FILE_LEN)
#define _7_START_ADDR									    (_6_START_ADDR + WAVE_FILE_LEN)
#define _8_START_ADDR									    (_7_START_ADDR + WAVE_FILE_LEN)
#define _9_START_ADDR									    (_8_START_ADDR + WAVE_FILE_LEN)
#define _10_START_ADDR									  (_9_START_ADDR + WAVE_FILE_LEN)
#define _11_START_ADDR									  (_10_START_ADDR + WAVE_FILE_LEN)
#define _12_START_ADDR									  (_11_START_ADDR + WAVE_FILE_LEN)
#define _13_START_ADDR									  (_12_START_ADDR + WAVE_FILE_LEN)

#define USE_SPI  SPI2


/* Definition for SPIx Pins */
//CS
#define GD25Q_FLASH_SPIx_NSS_GPIO_PORT			GPIOB
#define GD25Q_FLASH_SPIx_NSS_PIN				GPIO_Pin_12

//===============================================
#define GD25Q_FLASH_SPIx_NSS_ENABLE()			(GD25Q_FLASH_SPIx_NSS_GPIO_PORT->BRR = GD25Q_FLASH_SPIx_NSS_PIN)
#define GD25Q_FLASH_SPIx_NSS_DISABLE()		(GD25Q_FLASH_SPIx_NSS_GPIO_PORT->BSRR = GD25Q_FLASH_SPIx_NSS_PIN)
#define	GD25Q_FLASH_SPIx_CS   					PBout(1)   //CS
#define	GD25Q_FLASH_SPIx_CLK  					PAout(11)  //CLK
#define	GD25Q_FLASH_SPIx_DO   					PBout(11)  //DO
#define	GD25Q_FLASH_SPIx_DI()   				PBin(10)   //DI

//===============================================
#define GD25Q_Dummy_Byte						0xFF

/* GD25QXXCSIG 指令表 */
#define GD25Q_WriteEnable						0x06
#define GD25Q_WriteDisable						0x04
#define GD25Q_ReadStatusReg1					0x05
#define GD25Q_ReadStatusReg2					0x35
#define GD25Q_WriteStatusReg					0x01
#define GD25Q_ReadData							0x03
#define GD25Q_FastReadData						0x0B
#define GD25Q_FastReadDual						0x3B
#define GD25Q_PageProgram						0x02
#define GD25Q_SectorErase						0x20
#define GD25Q_BlockErase						0xD8
#define GD25Q_ChipErase							0xC7
#define GD25Q_PowerDown							0xB9
#define GD25Q_ReleasePowerDown					0xAB
#define GD25Q_DeviceID							0xAB
#define GD25Q_ManufactDeviceID					0x90
#define GD25Q_JedecDeviceID						0x9F

/* GD25Q80CSIG End Command */
typedef enum
{
	GD25Q80CSIG_OK						= 0x00,
	GD25Q80CSIG_ERROR					= 0x01
}GD25Q_StatusTypeDef;

extern GD25Q_StatusTypeDef GD25QStatus;
extern u8 GD25Q_SPI_FLASH_SendByte(u8 byte);	
/* Exported functions ------------------------------------------------------- */
void GD25Q_SPIFLASH_Init(void);								//GD25Q SPIFLASH 初始化
u8 GD25Q_SPI_FLASH_ReadByte(void);	 //SPI FLASH 读取一个字节数据


u32 GD25Q_SPIFLASH_ReadDeviceID(void);			//GD25Q SPIFLASH 读取设备ID
u32 GD25Q_SPIFLASH_ReadManufactureID(void);		//GD25Q SPIFLASH 读取厂商ID
u32 GD25Q_SPIFLASH_ReadIdentificationID(void);	//GD25Q SPIFLASH 读取型号ID
void GD25Q_SPIFLASH_WriteEnable(void);			//GD25Q SPIFLASH 写使能
void GD25Q_SPIFLASH_WriteDisable(void);			//GD25Q SPIFLASH 写禁止
void GD25Q_SPIFLASH_WaitForBusy(void);			//GD25Q SPIFLASH 等待设备空闲
void GD25Q_SPIFLASH_PowerDown(void);			//GD25Q SPIFLASH 进入掉电模式
void GD25Q_SPIFLASH_WakeUp(void);				//GD25Q SPIFLASH 唤醒
void GD25Q_SPIFLASH_EraseSector(u32 SectorAddr);	//GD25Q SPIFLASH 擦除Sector

void GD25Q_SPIFLASH_ReadBuffer(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);	//GD25Q SPIFLASH 读取SPI FLASH数据
void GD25Q_SPIFLASH_WritePage(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);	//GD25Q SPIFLASH 写入SPI FLASH数据Page
unsigned char GD25Q_SPIFLASH_GetByte(u32 ReadAddr);			//GD25Q SPIFLASH 读取1个Byte

#endif 

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

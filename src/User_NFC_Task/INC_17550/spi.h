/******************** (C) COPYRIGHT 2020  Ninebot ********************
* File Name     : spi.h
* Author        : 李美华
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
**********************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SPI_H_
#define _SPI_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "REDEFINE.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define	SPI_SUCCESS							0
#define	SPI_TIMEOUT							1
#define	SPI_ILLEGAL							2
#define SPI_DATAERROR						3
#define SPI_FIRSTTIME						4

#define  NFC_SPI    SPI1
#define  FLASH_SPI  SPI2

#define  SPI_GPIO_PORT      GPIOB
#define  SPI_PIN_CLK        GPIO_Pin_3
#define  SPI_PIN_MOSI       GPIO_Pin_5    
#define  SPI_PIN_MISO       GPIO_Pin_4

#define  PORT_NSS			GPIOA
#define  PIN_NSS			GPIO_Pin_15		// CS 17550

//#define  SPI2_PIN_CLK       GPIO_Pin_13
//#define  SPI2_PIN_MOSI      GPIO_Pin_15    
//#define  SPI2_PIN_MISO      GPIO_Pin_14

//#define  SPI2_CS_PORT				GPIOB
//#define  SPI2_CS_PIN				GPIO_Pin_12		// CS FLASH

/* Exported types ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void EXTI_Config(void);
void SPI1_Configuration(void);
void SPI2_Configuration(void);             
u8 SPI_ReadWriteByte(u8 TxData, u8* pRxData, SPI_TypeDef* SPIx);   //SPI总线读写一个字节
#endif

/************************ (C) COPYRIGHT Ninebot *****END OF FILE****/

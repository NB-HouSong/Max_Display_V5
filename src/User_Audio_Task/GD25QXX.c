///******************** (C) COPYRIGHT 2020 Ninebot *******************************
//* File Name     : GD25QXX.c
//* Author        : meihua
//* Version       : V1.0.0
//* Date          : 2020-11-13
//* Description   : SPI FLASH GD25Q32 Devic
//********************************************************************************/

///* Includes ------------------------------------------------------------------*/  
//#include "GD25QXX.h" 

///* Private typedef -----------------------------------------------------------*/
///* Private define ------------------------------------------------------------*/
///* Private macro -------------------------------------------------------------*/
///* Variables -----------------------------------------------------------------*/
//GD25Q_StatusTypeDef GD25QStatus = GD25Q80CSIG_OK;

///* Private function prototypes -----------------------------------------------*/
///* Functions -----------------------------------------------------------------*/

///*****************************************************************
//* Function Name : GD25Q_SPI2_HARDWARE_Init
//* Description   : 语音Flash SPI相关初始化 
//* Input         : None
//* Output        : None
//* Notes         :
//******************************************************************/
//void GD25Q_SPI2_HARDWARE_Init(void)
//{
//    SPI_InitTypeDef  SPI_InitStructure;
//    GPIO_InitTypeDef GPIO_InitStructure;
//  
//    GPIO_PinAFConfig(SPI_GPIO_PORT, GPIO_PinSource13, GPIO_AF_0);
//    GPIO_PinAFConfig(SPI_GPIO_PORT, GPIO_PinSource14, GPIO_AF_0);
//    GPIO_PinAFConfig(SPI_GPIO_PORT, GPIO_PinSource15, GPIO_AF_0);

//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
//  
//   /* SPI SCK pin configuration */
//    GPIO_InitStructure.GPIO_Pin = SPI2_PIN_CLK;
//    GPIO_Init(SPI_GPIO_PORT, &GPIO_InitStructure);

//  /* SPI  MOSI pin configuration */
//    GPIO_InitStructure.GPIO_Pin =  SPI2_PIN_MOSI;
//    GPIO_Init(SPI_GPIO_PORT, &GPIO_InitStructure);

//  /* SPI MISO pin configuration */
//    GPIO_InitStructure.GPIO_Pin = SPI2_PIN_MISO;
//    GPIO_Init(SPI_GPIO_PORT, &GPIO_InitStructure); 
//	
//	GPIO_InitStructure.GPIO_Pin = SPI2_CS_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
//	GPIO_Init(SPI2_CS_PORT, &GPIO_InitStructure);
//	GPIO_SetBits(SPI2_CS_PORT,SPI2_CS_PIN);
//		
//    //SPI初始化
//	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
//	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
//	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
//	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//	SPI_InitStructure.SPI_CRCPolynomial = 7;
//	SPI_Init(SPI2, &SPI_InitStructure);
//  
//    SPI_RxFIFOThresholdConfig(SPI2, SPI_RxFIFOThreshold_QF);
//	SPI_Cmd(SPI2, ENABLE);
//}

///*****************************************************************
//* Function Name : SPI2_HardWare_ReadWriteByte
//* Description   : 通过SPI按字节读写语音Flash 
//* Input         : TxData:要发送的数据
//* Output        : 读出的数据
//* Notes         :
//******************************************************************/
//u8 SPI2_HardWare_ReadWriteByte(u8 TxData)
//{		
//	u8 retry = 0;				 	
//	while(SPI_I2S_GetFlagStatus(USE_SPI, SPI_I2S_FLAG_TXE) == RESET) 
//	{
//		retry++;
//		if(retry>200)return 0;
//	}			  
//	SPI_SendData8(USE_SPI, TxData);
//	retry=0;

//	while (SPI_I2S_GetFlagStatus(USE_SPI, SPI_I2S_FLAG_RXNE) == RESET)
//	{
//		retry++;
//		if(retry>200)return 0;
//	}	  						    
//	return SPI_ReceiveData8(USE_SPI); 				    
//}

///*****************************************************************
//* Function Name : GD25Q_SPIFLASH_Init
//* Description   : 语音Flash相关初始化 
//* Input         : None
//* Output        : None
//* Notes         :
//******************************************************************/
//void GD25Q_SPIFLASH_Init(void)
//{		
//	GD25Q_SPI2_HARDWARE_Init();
//	SPI2_HardWare_ReadWriteByte(GD25Q_Dummy_Byte);//启动
//	GD25Q_SPIFLASH_WakeUp(); 
//	
//	GD25QStatus = GD25Q80CSIG_OK;

//	if(GD25Q_SPIFLASH_ReadIdentificationID() != GD25Q32BSIGIdentificationID)	
//	{
//		GD25QStatus = GD25Q80CSIG_ERROR;
//	}
////	GD25Q_SPIFLASH_PowerDown();  
//}

///*****************************************************************
//* Function Name : GD25Q_SPI_FLASH_SendByte
//* Description   : 通过SPI向语音Flash发送单字节 
//* Input         : byte:要发送的数据
//* Output        : 结果
//* Notes         :
//******************************************************************/
//u8 GD25Q_SPI_FLASH_SendByte(u8 byte)
//{
//    static u8 readByte = 0x88;	
//	
//    readByte = SPI2_HardWare_ReadWriteByte(byte);
//	return readByte;
//}

///*****************************************************************
//* Function Name : GD25Q_SPIFLASH_ReadDeviceID
//* Description   : 读取设备ID
//* Input         : None
//* Output        : 设备ID
//* Notes         :
//******************************************************************/
//u32 GD25Q_SPIFLASH_ReadDeviceID(void)
//{
//	u32 deviceID = 0;
//	/* 选择SPI FLASH, NSS使能 */
//	GD25Q_FLASH_SPIx_NSS_ENABLE();	
//	GD25Q_SPI_FLASH_SendByte(GD25Q_DeviceID);
//	GD25Q_SPI_FLASH_SendByte(GD25Q_Dummy_Byte);
//	GD25Q_SPI_FLASH_SendByte(GD25Q_Dummy_Byte);
//	GD25Q_SPI_FLASH_SendByte(GD25Q_Dummy_Byte);

//    deviceID = GD25Q_SPI_FLASH_SendByte(GD25Q_Dummy_Byte); 
//    
//	/* 禁用SPI FLASH, NSS失能 */
//	GD25Q_FLASH_SPIx_NSS_DISABLE();	
//	return deviceID;
//}

///*****************************************************************
//* Function Name : GD25Q_SPIFLASH_ReadManufactureID
//* Description   : 读取厂商ID
//* Input         : None
//* Output        : 厂商ID
//* Notes         :
//******************************************************************/
//u32 GD25Q_SPIFLASH_ReadManufactureID(void)
//{
//	u32 manufactureID = 0;	
//	/* 选择SPI FLASH, NSS使能 */
//	GD25Q_FLASH_SPIx_NSS_ENABLE();

//	GD25Q_SPI_FLASH_SendByte(GD25Q_ManufactDeviceID);//90
//	GD25Q_SPI_FLASH_SendByte(0x00);
//	GD25Q_SPI_FLASH_SendByte(0x00);
//	GD25Q_SPI_FLASH_SendByte(0x00);

//	manufactureID |= GD25Q_SPI_FLASH_SendByte(GD25Q_Dummy_Byte) << 8;
//	manufactureID |= GD25Q_SPI_FLASH_SendByte(GD25Q_Dummy_Byte);
//	 	
//	/* 禁用SPI FLASH, NSS失能 */
//	GD25Q_FLASH_SPIx_NSS_DISABLE();	
//	return manufactureID;
//}

///*****************************************************************
//* Function Name : GD25Q_SPIFLASH_ReadIdentificationID
//* Description   : 读取型号ID
//* Input         : None
//* Output        : 型号ID
//* Notes         :
//******************************************************************/
//u32 GD25Q_SPIFLASH_ReadIdentificationID(void)
//{
//	u32 identificationID = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;	

//	/* 选择SPI FLASH, NSS使能 */
//	GD25Q_FLASH_SPIx_NSS_ENABLE();	
//	GD25Q_SPI_FLASH_SendByte(GD25Q_JedecDeviceID);//9F

//	Temp0 = GD25Q_SPI_FLASH_SendByte(GD25Q_Dummy_Byte);
//	Temp1 = GD25Q_SPI_FLASH_SendByte(GD25Q_Dummy_Byte);
//	Temp2 = GD25Q_SPI_FLASH_SendByte(GD25Q_Dummy_Byte); 

//	/* 禁用SPI FLASH, NSS失能 */
//	GD25Q_FLASH_SPIx_NSS_DISABLE();	
//	identificationID = (Temp0 << 16) | (Temp1 << 8) | Temp2;		
//	return identificationID;
//}

///*****************************************************************
//* Function Name : GD25Q_SPIFLASH_WriteEnable
//* Description   : GD25Q SPIFLASH写使能
//* Input         : None
//* Output        : None
//* Notes         :
//******************************************************************/
//void GD25Q_SPIFLASH_WriteEnable(void)
//{
//	/* 选择SPI FLASH, NSS使能 */
//	GD25Q_FLASH_SPIx_NSS_ENABLE();	
//	GD25Q_SPI_FLASH_SendByte(GD25Q_WriteEnable);	
//	/* 禁用SPI FLASH, NSS失能 */
//	GD25Q_FLASH_SPIx_NSS_DISABLE();
//}

///*****************************************************************
//* Function Name : GD25Q_SPIFLASH_WriteDisable
//* Description   : GD25Q SPIFLASH写禁能
//* Input         : None
//* Output        : None
//* Notes         :
//******************************************************************/
//void GD25Q_SPIFLASH_WriteDisable(void)
//{
//	/* 选择SPI FLASH, NSS使能 */
//	GD25Q_FLASH_SPIx_NSS_ENABLE();	
//	GD25Q_SPI_FLASH_SendByte(GD25Q_WriteDisable);	
//	/* 禁用SPI FLASH, NSS失能 */
//	GD25Q_FLASH_SPIx_NSS_DISABLE();
//}

///*****************************************************************
//* Function Name : GD25Q_SPIFLASH_ReadStatusRegister
//* Description   : GD25Q SPIFLASH 读状态寄存器
//* Input         : 状态寄存器地址
//* Output        : 状态寄存器值
//* Notes         :
//******************************************************************/
//u8 GD25Q_SPIFLASH_ReadStatusRegister(u8 statusRegister)
//{
//	u8 readByte = 0;
//	
//	GD25Q_FLASH_SPIx_NSS_ENABLE();	
//	GD25Q_SPI_FLASH_SendByte(statusRegister);
//	readByte = GD25Q_SPI_FLASH_SendByte(GD25Q_Dummy_Byte);
//	GD25Q_FLASH_SPIx_NSS_DISABLE();
//	return readByte;
//}

///*****************************************************************
//* Function Name : GD25Q_SPIFLASH_WaitForBusy
//* Description   : 等待设备空闲
//* Input         : None
//* Output        : None
//* Notes         :
//******************************************************************/
//void GD25Q_SPIFLASH_WaitForBusy(void)
//{
//	u8 flashStatus = 0;
//	
//	GD25Q_FLASH_SPIx_NSS_ENABLE();	
//	
//	GD25Q_SPI_FLASH_SendByte(GD25Q_ReadStatusReg1);		
//	do 
//	{
//		#ifdef USE_SOFTWARE_SPI 	
//		flashStatus = GD25Q_SPI_FLASH_ReadByte();	
//        #else
//        flashStatus = GD25Q_SPI_FLASH_SendByte(GD25Q_Dummy_Byte);	
//        #endif		
//	}
//	while ((flashStatus & 0x01) == 0x01);
//	/* 禁用SPI FLASH, NSS失能 */
//	GD25Q_FLASH_SPIx_NSS_DISABLE();
//}

///*****************************************************************
//* Function Name : GD25Q_SPIFLASH_PowerDown
//* Description   : 进入掉电模式
//* Input         : None
//* Output        : None
//* Notes         :
//******************************************************************/
//void GD25Q_SPIFLASH_PowerDown(void)
//{
//	if (GD25Q80CSIG_OK != GD25QStatus) 
//	{
//		return;
//	}

//	GD25Q_FLASH_SPIx_NSS_ENABLE();
//	/* Send "Power Down" instruction */
//	GD25Q_SPI_FLASH_SendByte(GD25Q_PowerDown);
//	/* Deselect the FLASH: Chip Select high */
//	GD25Q_FLASH_SPIx_NSS_DISABLE();
//	/* Wait for TDP */
//	delay_us(3);
//}

///*****************************************************************
//* Function Name : GD25Q_SPIFLASH_WakeUp
//* Description   : GD25Q SPIFLASH 唤醒
//* Input         : None
//* Output        : None
//* Notes         :
//******************************************************************/
//void GD25Q_SPIFLASH_WakeUp(void)
//{	
//	/* Select the FLASH: Chip Select low */
//	GD25Q_FLASH_SPIx_NSS_ENABLE();
//	/* Send "Release Power Down" instruction */
//	GD25Q_SPI_FLASH_SendByte(GD25Q_ReleasePowerDown);	
//	/* Deselect the FLASH: Chip Select high */
//	GD25Q_FLASH_SPIx_NSS_DISABLE();	
//	/* Wait for TRES1 */
//	delay_us(5);
//}

///*****************************************************************
//* Function Name : GD25Q_SPIFLASH_EraseSector
//* Description   : GD25Q SPIFLASH 擦除Sector
//* Input         : SectorAddr: 扇区地址(0x1000倍数)
//* Output        : None
//* Notes         : GD25Qxx SPI FLASH 最小擦除大小为4KB(4096Byte),即
//				: 一个扇区,在写入数据之前要求先擦除空间
//******************************************************************/
//void GD25Q_SPIFLASH_EraseSector(u32 SectorAddr)
//{
//	if (GD25Q80CSIG_OK != GD25QStatus) 
//	{
//		return;
//	}	

//	/* 发送FLASH写使能命令 */
//    SectorAddr *= 0x1000;
//	GD25Q_SPIFLASH_WriteEnable();	

//	/* 等待FLASH空闲 */
//	GD25Q_SPIFLASH_WaitForBusy();	
//	/* Select the FLASH: Chip Select low */
//	GD25Q_FLASH_SPIx_NSS_ENABLE();	
//	/* 发送扇区擦除命令 */
//	GD25Q_SPI_FLASH_SendByte(GD25Q_SectorErase);//0x20	
//	/* 发送扇区擦除地址高位 */
//	GD25Q_SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
////	GD25Q_SPI_FLASH_SendByte(SectorAddr & 0xFF);
//	/* 发送扇区擦除地址中位 */
//	GD25Q_SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);	
//	/* 发送扇区擦除地址低位 */
////	GD25Q_SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
//	GD25Q_SPI_FLASH_SendByte(SectorAddr & 0xFF);
//	/* Deselect the FLASH: Chip Select high */
//	GD25Q_FLASH_SPIx_NSS_DISABLE();	
//	/* 等待擦除完毕 */
//	GD25Q_SPIFLASH_WaitForBusy();
//}

///*****************************************************************
//* Function Name : GD25Q_SPIFLASH_ReadBuffer
//* Description   : GD25Q SPIFLASH 读取SPI FLASH数据
//* Input         : pBuffer: 数据存储区  ReadAddr: 数据读取地址
//                :NumByteToRead: 读取数据长度
//* Output        : None
//* Notes         : 
//******************************************************************/
//void GD25Q_SPIFLASH_ReadBuffer(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
//{

//	if (GD25Q80CSIG_OK != GD25QStatus) 
//	{
//		return;
//	}	
//	/* Select the FLASH: Chip Select low */
//	GD25Q_FLASH_SPIx_NSS_ENABLE();	
//	/* 发送读指令 */
//	GD25Q_SPI_FLASH_SendByte(GD25Q_ReadData);
//	/* 发送读地址高位 */
//	GD25Q_SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
//	/* 发送读地址中位 */
//	GD25Q_SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);	
//	/* 发送读地址低位 */
//	GD25Q_SPI_FLASH_SendByte(ReadAddr & 0xFF);	
//	/* 读取数据 */
//	while(NumByteToRead--) 
//	{
//		#ifdef USE_SOFTWARE_SPI 	
//		*pBuffer = GD25Q_SPI_FLASH_ReadByte();	
//        #else
//		*pBuffer = GD25Q_SPI_FLASH_SendByte(GD25Q_Dummy_Byte);
//        #endif   		
//		pBuffer++;
//	}
//	/* Deselect the FLASH: Chip Select high */
//	GD25Q_FLASH_SPIx_NSS_DISABLE();
//}

///*****************************************************************
//* Function Name : GD25Q_SPIFLASH_WritePage
//* Description   : GD25Q SPIFLASH 写入SPI FLASH数据Page
//* Input         : pBuffer: 数据存储区  ReadAddr: 数据写入地址
//                :NumByteToRead: 写入数据长度
//* Output        : None
//* Notes         : 写入字节数不得超过该页写入剩余字节数
//******************************************************************/
//void GD25Q_SPIFLASH_WritePage(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
//{
//	if (GD25Q80CSIG_OK != GD25QStatus)
//	{
//		return;
//	}

//	/* 发送FLASH写使能命令 */
//	GD25Q_SPIFLASH_WriteEnable();	
//	/* 等待FLASH空闲 */
//	GD25Q_SPIFLASH_WaitForBusy();	
//	/* Select the FLASH: Chip Select low */
//	GD25Q_FLASH_SPIx_NSS_ENABLE();	
//	/* 发送页写命令 */
//	GD25Q_SPI_FLASH_SendByte(GD25Q_PageProgram);	
//	/* 发送写地址高位 */
//	GD25Q_SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
//	/* 发送写地址中位 */
//	GD25Q_SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);	
//	/* 发送写地址低位 */
//	GD25Q_SPI_FLASH_SendByte(WriteAddr & 0xFF);	
//	/* 写入数据 */
//	while (NumByteToWrite--) 
//	{
//		GD25Q_SPI_FLASH_SendByte(*pBuffer);
//		pBuffer++;
//	}
//	/* Deselect the FLASH: Chip Select high */
//	GD25Q_FLASH_SPIx_NSS_DISABLE();
//	/* 等待写入完毕 */
//	GD25Q_SPIFLASH_WaitForBusy();
//}

///*****************************************************************
//* Function Name : GD25Q_SPIFLASH_GetByte
//* Description   : GD25Q SPIFLASH 读取1个Byte
//* Input         : ReadAddr: 数据写入地址
//* Output        : 读出的数据
//* Notes         : 
//******************************************************************/
//unsigned char GD25Q_SPIFLASH_GetByte(u32 ReadAddr)
//{
//	unsigned char val = 0;	
//	GD25Q_SPIFLASH_ReadBuffer(&val, ReadAddr, 1);	
//	return val;
//} 

////====================================================================================//

////==================================== The End===============================================/


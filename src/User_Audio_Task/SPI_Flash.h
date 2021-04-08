#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H
//===============================================
#define AUDIO_SECTOR_SIZE     	50 //* 4K 

#define SECTORNUM(n)       		    n
#define FLASH_START_ADDRESS(AudioSectorNum)       	GD25Q32_SECTOR_ADDRESS(AudioSectorNum*50)	
//ÿһ֡���ݸ�����256���ֽ�Ϊһҳ
#define MAX_IAP_SPI_FLASH  (200*1024) //200K
#define FRAME_BYTES           256
#define FRAME_BYTES_MAX_NUM   16  //(4096/FRAME_BYTES)    //16    //4*1024/ FRAME_BYTES
#define FRAME_START_ADDRESS(AudioSectorNum,FrameNum)    GD25Q32_SECTOR_ADDRESS(AudioSectorNum*50) + FrameNum*FRAME_BYTES	
#define AUDIO_BEEP         		0
#define AUDIO_10876        		1
//===============================================
typedef struct
{	
	u32 FileSize;
	u16 AudioNum;
	u8  FileHeadisOK;
	u8  DownloadBusy;
	u32 DownloadIndex;
	u8  DownloadFinishFlag;
	u8  Buffer1[256];
	u8  Buffer2[256];
	u16 Buffer1_LoadLength; //ÿһ֡�����ݣ����һ֡���ܲ�Ϊ256�ֽ�
	u16 Buffer2_LoadLength;
	u8  Buffer1_LoadFinishFlag;
	u8  Buffer2_LoadFinishFlag;
	
	u32 Buffer_Write_Index;	
	u8  Buffer_WriteFinishFlag;
	u8  Buffer_WritingFlag;
	u8  Buffer_WritingNum;
	
}SPI_FLASH_IAP; 

extern SPI_FLASH_IAP   SpiFlash_IAP; 


void SPI_Write_Flash_Test(void);	
void IAP_ParseFrame_SPI_FLASH(u8* pData, u8 channel);

#endif


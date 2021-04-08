
#include "MIFARE.h"

unsigned char NFC_SECTOR,NFC_BLOCK,NFC_BLOCK_NUM;
unsigned char BLOCK_DATA[16]= {0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x07,0x80,0x69,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char BLOCK_DATA_R[16];
unsigned char BLOCK_DATA_W[16]= {0x01,0x02,0x03,0x04,0x05,0x06,0xFF,0x07,0x80,0x69,0x11,0x12,0x13,0x14,0x15,0x16};
unsigned char DATA_W_RAND[16];
unsigned char KEY_A[16][6]=
{
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//0
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//1
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//2
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//3
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//4
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//5
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//6
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//7
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//8
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//9
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//10
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//11
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//12
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//13
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//14
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}
};//15

unsigned char KEY_B[16][6]=
{
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//0
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//1
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//2
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//3
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//4
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//5
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//6
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//7
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//8
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//9
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//10
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//11
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//12
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//13
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//14
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}
};//15

/*****************************************************************************************/
/*名称：Mifare_Clear_Crypto																															 */
/*功能：Mifare_Clear_Crypto清除认证标志																 									*/
/*输入：																																								 */
/*																																						 					*/
/*输出:																																									 */
/*																																											 */
/*																																											 */
/*****************************************************************************************/
void Mifare_Clear_Crypto(void)
{
    ModifyReg(JREG_STATUS2,BIT3,RESET);
    return;
}
/*****************************************************************************************/
/*名称：Mifare_Auth																		 */
/*功能：Mifare_Auth卡片认证																 */
/*输入：mode，认证模式（0：key A认证，1：key B认证）；sector，认证的扇区号（0~15）		 */
/*		*mifare_key，6字节认证密钥数组；*card_uid，4字节卡片UID数组						 */
/*输出:																					 */
/*		OK    :认证成功																	 */
/*		ERROR :认证失败																	 */
/*****************************************************************************************/
unsigned char Mifare_Auth(unsigned char mode,unsigned char sector,unsigned char *mifare_key,unsigned char *card_uid)
{
    uchar result,reg_data;
    uchar outbuf[12],inbuf[1];
    command_struct command_status;
    command_status.pSendBuf = outbuf;
    command_status.pReceiveBuf = inbuf;
    command_status.SendCRCEnable = SET;
    command_status.ReceiveCRCEnable = SET;
    if(mode == KEY_A_AUTH)
        command_status.pSendBuf[0] = 0x60;//60 kayA认证指令
    if(mode == KEY_B_AUTH)
        command_status.pSendBuf[0] = 0x61;//61 keyB认证指令
    command_status.pSendBuf[1] = sector * 4;//认证扇区的块0地址
    command_status.pSendBuf[2] = mifare_key[0];
    command_status.pSendBuf[3] = mifare_key[1];
    command_status.pSendBuf[4] = mifare_key[2];
    command_status.pSendBuf[5] = mifare_key[3];
    command_status.pSendBuf[6] = mifare_key[4];
    command_status.pSendBuf[7] = mifare_key[5];
    command_status.pSendBuf[8] = card_uid[0];
    command_status.pSendBuf[9] = card_uid[1];
    command_status.pSendBuf[10] = card_uid[2];
    command_status.pSendBuf[11] = card_uid[3];

    command_status.Timeout = 10;//10ms
    command_status.nBytesToSend = 12;
    command_status.nBitsToSend = 0;
    command_status.nBitsToReceive = 0;
    command_status.nBytesToReceive = 0;
    command_status.Cmd = CMD_AUTHENT;
    result = Command_Execute(&command_status);

    if (result == FM175XX_SUCCESS)
    {
        GetReg(JREG_STATUS2,&reg_data);
        if(reg_data & 0x08)//判断加密标志位，确认认证结果
            return FM175XX_SUCCESS;
        else
            return FM175XX_AUTH_ERR;
    }
    return FM175XX_AUTH_ERR;
}
/*****************************************************************************************/
/*名称：Mifare_Blockset									 */
/*功能：Mifare_Blockset卡片数值设置							 */
/*输入：block，块号；*buff，需要设置的4字节数值数组					 */
/*											 */
/*输出:											 */
/*		OK    :设置成功								 */
/*		ERROR :设置失败								 */
/*****************************************************************************************/
unsigned char Mifare_Blockset(unsigned char block,unsigned char *data_buff)
{
    unsigned char block_data[16],result;
    block_data[0] = data_buff[3];
    block_data[1] = data_buff[2];
    block_data[2] = data_buff[1];
    block_data[3] = data_buff[0];
    block_data[4] = ~data_buff[3];
    block_data[5] = ~data_buff[2];
    block_data[6] = ~data_buff[1];
    block_data[7] = ~data_buff[0];
    block_data[8] = data_buff[3];
    block_data[9] = data_buff[2];
    block_data[10] = data_buff[1];
    block_data[11] = data_buff[0];
    block_data[12] = block;
    block_data[13] = ~block;
    block_data[14] = block;
    block_data[15] = ~block;
    result = Mifare_Blockwrite(block,block_data);
    return result;
}
/*****************************************************************************************/
/*名称：Mifare_Blockread																 */
/*功能：Mifare_Blockread卡片读块操作													 */
/*输入：block，块号（0x00~0x3F）；buff，16字节读块数据数组								 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockread(unsigned char block,unsigned char *data_buff)
{
    uchar result;
    uchar outbuf[2],inbuf[16];
    command_struct command_status;
    command_status.pSendBuf = outbuf;
    command_status.pReceiveBuf = inbuf;
    command_status.SendCRCEnable = SET;
    command_status.ReceiveCRCEnable = SET;
    command_status.pSendBuf[0] = 0x30;//30 读块指令
    command_status.pSendBuf[1] = block;//块地址
    command_status.nBytesToSend = 2;
    command_status.nBitsToSend = 0;
    command_status.nBitsToReceive = 0;
    command_status.nBytesToReceive = 16;
    command_status.Timeout = 10;//10ms
    command_status.Cmd = CMD_TRANSCEIVE;
    result = Command_Execute(&command_status);
    if ((result != FM175XX_SUCCESS )||(command_status.nBytesReceived != 16)) //接收到的数据长度为16
        return FM175XX_COMM_ERR;
    memcpy(data_buff,command_status.pReceiveBuf,16);
    return FM175XX_SUCCESS;
}
/*****************************************************************************************/
/*名称：mifare_blockwrite																 */
/*功能：Mifare卡片写块操作																 */
/*输入：block，块号（0x00~0x3F）；buff，16字节写块数据数组								 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockwrite(unsigned char block,unsigned char *data_buff)
{

    uchar outbuf[16],inbuf[1];
    command_struct command_status;
    command_status.pSendBuf = outbuf;
    command_status.pReceiveBuf = inbuf;
    command_status.SendCRCEnable = SET;
    command_status.ReceiveCRCEnable = RESET;
    command_status.pSendBuf[0] = 0xA0;//A0 写块指令
    command_status.pSendBuf[1] = block;//块地址

    command_status.nBytesToSend = 2;
    command_status.nBitsToSend = 0;
    command_status.nBitsToReceive = 0;
    command_status.nBytesToReceive = 1;
    command_status.Timeout = 10;//10ms
    command_status.Cmd = CMD_TRANSCEIVE;
    Command_Execute(&command_status);
    if ((command_status.nBitsReceived != 4)||((command_status.pReceiveBuf[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
        return FM175XX_COMM_ERR;

    SetTimeOut(10);//10ms
    memcpy(command_status.pSendBuf,data_buff,16);
    command_status.nBytesToSend = 16;
    command_status.nBytesToReceive = 1;
    command_status.Cmd = CMD_TRANSCEIVE;
    Command_Execute(&command_status);
    if ((command_status.nBitsReceived != 4)||((command_status.pReceiveBuf[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
        return FM175XX_COMM_ERR;
    return FM175XX_SUCCESS;
}

/*****************************************************************************************/
/*名称：																				 */
/*功能：Mifare 卡片增值操作																 */
/*输入：block，块号（0x00~0x3F）；buff，4字节增值数据数组								 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockinc(unsigned char block,unsigned char *data_buff)
{

    uchar outbuf[4],inbuf[1];
    command_struct command_status;
    command_status.pSendBuf = outbuf;
    command_status.pReceiveBuf = inbuf;
    command_status.SendCRCEnable = SET;
    command_status.ReceiveCRCEnable = RESET;
    command_status.pSendBuf[0] = 0xC1;// C1 增值指令
    command_status.pSendBuf[1] = block;//块地址
    command_status.Timeout = 10;//10ms
    command_status.nBytesToSend = 2;
    command_status.nBitsToSend = 0;
    command_status.nBitsToReceive = 0;
    command_status.nBytesToReceive = 1;
    command_status.Cmd = CMD_TRANSCEIVE;
    Command_Execute(&command_status);
    if ((command_status.nBitsReceived != 4)||((command_status.pReceiveBuf[0] & 0x0F) != 0x0A))	//如果未接收到0x0A，表示无ACK
        return FM175XX_COMM_ERR;

    command_status.Timeout = 10;//10ms
    memcpy(command_status.pSendBuf,data_buff,4);
    command_status.nBytesToSend = 4;
    command_status.nBytesToReceive = 0;
    command_status.Cmd = CMD_TRANSCEIVE;
    Command_Execute(&command_status);

    return FM175XX_SUCCESS;
}
/*****************************************************************************************/
/*名称：mifare_blockdec																	 */
/*功能：Mifare 卡片减值操作																 */
/*输入：block，块号（0x00~0x3F）；buff，4字节减值数据数组								 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockdec(unsigned char block,unsigned char *data_buff)
{

    uchar outbuf[4],inbuf[1];
    command_struct command_status;
    command_status.pSendBuf = outbuf;
    command_status.pReceiveBuf = inbuf;
    command_status.SendCRCEnable = SET;
    command_status.ReceiveCRCEnable = RESET;
    command_status.pSendBuf[0] = 0xC0;// C0 减值指令
    command_status.pSendBuf[1] = block;//块地址
    command_status.Timeout = 10;//10ms
    command_status.nBytesToSend = 2;
    command_status.nBitsToSend = 0;
    command_status.nBitsToReceive = 0;
    command_status.nBytesToReceive = 1;
    command_status.Cmd = CMD_TRANSCEIVE;
    Command_Execute(&command_status);
    if ((command_status.nBitsReceived != 4)||((command_status.pReceiveBuf[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
        return FM175XX_COMM_ERR;

    command_status.Timeout = 10;//10ms
    memcpy(command_status.pSendBuf,data_buff,4);
    command_status.nBytesToSend = 4;
    command_status.Cmd = CMD_TRANSCEIVE;
    Command_Execute(&command_status);

    return FM175XX_SUCCESS;
}
/*****************************************************************************************/
/*名称：mifare_transfer																	 */
/*功能：Mifare 卡片transfer操作															 */
/*输入：block，块号（0x00~0x3F）														 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Transfer(unsigned char block)
{

    uchar outbuf[2],inbuf[1];
    command_struct command_status;
    command_status.pSendBuf = outbuf;
    command_status.pReceiveBuf = inbuf;
    command_status.SendCRCEnable = SET;
    command_status.ReceiveCRCEnable = RESET;
    command_status.pSendBuf[0] = 0xB0;// B0 Transfer指令
    command_status.pSendBuf[1] = block;//块地址
    command_status.Timeout = 10;//10ms
    command_status.nBytesToSend = 2;
    command_status.nBitsToSend = 0;
    command_status.nBitsToReceive = 0;
    command_status.nBytesToReceive = 1;
    command_status.Cmd = CMD_TRANSCEIVE;
    Command_Execute(&command_status);
    if ((command_status.nBitsReceived != 4)||((command_status.pReceiveBuf[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
        return FM175XX_COMM_ERR;
    return FM175XX_SUCCESS;
}
/*****************************************************************************************/
/*名称：mifare_restore																	 */
/*功能：Mifare 卡片restore操作															 */
/*输入：block，块号（0x00~0x3F）														 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/

unsigned char Mifare_Restore(unsigned char block)
{

    uchar outbuf[4],inbuf[1];
    command_struct command_status;
    command_status.pSendBuf = outbuf;
    command_status.pReceiveBuf = inbuf;
    command_status.SendCRCEnable = SET;
    command_status.ReceiveCRCEnable = RESET;
    command_status.pSendBuf[0] = 0xC2;// C2 Restore指令
    command_status.pSendBuf[1] = block;//块地址

    command_status.nBytesToSend = 2;
    command_status.nBitsToSend = 0;
    command_status.nBitsToReceive = 0;
    command_status.nBytesToReceive = 1;
    command_status.Timeout = 10;//10ms
    command_status.Cmd = CMD_TRANSCEIVE;
    Command_Execute(&command_status);
    if ((command_status.nBitsReceived != 4)||((command_status.pReceiveBuf[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
        return FM175XX_COMM_ERR;

    command_status.Timeout = 10;//10ms
    memcpy(command_status.pSendBuf,"\x00\x00\x00\x00",4);
    command_status.nBytesToSend = 4;
    command_status.nBytesToReceive = 1;
    command_status.Cmd = CMD_TRANSCEIVE;
    Command_Execute(&command_status);
    if ((command_status.nBitsReceived != 4)||((command_status.pReceiveBuf[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
        return FM175XX_COMM_ERR;
    return FM175XX_SUCCESS;
}

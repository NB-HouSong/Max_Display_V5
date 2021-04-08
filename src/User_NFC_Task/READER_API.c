/******************** (C) COPYRIGHT 2020 Ninebot *******************************
* File Name     : READER_API.c
* Author        : 
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "READER_API.h"

struct picc_a_struct PICC_A;
struct picc_b_struct PICC_B;
struct lpcd_struct Lpcd;
u8 succeed;
extern u8 VALUE;

volatile u8 NFC_INT = 0;
NFC_SET NFC_set;
NFC_CARD_STRUCT  _my_id =
{
    .s_upload_timeout = 0,
    .s_card_id_len = 4,
    .s_send_cmd_id = 0x66,
    .s_card_id = 0,
};

void SetSendCRC(unsigned char mode)
{
    if(mode)
        ModifyReg(JREG_TXMODE,BIT7,SET);
    else
        ModifyReg(JREG_TXMODE,BIT7,RESET);
    return;
}

void SetReceiveCRC(unsigned char mode)
{
    if(mode)
        ModifyReg(JREG_RXMODE,BIT7,SET);
    else
        ModifyReg(JREG_RXMODE,BIT7,RESET);
    return;
}

unsigned char SetTimeOut(unsigned int microseconds)
{
    unsigned long  timereload;
    unsigned int  tprescaler;

    if(microseconds == 0)
        microseconds = 1;//定时时间不能为0

    tprescaler = 0;
    timereload = 0;
    while(tprescaler < 0xfff)
    {
        timereload = ((microseconds*(long)13560) - 1)/((tprescaler * 2) + 1);

        if( timereload < 0xffff)
            break;
        tprescaler++;
    }
    timereload = timereload & 0xFFFF;

    SetReg(JREG_TMODE,((tprescaler >> 8) & 0x0F) | BIT7);//TAuto = 1 , TPreScale_Hi
    SetReg(JREG_TPRESCALER,tprescaler & 0xFF);	//TPreScale_Lo
    SetReg(JREG_TRELOADHI,timereload >> 8);
    SetReg(JREG_TRELOADLO,timereload & 0xFF);
    return FM175XX_SUCCESS;
}
/******************************************************************************/
//函数名：Command_Execute
//入口参数：
//			uchar* sendbuf:发送数据缓冲区	uchar sendlen:发送数据长度
//			uchar* recvbuf:接收数据缓冲区	uchar* recvlen:接收到的数据长度
//出口参数
/******************************************************************************/
unsigned char Command_Execute(command_struct *comm_status)
{
    unsigned char reg_data,send_length,receive_length,send_finish;
    unsigned char irq;
    unsigned char result;
    send_length = comm_status->nBytesToSend;//Length of bytes to be sent
    receive_length = 0;
    send_finish = 0;//clear flag
    comm_status->nBitsReceived = 0;
    comm_status->nBytesReceived = 0;
    comm_status->CollPos = 0;
    comm_status->Error = 0;

    SetReg(JREG_COMMAND,CMD_IDLE);//command = Idel
    Clear_FIFO();//Clear FIFO
    SetReg(JREG_WATERLEVEL,0x20);//waterlevel = 32bytes
    SetReg(JREG_DIVIEN,0x00);//Disable DIVIRQ to be propagated to IRQ pin
    SetReg(JREG_COMMIEN,0x80);//Disable COMMIRQ to be propagated to IRQ pin
    SetReg(JREG_COMMIRQ,0x7F);//Clear COMMIRQ

    SetSendCRC(comm_status->SendCRCEnable);//Config TxCRC
    SetReceiveCRC(comm_status->ReceiveCRCEnable);//Config RxCRC
    SetTimeOut(comm_status->Timeout);//Config Timer
    if(comm_status->Cmd == CMD_TRANSCEIVE)
    {
        SetReg(JREG_BITFRAMING,(comm_status->nBitsToReceive << 4) | comm_status->nBitsToSend );//Config BITFRAMING
        SetReg(JREG_COMMAND,comm_status->Cmd);
    }

    if((comm_status->Cmd == CMD_AUTHENT)||(comm_status->Cmd == CMD_TRANSMIT))
    {
        SPI_Write_FIFO(send_length,comm_status->pSendBuf);//WRITE FIFO
        send_length = 0;//Length of bytes to be sent
        SetReg(JREG_BITFRAMING,BIT7 | comm_status->nBitsToSend);//Start Send
        SetReg(JREG_COMMAND,comm_status->Cmd);
    }

    while(1)
    {
        GetReg(JREG_COMMIRQ,&irq);
        if(irq & BIT0)//TimerIRq = 1
        {
            SetReg(JREG_COMMIRQ,BIT0);//Clear COMMIRQ BIT0
            result = FM175XX_TIMER_ERR;
            break;
        }

        if(irq & BIT1)//ERRIRq = 1
        {
            GetReg(JREG_ERROR,&reg_data);
            comm_status->Error = reg_data;

            if(comm_status->Error & BIT3)
            {
                GetReg(JREG_COLL,&reg_data);
                comm_status->CollPos = reg_data & 0x1F;//读取冲突位置
                result = FM175XX_COLL_ERR;
                break;
            }

            result = FM175XX_COMM_ERR;
            SetReg(JREG_COMMIRQ,BIT1);//Clear COMMIRQ BIT1
            break;
        }

        if(irq & BIT2)//LoAlterIRq = 1
        {
            if(send_length > 0)
            {

                if(send_length > 32)
                {
                    SPI_Write_FIFO(32,comm_status->pSendBuf);//Write 32 bytes to FIFO
                    comm_status->pSendBuf = comm_status->pSendBuf + 32;
                    send_length = send_length - 32;
                }
                else
                {
                    SPI_Write_FIFO(send_length,comm_status->pSendBuf);
                    send_length = 0;
                }
                ModifyReg(JREG_BITFRAMING,BIT7,SET);
							
            }
            SetReg(JREG_COMMIRQ,BIT2);//Clear COMMIRQ BIT2
							
        }

        if(irq & BIT3)//HiAlterIRq = 1
        {
            if(send_finish == 1)//确认发送完成
            {
                SPI_Read_FIFO(32,comm_status->pReceiveBuf + comm_status->nBytesReceived);
                comm_status->nBytesReceived = comm_status->nBytesReceived + 32;
            }
            SetReg(JREG_COMMIRQ,BIT3);//Clear COMMIRQ BIT3
        }

        if((irq & BIT4)&&(comm_status->Cmd == CMD_AUTHENT))//IdleTRq = 1
        {
            SetReg(JREG_COMMIRQ,BIT4);//Clear COMMIRQ BIT4
            result = FM175XX_SUCCESS;
            break;
        }

        if((irq & BIT5)&&((comm_status->Cmd == CMD_TRANSCEIVE)||(comm_status->Cmd == CMD_RECEIVE)))//RxIRq = 1
        {

            GetReg(JREG_CONTROL,&reg_data);
            comm_status->nBitsReceived = reg_data & 0x07;//Get length of bits received
            GetReg(JREG_FIFOLEVEL,&reg_data);//Get length of bytes received
            receive_length = reg_data & 0x7F;//Get length of bytes received
            SPI_Read_FIFO(receive_length,comm_status->pReceiveBuf + comm_status->nBytesReceived);//Read FIFO
            comm_status->nBytesReceived = comm_status->nBytesReceived + receive_length;//update length of bytes received
            if((comm_status->nBytesToReceive != comm_status->nBytesReceived)&&(comm_status->nBytesToReceive != 0))
            {
                result = FM175XX_COMM_ERR;//
                break;
            }
            SetReg(JREG_COMMIRQ,BIT5);//Clear COMMIRQ BIT5
            result = FM175XX_SUCCESS;
            break;
        }

        if(irq & BIT6)//TxIRq = 1
        {
            SetReg(JREG_COMMIRQ,BIT6);//Clear COMMIRQ BIT6
            if(comm_status->Cmd == CMD_TRANSMIT)
            {
                result = FM175XX_SUCCESS;
                break;
            }
            if(comm_status->Cmd == CMD_TRANSCEIVE)
                send_finish = 1;//发送完成
        }

    }

    ModifyReg(JREG_BITFRAMING,BIT7,RESET);//CLR Start Send
    SetReg(JREG_COMMAND,CMD_IDLE);
    return result;
}


void FM175XX_Initial_ReaderA(void)
{
    SetReg(JREG_TXMODE,0x00);//TxCRCEnable = 0; TxSpeed = 106kbps; InvMode = 0; TxMix = 0; TxFraming = ISO14443A
    SetReg(JREG_RXMODE,BIT3);	//RxCRCEnable = 0; RxSpeed = 106kbps;RxNoError = 1; RxMultiple = 0; RxFraming = ISO14443A
    SetReg(JREG_MODWIDTH,MODWIDTH_106);	//MODWIDTH = 106kbps
    ModifyReg(JREG_TXAUTO,BIT6,SET);//Force 100ASK = 1
    SetReg(JREG_GSN,(GSNON_A<<4));//Config GSN; Config ModGSN
    SetReg(JREG_CWGSP,GSP_A);//Config GSP
    SetReg(JREG_CONTROL,BIT4);//Initiator = 1
    SetReg(JREG_RFCFG,RXGAIN_A<<4);//Config RxGain
    SetReg(JREG_RXTRESHOLD,(MINLEVEL_A<<4) | COLLLEVEL_A);//Config MinLevel; Config CollLevel
    return;
}


void FM175XX_Initial_ReaderB(void)
{
    ModifyReg(JREG_STATUS2,BIT3,RESET);
    SetReg(JREG_TXMODE,BIT7|BIT1|BIT0);//TxCRCEnable = 1; TxSpeed = 106kbps; InvMode = 0; TxMix = 0; TxFraming = ISO14443B
    SetReg(JREG_RXMODE,BIT7|BIT3|BIT1|BIT0);//RxCRCEnable = 1; RxSpeed = 106kbps;RxNoError = 1; RxMultiple = 0; RxFraming = ISO14443B
    SetReg(JREG_MODWIDTH,MODWIDTH_106);//MODWIDTH = 106kbps
    SetReg(JREG_TXAUTO,0);//Force 100ASK = 0
    SetReg(JREG_GSN,(GSNON_B<<4)|MODGSNON_B);//Config GSN; Config ModGSN
    SetReg(JREG_CWGSP,GSP_B);//Config GSP
    SetReg(JREG_MODGSP,MODGSP_B);//Config ModGSP
    SetReg(JREG_CONTROL,BIT4);//Initiator = 1
    SetReg(JREG_RFCFG,RXGAIN_B<<4);//Config RxGain
    SetReg(JREG_RXTRESHOLD,MINLEVEL_B<<4);//Config MinLevel;
    return;
}

unsigned char ReaderA_Halt(void)
{
    unsigned char result;
    unsigned char outbuf[2],inbuf[1];
    command_struct command_status;
    command_status.pSendBuf = outbuf;
    command_status.pReceiveBuf = inbuf;
    command_status.ReceiveCRCEnable = SET;
    command_status.SendCRCEnable = SET;
    command_status.pSendBuf[0] = 0x50;
    command_status.pSendBuf[1] = 0x00;
    command_status.nBytesToSend = 2;
    command_status.nBitsToSend = 0;
    command_status.nBitsToReceive = 0;
    command_status.nBytesToReceive = 0;
    command_status.Timeout = 1;//1ms
    command_status.Cmd = CMD_TRANSMIT;
    result = Command_Execute(&command_status);
    return result;
}

unsigned char ReaderA_Wakeup(void)
{
    unsigned char result;
    unsigned char outbuf[1],inbuf[2];
    command_struct command_status;
    command_status.pSendBuf = outbuf;
    command_status.pReceiveBuf = inbuf;
    command_status.ReceiveCRCEnable = RESET;
    command_status.SendCRCEnable = RESET;
    command_status.pSendBuf[0] = RF_CMD_WUPA;   //
    command_status.nBytesToSend = 1;//
    command_status.nBitsToSend = 7;
    command_status.nBitsToReceive = 0;
    command_status.nBytesToReceive = 2;
    command_status.Timeout = 2;//1ms
    command_status.Cmd = CMD_TRANSCEIVE;

    result = Command_Execute(&command_status);

    if((result == FM175XX_SUCCESS)&&(command_status.nBytesReceived == 2))
    {
        memcpy(PICC_A.ATQA,command_status.pReceiveBuf,2);
    }
    else
        result =  FM175XX_COMM_ERR;
    return result;
}

unsigned char ReaderA_Request(void)
{
    unsigned char result;
    unsigned char outbuf[1],inbuf[2];
    command_struct command_status;
    command_status.pSendBuf = outbuf;
    command_status.pReceiveBuf = inbuf;
    command_status.ReceiveCRCEnable = RESET;
    command_status.SendCRCEnable = RESET;
    command_status.pSendBuf[0] = RF_CMD_REQA;   //
    command_status.nBytesToSend = 1;//
    command_status.nBitsToSend = 7;
    command_status.nBitsToReceive = 0;
    command_status.nBytesToReceive = 2;
    command_status.Timeout = 1;//1ms
    command_status.Cmd = CMD_TRANSCEIVE;
    result = Command_Execute(&command_status);
    if((result == FM175XX_SUCCESS)&&(command_status.nBytesReceived == 2))
    {
        memcpy(PICC_A.ATQA, command_status.pReceiveBuf,2);
    }
    else
        result =  FM175XX_COMM_ERR;
    return result;
}

//*************************************
//函数  名：ReaderA_AntiColl
//入口参数：size:防冲突等级，包括0、1、2
//出口参数：uchar:0:OK  others：错误
//*************************************
unsigned char ReaderA_AntiColl(unsigned char cascade_level)
{
    unsigned char result;
    unsigned char outbuf[2],inbuf[5];
    command_struct command_status;
    command_status.pSendBuf = outbuf;
    command_status.pReceiveBuf = inbuf;
    command_status.ReceiveCRCEnable = RESET;
    command_status.SendCRCEnable = RESET;
    if(cascade_level > 2)
        return FM175XX_PARAM_ERR;
    command_status.pSendBuf[0] = RF_CMD_ANTICOL[cascade_level];   //防冲突等级
    command_status.pSendBuf[1] = 0x20;
    command_status.nBytesToSend = 2;						//发送长度：2
    command_status.nBitsToSend = 0;
    command_status.nBitsToReceive = 0;
    command_status.nBytesToReceive = 5;
    command_status.Timeout = 2;//1ms
    command_status.Cmd = CMD_TRANSCEIVE;
    result = Command_Execute(&command_status);
    ModifyReg(JREG_COLL,BIT7,SET);
//		PICC_A.Have_get_uid_status = 1;
    if((result == FM175XX_SUCCESS)&&(command_status.nBytesReceived == 5))
    {
        memcpy(PICC_A.UID +(cascade_level*4),command_status.pReceiveBuf,4);
        memcpy(PICC_A.BCC + cascade_level,command_status.pReceiveBuf + 4,1);
        if((PICC_A.UID[cascade_level*4] ^  PICC_A.UID[cascade_level*4 + 1] ^ PICC_A.UID[cascade_level*4 + 2]^ PICC_A.UID[cascade_level*4 + 3] ^ PICC_A.BCC[cascade_level]) !=0)
            result = FM175XX_COMM_ERR;
        PICC_A.Have_get_uid_status = 1;
    }
    return result;
}

//*************************************
//函数  名：ReaderA_Select
//入口参数：size:防冲突等级，包括0、1、2
//出口参数：uchar:0:OK  others：错误
//*************************************
unsigned char ReaderA_Select(unsigned char cascade_level)
{
    unsigned char result;
    unsigned char outbuf[7],inbuf[1];
    command_struct command_status;
    command_status.pSendBuf = outbuf;
    command_status.pReceiveBuf = inbuf;
    command_status.ReceiveCRCEnable = SET;
    command_status.SendCRCEnable = SET;
    if(cascade_level > 2)							 //最多三重防冲突
        return FM175XX_PARAM_ERR;
    *(command_status.pSendBuf+0) = RF_CMD_ANTICOL[cascade_level];   //防冲突等级
    *(command_status.pSendBuf+1) = 0x70;   					//
    *(command_status.pSendBuf+2) = PICC_A.UID[4*cascade_level];
    *(command_status.pSendBuf+3) = PICC_A.UID[4*cascade_level+1];
    *(command_status.pSendBuf+4) = PICC_A.UID[4*cascade_level+2];
    *(command_status.pSendBuf+5) = PICC_A.UID[4*cascade_level+3];
    *(command_status.pSendBuf+6) = PICC_A.BCC[cascade_level];
    command_status.nBytesToSend = 7;
    command_status.nBitsToSend = 0;
    command_status.nBitsToReceive = 0;
    command_status.nBytesToReceive = 1;
    command_status.Timeout = 2;//1ms
    command_status.Cmd = CMD_TRANSCEIVE;
    result = Command_Execute(&command_status);
    if((result == FM175XX_SUCCESS)&&(command_status.nBytesReceived == 1))
        PICC_A.SAK [cascade_level] = *(command_status.pReceiveBuf);
    else
        result = FM175XX_COMM_ERR;
    return result;
}



void SetRf(unsigned char mode)
{
    if(mode == 0)
    {
        ModifyReg(JREG_TXCONTROL,BIT0|BIT1,RESET);
    }
    if(mode == 1)
    {
        ModifyReg(JREG_TXCONTROL,BIT0,SET);
        ModifyReg(JREG_TXCONTROL,BIT1,RESET);
    }
    if(mode == 2)
    {
        ModifyReg(JREG_TXCONTROL,BIT0,RESET);
        ModifyReg(JREG_TXCONTROL,BIT1,SET);
    }
    if(mode == 3)
    {
        ModifyReg(JREG_TXCONTROL,BIT0|BIT1,SET);
    }
		delay_us(10000);
//    mDelay(10);
    // GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

unsigned char ReaderA_CardActivate(void)
{
    unsigned char  result,cascade_level;
    result = ReaderA_Wakeup();//
    if (result != FM175XX_SUCCESS)
    {
        return FM175XX_COMM_ERR;
    }

    if 	((PICC_A.ATQA[0]&0xC0)==0x00)	//1重UID
        PICC_A.CASCADE_LEVEL = 1;

    if 	((PICC_A.ATQA[0]&0xC0)==0x40)	//2重UID
        PICC_A.CASCADE_LEVEL = 2;

    if 	((PICC_A.ATQA[0]&0xC0)==0x80)	//3重UID
        PICC_A.CASCADE_LEVEL = 3;

    for (cascade_level = 0; cascade_level < PICC_A.CASCADE_LEVEL; cascade_level++)
    {
        result = ReaderA_AntiColl(cascade_level);//

        if (result != FM175XX_SUCCESS)
            return FM175XX_COMM_ERR;

        result = ReaderA_Select(cascade_level);//
        if (result != FM175XX_SUCCESS)
            return FM175XX_COMM_ERR;
    }

    return result;
}

unsigned char ReaderB_Wakeup(void)
{
    unsigned char result;
    unsigned char outbuf[3],inbuf[12];
    command_struct command_status;
    command_status.pSendBuf = outbuf;
    command_status.pReceiveBuf = inbuf;
    command_status.ReceiveCRCEnable = SET;
    command_status.SendCRCEnable = SET;
    command_status.pSendBuf[0] = 0x05;  //APf
    command_status.pSendBuf[1] = 0x00;   //AFI (00:for all cards)
    command_status.pSendBuf[2] = 0x08;   //PARAM(REQB,Number of slots =0)
    command_status.nBytesToSend = 3;//
    command_status.nBitsToSend = 0;
    command_status.nBitsToReceive = 0;
    command_status.nBytesToReceive = 12;
    command_status.Timeout = 1;//1ms
    command_status.Cmd = CMD_TRANSCEIVE;
    result = Command_Execute(&command_status);
    if((result == FM175XX_SUCCESS)&&(command_status.nBytesReceived == 12))
    {
        memcpy(PICC_B.ATQB, command_status.pReceiveBuf,12);
        memcpy(PICC_B.PUPI,PICC_B.ATQB + 1,4);
        memcpy(PICC_B.APPLICATION_DATA,PICC_B.ATQB + 6,4);
        memcpy(PICC_B.PROTOCOL_INF,PICC_B.ATQB + 10,3);
    }
    else
        result = FM175XX_COMM_ERR;
    return result;
}
unsigned char ReaderB_Request(void)
{
    unsigned char result;
    unsigned char outbuf[3],inbuf[12];
    command_struct command_status;
    command_status.pSendBuf = outbuf;
    command_status.pReceiveBuf = inbuf;
    command_status.ReceiveCRCEnable = SET;
    command_status.SendCRCEnable = SET;
    command_status.pSendBuf[0] = 0x05;  //APf
    command_status.pSendBuf[1] = 0x00;   //AFI (00:for all cards)
    command_status.pSendBuf[2] = 0x00;   //PARAM(REQB,Number of slots =0)
    command_status.nBytesToSend = 3;//
    command_status.nBitsToSend = 0;
    command_status.nBitsToReceive = 0;
    command_status.nBytesToReceive = 12;
    command_status.Timeout = 1;//1ms
    command_status.Cmd = CMD_TRANSCEIVE;
    result = Command_Execute(&command_status);
    if((result == FM175XX_SUCCESS)&&(command_status.nBytesReceived == 12))
    {
        memcpy(PICC_B.ATQB, command_status.pReceiveBuf,12);
        memcpy(PICC_B.PUPI,PICC_B.ATQB + 1,4);
        memcpy(PICC_B.APPLICATION_DATA,PICC_B.ATQB + 6,4);
        memcpy(PICC_B.PROTOCOL_INF,PICC_B.ATQB + 10,3);
    }
    else
        result = FM175XX_COMM_ERR;
    return result;
}

unsigned char ReaderB_Attrib(void)
{
    unsigned char result;
    unsigned char outbuf[9],inbuf[1];
    command_struct command_status;
    command_status.pSendBuf = outbuf;
    command_status.pReceiveBuf = inbuf;
    command_status.ReceiveCRCEnable = SET;
    command_status.SendCRCEnable = SET;
    command_status.pSendBuf[0] = 0x1D;  //
    command_status.pSendBuf[1] = PICC_B.PUPI[0];   //
    command_status.pSendBuf[2] = PICC_B.PUPI[1];   //
    command_status.pSendBuf[3] = PICC_B.PUPI[2];   //
    command_status.pSendBuf[4] = PICC_B.PUPI[3];   //
    command_status.pSendBuf[5] = 0x00;  //Param1
    command_status.pSendBuf[6] = 0x08;  //01--24,08--256
    command_status.pSendBuf[7] = 0x01;  //COMPATIBLE WITH 14443-4
    command_status.pSendBuf[8] = 0x01;  //CID:01
    command_status.nBytesToSend = 9;//
    command_status.nBitsToSend = 0;
    command_status.nBitsToReceive = 0;
    command_status.nBytesToReceive = 1;
    command_status.Timeout = 10;//10ms
    command_status.Cmd = CMD_TRANSCEIVE;
    result = Command_Execute(&command_status);
    if((result == FM175XX_SUCCESS)&&(command_status.nBytesReceived == 1))
    {
        memcpy(PICC_B.ATTRIB, command_status.pReceiveBuf,1);
    }
    else
    {
        result = FM175XX_COMM_ERR;

    }
    return result;
}

unsigned char ReaderB_GetUID(void)
{
    unsigned char result;
    unsigned char outbuf[5],inbuf[10];
    command_struct command_status;
    command_status.pSendBuf = outbuf;
    command_status.pReceiveBuf = inbuf;
    command_status.ReceiveCRCEnable = SET;
    command_status.SendCRCEnable = SET;
    command_status.pSendBuf[0] = 0x00;  //
    command_status.pSendBuf[1] = 0x36;   //
    command_status.pSendBuf[2] = 0x00;   //
    command_status.pSendBuf[3] = 0x00;   //
    command_status.pSendBuf[4] = 0x08;   //

    command_status.nBytesToSend = 5;//
    command_status.nBitsToSend = 0;
    command_status.nBitsToReceive = 0;
    command_status.nBytesToReceive = 10;
    command_status.Timeout = 1;//1ms
    command_status.Cmd = CMD_TRANSCEIVE;
    result = Command_Execute(&command_status);
    if(result == FM175XX_SUCCESS)
    {
        memcpy(PICC_B.UID, command_status.pReceiveBuf,8);

    }
    else
        result = FM175XX_COMM_ERR;

    return result;
}

unsigned char FM175XX_Polling(unsigned char *polling_card)
{
    unsigned char result;
    *polling_card = 0;
    SetRf(3);
    FM175XX_Initial_ReaderA();
    //GPIO_SetBits(GPIOC, GPIO_Pin_13);
    result = ReaderA_Wakeup();//
    //GPIO_ResetBits(GPIOC, GPIO_Pin_13);

    if (result == FM175XX_SUCCESS)
    {
        *polling_card |= BIT0;
    }
#ifdef ID_CARD
    FM175XX_Initial_ReaderB();
    result = ReaderB_Wakeup();//
    if (result == FM175XX_SUCCESS)
    {
        *polling_card |= BIT1;
    }
#endif
    SetRf(0);
    if (*polling_card != 0)
        return SUCCESS;
    else
        return ERROR;
}

u8 JUDGE(u8 *dat1,u8 *dat2,u8 len)
{
    u8 i;
    for(i=0; i<len; i++)
    {
        if(dat1[i]!=dat2[i]) return 1;
    }
    return 0;

}

unsigned char TYPE_A_EVENT(void)
{
    unsigned char result;
    //Uart_Send_Msg("\r\n-> TYPE A CARD!\r\n");
    SetRf(3);
    FM175XX_Initial_ReaderA();
    result = ReaderA_CardActivate();
    if(result != FM175XX_SUCCESS)
    {
        SetRf(0);
        return result;
    }
    NFC_SECTOR = 5;
    NFC_BLOCK = 3;
    NFC_BLOCK_NUM = (NFC_SECTOR * 4) + NFC_BLOCK;
//////////////////////CHECK LEARN///////////////////
    if(g_bool[B_NFC_LEARN_BOOL])
    {
        g_bool[B_NFC_LEARN_BOOL]=0;
/////////////////////////////check//////////////////////////////////
        result = Mifare_Auth(KEY_A_AUTH,NFC_SECTOR,KEY_A[NFC_SECTOR],PICC_A.UID);

        if(result != FM175XX_SUCCESS)
        {
            SetRf(0);
            result =FM175XX_LEARN_ERR;
            return result;
        }
////////////////////////////////write//////////////////////////////////////
        memcpy(BLOCK_DATA_W,BLOCK_DATA, 6);
        memcpy(BLOCK_DATA_W+10,BLOCK_DATA+10, 6);
        result = Mifare_Blockwrite(NFC_BLOCK_NUM,BLOCK_DATA_W);
        if(result == FM175XX_SUCCESS)
        {
            //g_bool[B_SAVE_FDATA]=1;
//			  memcpy(((u16*)g_NBCmdMap)+NB_INF_KEY_A,BLOCK_DATA_W , 6);
//			  memcpy(((u16*)g_NBCmdMap)+NB_INF_KEY_B,BLOCK_DATA_W+10 , 6);
            // memcpy(BLOCK_DATA,BLOCK_DATA_W , 16);
            result =FM175XX_LEARN_SUCCESS;
        }
        else
        {
            SetRf(0);
            result =FM175XX_LEARN_ERR;
            return result;
        }
        SetRf(0);
//        return  result;
    }
/////////////////////////////////////////////////////////////////
    else if(!g_bool[B_NFC_LEARN_BOOL])
    {
        result = Mifare_Auth(KEY_A_AUTH,NFC_SECTOR,BLOCK_DATA,PICC_A.UID);
        if(result != FM175XX_SUCCESS)
        {
            //WT588_Write(0x0004);
            //succeed++;
            SetRf(0);
            return result;
        }
        result = Mifare_Blockread(NFC_BLOCK_NUM,BLOCK_DATA_R);
        if(result != FM175XX_SUCCESS)
        {
            //WT588_Write(0x0004);
            SetRf(0);
            return result;
        }
        if(JUDGE(BLOCK_DATA+6,BLOCK_DATA_R+6,10))//succeed  save// KEY_A not support read
        {
            result =FM175XX_CHECK_ERROR;
        }
        SetRf(0);
//        return result;
    }
	return result;
}





//unsigned char TYPE_B_EVENT(void)
//{
//    unsigned char result;
//    //Uart_Send_Msg("-> TYPE B CARD!\r\n");
//    SetRf(3);
//    FM175XX_Initial_ReaderB();
////	  SetReg(0x27,0xf6);//
////	  SetReg(0x28,0x3f);//
////	  SetReg(0x29,0x1a);//
////	  SetReg(0x26,0x58);//
////	  SetReg(0x19,0x5d);//
////	  SetReg(0x18,0x83);//
////	  SetReg(0x1e,0x80);//
//    //SetReg(0x13,0x08);//
//    result = ReaderB_Wakeup();
//    if(result != FM175XX_SUCCESS)
//    {
//        SetRf(0);
//        return result;
//    }
//    //Uart_Send_Msg("-> ATQB = ");Uart_Send_Hex(PICC_B.ATQB, 12);Uart_Send_Msg("\r\n");

//    result = ReaderB_Attrib();
//    if(result != FM175XX_SUCCESS)
//    {
//        SetRf(0);
//        return result;
//    }
//    //Uart_Send_Msg("-> ATTRIB = ");Uart_Send_Hex(PICC_B.ATTRIB,1);Uart_Send_Msg("\r\n");

//    result = ReaderB_GetUID();
//    if(result != FM175XX_SUCCESS)
//    {
//        SetRf(0);
//        return result;
//    }
//    //Uart_Send_Msg("-> UID = ");Uart_Send_Hex(PICC_B.UID, 8);Uart_Send_Msg("\r\n");

//    SetRf(0);
//    return result;
//}


//***********************************************
//函数名称：Lpcd_Set_IE()
//函数功能：LPCD中断引脚设置
//入口参数：mode = 1:开启 mode = 0 ：关闭
//出口参数：SUCCESS：操作完成
//***********************************************
unsigned char Lpcd_Set_IE(unsigned char mode)
{
    if(mode == 1)
    {
        SetReg_Ext(JREG_LPCD_CTRL1,JBIT_BIT_CTRL_SET + JBIT_LPCD_IE);//打开LPCD中断
    }
    else
    {
        SetReg_Ext(JREG_LPCD_CTRL1,JBIT_BIT_CTRL_CLR + JBIT_LPCD_IE);//关闭LPCD中断
    }
    return SUCCESS;
}
//***********************************************
//函数名称：Lpcd_Set_Mode()
//函数功能：LCPD工作模式设置
//入口参数：mode = 1:开启 mode = 0 :关闭
//出口参数：
//***********************************************
void Lpcd_Set_Mode(unsigned char mode)
{
    if(mode == 1)
    {
        Lpcd_Set_IE(1);//FM175XX打开LPCD中断
        FM175XX_HPD(0);//进入LPCD模式
    }
    else
    {
        FM175XX_HPD(1);//退出LPCD模式
        Lpcd_Set_IE(0);//FM75XX关闭LPCD中断
    }
    return;
}
void Lpcd_Set_IRQ_pin(void)
{
    SetReg(JREG_COMMIEN,BIT7);//IRQ引脚反相输出
    SetReg(JREG_DIVIEN,BIT7);//IRQ引脚CMOS输出模式（IRQ引脚不需要外接上拉电阻）
}
//***********************************************
//函数名称：Lpcd_Get_IRQ()
//函数功能：LPCD读取中断标志
//入口参数：*irq IRQ标志
//出口参数:
//***********************************************
void Lpcd_Get_IRQ(unsigned char irq)
{
    GetReg_Ext(JREG_LPCD_IRQ,&(irq));//读取LPCD中断寄存器
//	if(Lpcd.Irq & JBIT_CARD_IN_IRQ)
//	{
//		Lpcd_Get_Value(&Lpcd.Value);
//	}
    Lpcd_Reset_Status();//清除FM175XX中断状态
    return;
}
//函数名称：Lpcd_Reset_Status()
//函数功能：LPCD功能复位
//入口参数：
//出口参数：SUCCESS：操作完成
//***********************************************
unsigned char Lpcd_Reset_Status(void)//LPCD功能复位
{
    SetReg_Ext(JREG_LPCD_CTRL1,JBIT_BIT_CTRL_CLR + JBIT_LPCD_RSTN);  //复位LPCD寄存器
    SetReg_Ext(JREG_LPCD_CTRL1,JBIT_BIT_CTRL_SET + JBIT_LPCD_RSTN); //复位放开LPCD寄存器
    return SUCCESS;
}
//***********************************************
//函数名称：Lpcd_Init_Register()
//函数功能：LPCD寄存器初始化
//入口参数：
//出口参数：SUCCESS：操作完成
//***********************************************
unsigned char Lpcd_Init_Register(void)
{
    Lpcd_Set_IRQ_pin();

    SetReg_Ext(JREG_LPCD_CTRL1,JBIT_BIT_CTRL_SET + JBIT_LPCD_EN);	//使能LPCD功能

    if(LPCD_AUTO_DETECT_TIMES == 1)
        SetReg_Ext(JREG_LPCD_CTRL1,JBIT_BIT_CTRL_SET + JBIT_LPCD_SENSE_1);//配置1次检测
    else
        SetReg_Ext(JREG_LPCD_CTRL1,JBIT_BIT_CTRL_CLR + JBIT_LPCD_SENSE_1);//配置3次检测

    SetReg_Ext(JREG_LPCD_CTRL3,LPCD_MODE << 3);//配置LpcdCtrl3寄存器,LPCD工作模式
    SetReg_Ext(JREG_LPCD_AUTO_WUP_CFG,(LPCD_AUTO_WUP_EN << 3) + LPCD_AUTO_WUP_CFG );//配置Auto_Wup_Cfg寄存器

    return SUCCESS;
}
//***********************************************
//函数名称：Lpcd_Set_Timer()
//函数功能：LPCD设置Timer
//入口参数：
//出口参数：SUCCESS：操作完成
//***********************************************
unsigned char Lpcd_Set_Timer(void)
{
    Lpcd.Timer1 = LPCD_TIMER1_CFG & 0x0F;//TIMER1 = 0x01~0x0F
    Lpcd.Timer2 = LPCD_TIMER2_CFG & 0x1F;//TIMER2 = 0x01~0x1F
    Lpcd.Timer3 = LPCD_TIMER3_CFG & 0x1F;//TIMER3 = 0x03~0x1F
    Lpcd.TimerVmid = LPCD_TIMER_VMID_CFG & 0x1F;
    if (Lpcd.Timer3 > 0xF) //Timer3Cfg用到5bit，选择16分频
    {
        Lpcd.Timer3_Offset = 0x05;
        Lpcd.Timer3_Div  = 2;			//16分频
        Lpcd.Fullscale_Value =  ((Lpcd.Timer3 - 1)<<3) - Lpcd.Timer3_Offset;
    }
    else if(Lpcd.Timer3 > 0x7) //Timer3Cfg用到4bit，选择8分频
    {

        Lpcd.Timer3_Offset = 0x0E;
        Lpcd.Timer3_Div  = 1;			//8分频
        Lpcd.Fullscale_Value =  ((Lpcd.Timer3 - 1)<<4) - Lpcd.Timer3_Offset;
    }
    else
    {
        Lpcd.Timer3_Offset = 0x1F;
        Lpcd.Timer3_Div  = 0;			//4分频
        Lpcd.Fullscale_Value =  ((Lpcd.Timer3 - 1)<<5) - Lpcd.Timer3_Offset;
    }
    Lpcd.Calibration.Range_L = 0x45;//3*LPCD_THRESHOLD;//
    Lpcd.Calibration.Range_H = 0x75;//Lpcd.Fullscale_Value - 3*LPCD_THRESHOLD;//
    SetReg_Ext(JREG_LPCD_T1CFG,(Lpcd.Timer3_Div <<4) + Lpcd.Timer1);//配置Timer1Cfg寄存器
    SetReg_Ext(JREG_LPCD_T2CFG,Lpcd.Timer2);//配置Timer2Cfg寄存器
    SetReg_Ext(JREG_LPCD_T3CFG,Lpcd.Timer3);//配置Timer3Cfg寄存器
    SetReg_Ext(JREG_LPCD_VMIDBD_CFG,Lpcd.TimerVmid);//配置VmidBdCfg寄存器
    return SUCCESS;
}



void Lpcd_IRQ_Event(void)
{
    unsigned char irq;

    Lpcd_Get_IRQ(irq);
    //Lpcd_Set_Driver(LPCD_P_DRIVER[Lpcd.Calibration.Driver_Index],LPCD_N_DRIVER[Lpcd.Calibration.Driver_Index],LPCD_TX2RF_EN);//配置LPCD输出驱动
    //Lpcd_Set_Reference(LPCD_BIAS_CURRENT,Lpcd.Calibration.Reference);
    //Lpcd_Set_Gain(LPCD_GAIN[Lpcd.Calibration.Gain_Index]);
    Lpcd_Set_IRQ_pin();

    return;
}

void uDelay(unsigned int us)			//实测1us
{
    unsigned int i;
    unsigned int j;
    for(j=0; j<us; j++)
    {
        for(i=0; i<81; i++)
            ;
    }
}


//***********************************************
//函数名称：Lpcd_Set_Gain()
//函数功能：LPCD设置幅度检波包络信号的放大、衰减增益
//入口参数：
//出口参数：SUCCESS：操作完成
//***********************************************
unsigned char Lpcd_Set_Gain(unsigned char lpcd_gain)
{
    lpcd_gain &= 0x1F;
    SetReg_Ext(JREG_LPCD_CTRL4,lpcd_gain);
    return SUCCESS;
}
//***********************************************
//函数名称：WaitForLpcdIrq()
//函数功能：等待LPCD中断
//入口参数：IrqType
//出口参数：SUCCESS：读取操作完成   ERROR：超时退出
//***********************************************
unsigned char Lpcd_WaitFor_Irq(unsigned char IrqType)
{
    unsigned char ExtRegData;
    unsigned char TimeOutCount;

    TimeOutCount = 0;
    GetReg_Ext(JREG_LPCD_IRQ,&ExtRegData);

    for(TimeOutCount = LPCD_CALIBRATE_IRQ_TIMEOUT; TimeOutCount > 0; TimeOutCount--)
    {
        uDelay(1);	//延时1us
        GetReg_Ext(JREG_LPCD_IRQ,&ExtRegData);
        if(ExtRegData & IrqType)
            return SUCCESS;
    }
    return ERROR;
}
//***********************************************
//函数名称：Lpcd_Set_Drive()
//函数功能：LPCD设置输出驱动
//入口参数：
//出口参数：SUCCESS：操作完成
//***********************************************
unsigned char Lpcd_Set_Driver(unsigned char lpcd_cwp,unsigned char lpcd_cwn,unsigned char lpcd_tx2_en)
{
    unsigned char reg_data;
    GetReg(JREG_VERSION,&reg_data);
    if(reg_data == 0x88)//V03版本芯片
    {
        if(lpcd_cwn > 1)
            lpcd_cwn = 1;
        lpcd_cwn &= 0x01;
        lpcd_cwp &= 0x07;
        SetReg_Ext(JREG_LPCD_CTRL2,((lpcd_tx2_en << 4) + (lpcd_cwn << 3) + lpcd_cwp));//V03版本芯片
    }
    if(reg_data == 0x89)//V03以上版本芯片
    {
        lpcd_cwn &= 0x0F;
        lpcd_cwp &= 0x07;
        SetReg_Ext(JREG_LPCD_CTRL2,((lpcd_tx2_en<<4) + lpcd_cwp));//V03以上版本芯片
        SetReg(JREG_GSN, lpcd_cwn << 4); //V03以上版本芯片
    }
    return SUCCESS;
}
//***********************************************
//函数名称：Lpcd_Get_Value()
//函数功能：读取LPCD的数值
//入口参数： *value
//出口参数：SUCCESS：读取操作完成   ERROR:操作超时
//***********************************************

unsigned char Lpcd_Get_Value(unsigned char *value)
{
    unsigned char ExtRegData;
    GetReg_Ext(JREG_LPCD_ADC_RESULT_H,&ExtRegData);//读取幅度信息，高2位

    *value = ((ExtRegData & 0x3) << 6);

    GetReg_Ext(JREG_LPCD_ADC_RESULT_L,&ExtRegData);//读取幅度信息，低6位

    *value += (ExtRegData & 0x3F);

    return SUCCESS;
}
unsigned char Lpcd_Get_Calibration_Value(unsigned char *value)
{
    unsigned char result;

    SetReg_Ext(JREG_LPCD_MISC,BFL_JBIT_AMP_VMID_EN_SEL);//使能VMID电源，BFL_JBIT_AMP_EN_SEL = 1 提前使能AMP	Lpcd_Reset_Status();  //清除CalibraIRq标志
    mDelay(5);
    SetReg_Ext(JREG_LPCD_CTRL1,JBIT_BIT_CTRL_CLR + JBIT_LPCD_CALIBRATE_EN);//关闭校准模式
    SetReg_Ext(JREG_LPCD_CTRL1,JBIT_BIT_CTRL_SET + JBIT_LPCD_CALIBRATE_EN);//使能校准模式

    result = Lpcd_WaitFor_Irq(JBIT_CALIBRATE_IRQ);//等待校准结束中断

    ModifyReg_Ext(JREG_LPCD_MISC,BFL_JBIT_CALIBRATE_VMID_EN,0);//关闭VMID电源

    SetReg_Ext(JREG_LPCD_CTRL1,JBIT_BIT_CTRL_CLR + JBIT_LPCD_CALIBRATE_EN);//关闭校准模式

    Lpcd_Get_Value(&*value);

    Lpcd_Reset_Status();  //清除CalibraIRq标志

    return result;
}
//***********************************************
//函数名称：Lpcd_Set_Threshold()
//函数功能：LPCD设置触发阈值
//入口参数：
//出口参数：SUCCESS：操作完成
//***********************************************
unsigned char Lpcd_Set_Threshold(unsigned char lpcd_threshold_min,unsigned char lpcd_threshold_max)
{
    unsigned char temp;

    if(lpcd_threshold_max < lpcd_threshold_min)
    {
        temp = lpcd_threshold_min;
        lpcd_threshold_min = lpcd_threshold_max;
        lpcd_threshold_max = temp;
    }

    SetReg_Ext(JREG_LPCD_THRESHOLD_MIN_L,(lpcd_threshold_min & 0x3F));//写入THRESHOLD_Min阈值低6位

    SetReg_Ext(JREG_LPCD_THRESHOLD_MIN_H,(lpcd_threshold_min>>6));//写入THRESHOLD_Min阈值高2位

    SetReg_Ext(JREG_LPCD_THRESHOLD_MAX_L,(lpcd_threshold_max & 0x3F));//写入THRESHOLD_Max阈值低6位

    SetReg_Ext(JREG_LPCD_THRESHOLD_MAX_H,(lpcd_threshold_max>>6));//写入THRESHOLD_Max阈值高2位

    return SUCCESS;

}

//***********************************************
//函数名称：Lpcd_Calibrate_Driver()
//函数功能：LPCD设置校准输出驱动
//入口参数：
//出口参数：SUCCESS：操作完成
//***********************************************
unsigned char Lpcd_Debug_Info = 0;
unsigned char Lpcd_Debug_Test = 0;
unsigned char Lpcd_Calibrate_Driver(void)
{
    unsigned char i,j;

    Lpcd.Calibration.Gain_Index = LPCD_GAIN_INDEX ;//

    for(j = 0; j < 12; j++) //
    {
        Lpcd_Set_Gain(LPCD_GAIN[Lpcd.Calibration.Gain_Index]);// 设置Gain
        for(i = 0; i < 7; i ++ ) //
        {
            Lpcd.Calibration.Driver_Index = i;
            Lpcd_Set_Driver(LPCD_P_DRIVER[Lpcd.Calibration.Driver_Index],LPCD_N_DRIVER[Lpcd.Calibration.Driver_Index],LPCD_TX2RF_EN);//
            Lpcd_Get_Calibration_Value(&Lpcd.Calibration.Value);//设置Driver
//			 if(Lpcd_Debug_Info)
//			 {
//      // Uart_Send_Msg("-> Driver = "); Uart_Send_Hex(&i,1); //Uart_Send_Msg("\r\n");
//       //Uart_Send_Msg("; Lpcd.Calibration.Value = "); Uart_Send_Hex(&Lpcd.Calibration.Value,1); Uart_Send_Msg("\r\n");
//			 }
            if((Lpcd.Calibration.Value > Lpcd.Calibration.Range_L)&&(Lpcd.Calibration.Value < Lpcd.Calibration.Range_H))
            {
                if((Lpcd.Calibration.Value - LPCD_THRESHOLD > 0 )&&((Lpcd.Calibration.Value + LPCD_THRESHOLD) < Lpcd.Fullscale_Value))//

                {
                    Lpcd.Calibration.Threshold_Max = Lpcd.Calibration.Value + LPCD_THRESHOLD;
                    Lpcd.Calibration.Threshold_Min = Lpcd.Calibration.Value - LPCD_THRESHOLD;

//               Uart_Send_Msg("-> Lpcd.Calibration.Value = "); Uart_Send_Hex(&Lpcd.Calibration.Value,1); //Uart_Send_Msg("\r\n");//
//						   Uart_Send_Msg("; Lpcd.Fullscale_Value = "); Uart_Send_Hex(&Lpcd.Fullscale_Value,1); Uart_Send_Msg("\r\n");
//               Uart_Send_Msg("-> Lpcd.Calibration.Threshold_Max = "); Uart_Send_Hex(&Lpcd.Calibration.Threshold_Max,1); //
//               Uart_Send_Msg("; Lpcd.Calibration.Threshold_Min = "); Uart_Send_Hex(&Lpcd.Calibration.Threshold_Min,1); Uart_Send_Msg("\r\n");//
                    Lpcd_Set_Threshold(Lpcd.Calibration.Threshold_Min,Lpcd.Calibration.Threshold_Max);//
//						   if(Lpcd_Debug_Test)
//								 {
//									while(1)//
//										 {
//											 Lpcd_Get_Calibration_Value(&Lpcd.Calibration.Value);//
//											// Uart_Send_Msg("-> Lpcd.Calibration.Value2 = "); Uart_Send_Hex(&Lpcd.Calibration.Value,1); Uart_Send_Msg("\r\n");
//										 }
//								 }
                    // Uart_Send_Msg("Calibra Driver Success!\r\n");
                    return SUCCESS;
                }
            }


        }

        if(Lpcd.Calibration.Value > Lpcd.Calibration.Range_H)//
        {
            if(Lpcd.Calibration.Gain_Index == 11)
                break;//
            else
                Lpcd.Calibration.Gain_Index++;
            // Uart_Send_Msg("Gain_Index++\r\n");
        }
        if(Lpcd.Calibration.Value < Lpcd.Calibration.Range_L)//
        {
            if(Lpcd.Calibration.Gain_Index == 0)
                break;//
            else
                Lpcd.Calibration.Gain_Index--;
            //  Uart_Send_Msg("Gain_Index--\r\n");
        }

    }
    // Uart_Send_Msg("Calibra Driver Error !\r\n");//
    return ERROR;
}
//***********************************************
//函数名称：Lpcd_Set_Reference()
//函数功能：LPCD设置基准信号的充电电流与充电电容
//入口参数：
//出口参数：SUCCESS：校准成功；ERROR：校准失败
//***********************************************
unsigned char Lpcd_Set_Reference(unsigned char lpcd_bias_current,unsigned char lpcd_reference)
{
    lpcd_bias_current &= 0x07; //0x00 ~ 0x07 基准电流
    lpcd_reference &= 0x7F;  //0x00 ~ 0x7F 基准电容
    SetReg_Ext(JREG_LPCD_BIAS_CURRENT,((lpcd_reference  & 0x40)>>1) + (lpcd_bias_current & 0x07));
    SetReg_Ext(JREG_LPCD_ADC_REFERECE,(lpcd_reference  & 0x3F));
    return SUCCESS;
}
//***********************************************
//函数名称：Lpcd_Calibrate_Reference()
//函数功能：LPCD设置校准基准电压
//入口参数：
//出口参数：SUCCESS：校准成功；ERROR：校准失败
//***********************************************
unsigned char Lpcd_Calibrate_Reference(void)
{
    unsigned char i,result;

    result = ERROR; //初始值配置
    Lpcd_Reset_Status();//校准基准电压前，先复位状态机
    for(i = 0 ; i < 0x7F; i ++)
    {
        Lpcd.Calibration.Reference = 0 + i;//
        Lpcd_Set_Reference(LPCD_BIAS_CURRENT,Lpcd.Calibration.Reference);//

        Lpcd.Calibration.Gain_Index = 10;//  max gain   1 shuaijian
        Lpcd_Set_Gain(LPCD_GAIN[Lpcd.Calibration.Gain_Index]);//

        Lpcd_Set_Driver(7,15,1);//
        Lpcd_Get_Calibration_Value(&Lpcd.Calibration.Value);
        if(Lpcd_Debug_Info)
        {
            //Uart_Send_Msg("-> Refference = "); Uart_Send_Hex(&i,1); //Uart_Send_Msg("\r\n");
            //Uart_Send_Msg("; Lpcd.Calibration.Value = "); Uart_Send_Hex(&Lpcd.Calibration.Value,1); Uart_Send_Msg("\r\n");
        }
        if((Lpcd.Calibration.Value == 0)&&(Lpcd.Calibration.Reference != 0))
        {
            //Uart_Send_Msg("Calibra Reference Success!\r\n");
            result = SUCCESS;
            break;//
        }
        if((Lpcd.Calibration.Value == 0)&&(Lpcd.Calibration.Reference == 0))//  ANT small
        {
            // Uart_Send_Msg("Calibra Reference Error!\r\n");
            result = ERROR; //
            break;
        }
    }
    return result;
}
void Lpcd_Calibration_Restore(void)
{
    Lpcd.Calibration.Reference = Lpcd.Calibration_Backup.Reference;
    Lpcd.Calibration.Gain_Index = Lpcd.Calibration_Backup.Gain_Index;
    Lpcd.Calibration.Driver_Index = Lpcd.Calibration_Backup.Driver_Index;
    Lpcd_Set_Reference(LPCD_BIAS_CURRENT,Lpcd.Calibration.Reference);
    Lpcd_Set_Gain(LPCD_GAIN[Lpcd.Calibration.Gain_Index]);
    Lpcd_Set_Driver(LPCD_P_DRIVER[Lpcd.Calibration.Driver_Index],LPCD_N_DRIVER[Lpcd.Calibration.Driver_Index],LPCD_TX2RF_EN);
    //Uart_Send_Msg("-> Restore Success!\r\n");
    return;
}
void Lpcd_Calibration_Backup(void)//
{
    Lpcd.Calibration_Backup.Reference = Lpcd.Calibration.Reference;
    Lpcd.Calibration_Backup.Gain_Index = Lpcd.Calibration.Gain_Index;
    Lpcd.Calibration_Backup.Driver_Index = Lpcd.Calibration.Driver_Index;
    //Uart_Send_Msg("-> Bakckup Success!\r\n");
    return;
}
//***********************************************
//函数名称：Lpcd_Calibration_Event()
//函数功能：LCPD校准程序
//入口参数：无
//出口参数：SUCCESS:校准成功；ERROR：校准失败
//***********************************************

unsigned char Lpcd_Calibration_Event(void)
{
    unsigned char result;

    Lpcd_Init_Register();		//
    Lpcd_Set_Timer();
    //Lpcd_Set_Aux(LPCD_AUX_EN);	// ??AUX????
    Lpcd_Reset_Status();
    result = Lpcd_Calibrate_Reference();
    if(result == ERROR)
        return ERROR;
    result = Lpcd_Calibrate_Driver();
    if (result == SUCCESS)
    {
        Lpcd_Calibration_Backup();//save the Calibrate parameter
        return SUCCESS;
    }
    else
    {
        Lpcd_Calibration_Restore();//恢复校准参数
    }
    return ERROR;
}

//天线校准
void RE_LPCD_CALIBRATION(void)
{
//    Lpcd_Set_Mode(0);     //leave lpcd
//    Lpcd_IRQ_Event();     //	clear IRq  bit
//    if(Lpcd_Calibration_Event()== SUCCESS)//
//    {
//        NFC_set.NFC_CLA_STATE = 2;
//        ClearErrorByte(MINI_NFC_CAL_ERROR);
//        Lpcd_Calibration_Backup();
//    }
//    else
//    {
//        NFC_set.NFC_CLA_STATE = 3;
//        SetErrorByte(MINI_NFC_CAL_ERROR, MySelf_Err);
//    }
//    Lpcd_Set_Mode(1); //LPCD  IN
}

//void check_NFC_key(u8 type)
//{
//    static u8 i,s_check_cnt=0;
//#ifdef CHECK_NFC_KEY
//    if(type==0x01)
//    {
//        for(i=0; i<4; i++) //  ??
//        {
//            if(PICC_A.UID[i]!=ID1[i])
//                break;
//            if(i==3)
//                g_NBCmdMap[NB_INF_POWER_ON_CNT]++;
//        }
//        for(i=0; i<8; i++) //??
//        {
//            if(PICC_A.UID[i]!=ID2[i])
//                break;
//            if(i==7)
//                g_NBCmdMap[NB_INF_POWER_ON_CNT]++;
//        }
//        for(i=0; i<8; i++) //??3
//        {
//            if(PICC_A.UID[i]!=ID8[i])
//                break;
//            if(i==7)
//                g_NBCmdMap[NB_INF_POWER_ON_CNT]++;
//        }
//        for(i=0; i<8; i++) //??4
//        {
//            if(PICC_A.UID[i]!=ID7[i])
//                break;
//            if(i==7)
//                g_NBCmdMap[NB_INF_POWER_ON_CNT]++;
//        }
//    }
//    if(type==0x02)
//    {
//        for(i=0; i<8; i++) //???1
//        {
//            if(PICC_B.UID[i]!=ID3[i])
//                break;
//            if(i==7)
//                g_NBCmdMap[NB_INF_POWER_ON_CNT]++;
//        }
//        for(i=0; i<8; i++) //???2
//        {
//            if(PICC_B.UID[i]!=ID4[i])
//                break;
//            if(i==7)
//                g_NBCmdMap[NB_INF_POWER_ON_CNT]++;
//        }
//        for(i=0; i<8; i++) //???2
//        {
//            if(PICC_B.UID[i]!=ID5[i])
//                break;
//            if(i==7)
//                g_NBCmdMap[NB_INF_POWER_ON_CNT]++;
//        }
//        for(i=0; i<8; i++) //???2
//        {
//            if(PICC_B.UID[i]!=ID6[i])
//                break;
//            if(i==7)
//                g_NBCmdMap[NB_INF_POWER_ON_CNT]++;
//        }
//    }
//#endif
//    //g_NBCmdMap[NB_INF_POWER_ON_CNT]++;

//    if( *_rental_object.status_register != IOT_NORMAL_WORK)
//    {
//        if(g_bool[B_ACC_STATE]==0)
//        {
//            PowCtrl_PowerON();
//        }
//        else
//        {
//            PowCtrl_PowerOFF();
//        }
//    }

//    memcpy((u8*)&_my_id.s_card_id,PICC_A.UID,4);
////    _my_id.s_card_id = (*(u32*)PICC_A.UID);
//    memset((u8*)&PICC_A.UID, 0, 8);
//    memset((u8*)&PICC_B.UID, 0, 8);
//}


u8 s_error_cnt;
extern u8 VALUE_CLA;
void LPCD_Read(void)
{
    static u8 s_NFC_Flage=0,s_Check=0;
    static u16 s_Count;
    unsigned char s_polling_card;
    unsigned char result;

    if(g_bool[B_IN_IAPMODE])
    {
        return;
    }

    if(NFC_set.SET_NFC_CALIBRATE == 1)  //有天线校准指令
    {
        NFC_set.SET_NFC_CALIBRATE = 0;
        NFC_set.NFC_CLA_STATE = 1;
        RE_LPCD_CALIBRATION();//    recalibrate
    }
//		NFC_INT = 1;
    if(NFC_INT) //有外部中断
    {
        s_Count=0;   //clear
        s_Check=1;
        NFC_INT=0;   //
        Lpcd_Set_Mode(0);     //leave lpcd
        Lpcd_IRQ_Event();     //	clear IRq  bit
        if(s_NFC_Flage==1)
        {
            Lpcd_Set_Mode(1);     //进入lpcd模式
            return;
        }
        if(FM175XX_Polling(&s_polling_card)== SUCCESS)
        {
            s_error_cnt=0;
            if(s_polling_card & BIT0)//TYPE A
            {
//							  PICC_A.Have_get_uid_status = 1;
                result=TYPE_A_EVENT();
                //result=FM175XX_SUCCESS;
            }
            if(s_polling_card & BIT1)
            {
#ifdef ID_CARD
                result=TYPE_B_EVENT();
#else
                result=FM175XX_CHECK_ERROR;
#endif
            }
            if((result==FM175XX_SUCCESS)||(result==FM175XX_LEARN_SUCCESS))
            {

                if(result==FM175XX_LEARN_SUCCESS)//
                {
                    s_NFC_Flage=1;
                    //g_bool[B_NFC_LEARN_BOOL]=0;
                    Lpcd_Set_Mode(1);     // IN lpcd
//                    WT588_Write(VOICE_LEARN_SUCCEED);
                    return;
                }
                //WT588_Write(0x0008);
//                if(BREAKIN()==0)//  need lv bo
//                {
//                    s_NFC_Flage=1;
////                    WT588_PlayVoice(VOICE_OPEN_BUCKET+VOICE_SEL,(u8)g_NBCmdMap[NB_CTL_WARM_VOLUME]);
//                }
                //if(g_RealSpeed_ABS<2)
//                else if(MC_RunningData.params.unlock_lock_State == LOCKSTATE_UNLOCK)//  out of power down state
//                {
//                    check_NFC_key(s_polling_card);	//
//                }
                s_NFC_Flage=1;
                Lpcd_Set_Mode(1);     // IN lpcd
            }
            else  //polling succed but not read and learn successful
            {
                if(result==FM175XX_LEARN_ERR)
                {
                    s_NFC_Flage=1;
//                    if(g_bool[B_POWER_DOWN]==0)
//                        WT588_Write(VOICE_LEARN_FAULT);
                    Lpcd_Set_Mode(1);     // IN lpcd
                    return;
                }
                else if(result==FM175XX_CHECK_ERROR)
                {
                    s_NFC_Flage=1;
                    Lpcd_Set_Mode(1);     // IN lpcd
                    return;
                }
                s_NFC_Flage=0;
                Lpcd_Set_Mode(1);     // IN lpcd
            }
        }
        else  //polling fail
        {
            s_NFC_Flage=0;
            Lpcd_Set_Mode(1);
            // WT588_Write(0x000B);
            s_error_cnt++;
            //g_NBCmdMap[NB_INF_NFC_CNT]++;
            if(s_error_cnt>5)//  10 ci recalibration
            {
                s_error_cnt=0;
//                RE_LPCD_CALIBRATION();
            }
        }
    }
    else
    {
        if(s_Check)
        {
            if(s_Count<600/5)
            {
                s_Count++;
                return;
            }
            else
            {
                s_NFC_Flage=0;
                s_Check=0;
            }
        }
    }

}

//static void NFC_Card_ID_Pro(void)
//{
//    if(PICC_A.Have_get_uid_status)
//    {
//			
//        PICC_A.Have_get_uid_status =0;
//        _my_id.s_upload_timeout = 9;
//        PushMiniFrame(MY_ID, ID_UART_DEBUGGER, _my_id.s_card_id_len, _my_id.s_send_cmd_id, 0, (u8*)&_my_id.s_card_id,COM_IOT); //发送心跳给gps盒子
//        return;
//    }
//    else
//    {
//        if( _my_id.s_upload_timeout > 0)
//        {
//            _my_id.s_upload_timeout --;
//            PushMiniFrame(V6_MY_ID, ID_UART_DEBUGGER, _my_id.s_card_id_len, _my_id.s_send_cmd_id, 0, (u8*)&_my_id.s_card_id,COM_IOT); //发送心跳给gps盒子
//            return;
//        }
//        _my_id.s_card_id = 0;
//    }
//}

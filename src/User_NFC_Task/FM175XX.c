/******************** (C) COPYRIGHT 2020 Ninebot *******************************
* File Name     : FM175XX.c
* Author        : meihua
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "FM175XX.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/*****************************************************************
* Function Name : mDelay
* Description   : ms��ʱ���� 
* Input         : ms����ʱ������
* Output        : None
* Notes         :
******************************************************************/
void mDelay(u16 ms)
{
    SysTick->LOAD = 1000*8; 			//����ʱ��Ϊ1ms
    SysTick->VAL = 0x00;        	//��ռ�����
    SysTick->CTRL = 0x01;      		//��ʼ����

    while(ms)
    {
        while((SysTick->CTRL & 0x10000) == 0);
        ms--;
    }
    SysTick->CTRL=0x00;       //�رռ�����
    SysTick->VAL =0X00;       //��ռ�����
}

/*****************************************************************
* Function Name : FM175XX_HPD
* Description   : ģʽ���� 
* Input         : mode��mode = 1 �˳�HPDģʽ ��mode = 0 ����HPDģʽ
* Output        : None
* Notes         :
******************************************************************/
void FM175XX_HPD(unsigned char mode)
{
    if(mode == 0x0)
    {
        GPIO_ResetBits(PORT_NRST,PIN_NRST);//NPD = 0	����HPDģʽ
    }
    else
    {
        GPIO_SetBits(PORT_NRST,PIN_NRST);	//NPD = 1 �˳�HPDģʽ
//        mDelay(1);
        delay_us(1000); //��ʱ1ms���ȴ�FM175XX����
    }
    return;
}

/*****************************************************************
* Function Name : GetReg
* Description   : ��ȡ�Ĵ���ֵ
* Input         : addr:Ŀ��Ĵ�����ַ   regdata:��ȡ��ֵ
* Output        : None
* Notes         :
******************************************************************/
void GetReg(uchar addr,uchar *regdata)
{
    GPIO_ResetBits(PORT_NSS,PIN_NSS);	//NSS = 0;
    addr = (addr << 1) | 0x80;

    SPI_SendData8(NFC_SPI,addr);   /* Send SPI2 data */
    //while(SPI_GetFlagStatus(NFC_SPI, SPI_FLAG_TXE)==RESET);	  /* Wait for SPI2 Tx buffer empty */
    while(SPI_I2S_GetFlagStatus(NFC_SPI, SPI_I2S_FLAG_RXNE)==RESET);
    SPI_I2S_ClearFlag(NFC_SPI,SPI_I2S_FLAG_RXNE);
    *regdata = SPI_ReceiveData8(NFC_SPI);		 /* Read SPI2 received data */
    SPI_I2S_ClearFlag(NFC_SPI, SPI_I2S_FLAG_RXNE);
    SPI_SendData8(NFC_SPI,0x00);   /* Send SPI2 data */
    //while(SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE)==RESET)
    while(SPI_I2S_GetFlagStatus(NFC_SPI, SPI_I2S_FLAG_RXNE)==RESET);
    SPI_I2S_ClearFlag(NFC_SPI,SPI_I2S_FLAG_RXNE);
    *regdata = SPI_ReceiveData8(NFC_SPI);		 /* Read SPI2 received data */

    GPIO_SetBits(PORT_NSS,PIN_NSS);	//NSS = 1;
    return ;
}

/*****************************************************************
* Function Name : SetReg
* Description   : д�Ĵ���ֵ
* Input         : addr:Ŀ��Ĵ�����ַ   regdata:Ҫд���ֵ
* Output        : None
* Notes         :
******************************************************************/
void SetReg(uchar addr,uchar regdata)
{
    GPIO_ResetBits(PORT_NSS,PIN_NSS);	//NSS = 0;
    addr = (addr << 1) & 0x7F;
    SPI_SendData8(NFC_SPI,addr);   /* Send SPI1 data */
    //while(SPI_GetFlagStatus(NFC_SPI, SPI_FLAG_TXE)==RESET);	  /* Wait for SPI2 Tx buffer empty */
    while(SPI_I2S_GetFlagStatus(NFC_SPI, SPI_I2S_FLAG_RXNE)==RESET)
			;
    SPI_I2S_ClearFlag(NFC_SPI,SPI_I2S_FLAG_RXNE);
    SPI_ReceiveData8(NFC_SPI);		 /* Read SPI2 received data */;
    SPI_SendData8(NFC_SPI,regdata);   /* Send SPI2 data */
    //while(SPI_GetFlagStatus(NFC_SPI, SPI_FLAG_TXE)==RESET);
    while(SPI_I2S_GetFlagStatus(NFC_SPI, SPI_I2S_FLAG_RXNE)==RESET)
			;
    SPI_I2S_ClearFlag(NFC_SPI,SPI_I2S_FLAG_RXNE);
    SPI_ReceiveData8(NFC_SPI);		 /* Read SPI2 received data */
    GPIO_SetBits(PORT_NSS,PIN_NSS);	//NSS = 1;

    return ;
}

/*****************************************************************
* Function Name : ModifyReg
* Description   : �޸ļĴ�����λ
* Input         : addr:Ŀ��Ĵ�����ַ   mask:Ҫ�ı��λ
                : set:�޸�λ����ֵ 0������ 1����λ
* Output        : None
* Notes         :
******************************************************************/
void ModifyReg(uchar addr,uchar mask,uchar set)
{
    uchar regdata;

    GetReg(addr,&regdata);

    if(set)
    {
        regdata |= mask;
    }
    else
    {
        regdata &= ~mask;
    }

    SetReg(addr,regdata);
    return ;
}

/*****************************************************************
* Function Name : SPI_Write_FIFO
* Description   : SPI�ӿ�����дFIFO�Ĵ���
* Input         : reglen��Ҫд����ֽ���
                : regbuf��Ҫд�������buf��ַ
* Output        : None
* Notes         :
******************************************************************/
void SPI_Write_FIFO(u8 reglen,u8* regbuf) 
{
    u8  i;
    u8 regdata;

    GPIO_ResetBits(PORT_NSS, PIN_NSS);	    //NSS = 0;
    SPI_SendData8(NFC_SPI,0x12);            /* Send FIFO addr 0x09<<1 */
    while(SPI_I2S_GetFlagStatus(NFC_SPI, SPI_I2S_FLAG_RXNE) == RESET);
    regdata = SPI_ReceiveData8(NFC_SPI);    /* not care data */
    for(i = 0; i < reglen; i++)
    {
        regdata = *(regbuf+i);	            //RegData_i
        SPI_SendData8(NFC_SPI,regdata);     /* Send addr_i i��1*/
        while(SPI_I2S_GetFlagStatus(NFC_SPI, SPI_I2S_FLAG_RXNE) == RESET);
        regdata = SPI_ReceiveData8(NFC_SPI);/* not care data */
    }
    GPIO_SetBits(PORT_NSS,PIN_NSS);	        //NSS = 1;

    return;
}

/*****************************************************************
* Function Name : SPI_Read_FIFO
* Description   : SPI�ӿ�������FIFO
* Input         : reglen��Ҫ�������ֽ���
                : regbuf��Ҫ���������buf��ַ
* Output        : None
* Notes         :
******************************************************************/
void SPI_Read_FIFO(u8 reglen,u8* regbuf)
{
    u8  i;

    GPIO_ResetBits(PORT_NSS, PIN_NSS);	//NSS = 0;
    SPI_SendData8(NFC_SPI,0x92);        /* Send FIFO addr 0x09<<1|0x80 */
    while(SPI_I2S_GetFlagStatus(NFC_SPI, SPI_I2S_FLAG_RXNE)==RESET);
    *(regbuf) = SPI_ReceiveData8(NFC_SPI);		 /* not care data */

    for(i=1; i<reglen; i++)
    {
        SPI_SendData8(NFC_SPI,0x92);    /* Send FIFO addr 0x09<<1|0x80 */
        while(SPI_I2S_GetFlagStatus(NFC_SPI, SPI_I2S_FLAG_RXNE)==RESET);
        *(regbuf+i-1) = SPI_ReceiveData8(NFC_SPI);  //Data_i-1
    }
    SPI_SendData8(NFC_SPI,0x00);        /* Send recvEnd data 0x00 */
    while(SPI_I2S_GetFlagStatus(NFC_SPI, SPI_I2S_FLAG_RXNE)==RESET);
    *(regbuf+i-1) = SPI_ReceiveData8(NFC_SPI);		 /* Read SPI2 received data */

    GPIO_SetBits(PORT_NSS, PIN_NSS);	//NSS = 1;

    return;
}

/*****************************************************************
* Function Name : Clear_FIFO
* Description   : SPI��FIFO
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void Clear_FIFO(void)
{
    u8 regdata;

    GetReg(JREG_FIFOLEVEL,&regdata);
    if((regdata & 0x7F) != 0)			//FIFO������գ���FLUSH FIFO
    {
        SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);
    }
    return ;
}

/*****************************************************************
* Function Name : FM175XX_HardReset
* Description   : FM175XX_Ӳ����λ
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void FM175XX_HardReset(void)
{
    GPIO_InitTypeDef   GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = PIN_NRST;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PORT_NRST, &GPIO_InitStructure);

    //GPIO_SetBits(PORT_NSS,PIN_NSS);//

    FM175XX_HPD(0);//NPD=0
//    mDelay(2);
	delay_us(2000);
    FM175XX_HPD(1);//NPD=1
	delay_us(2000);
//    mDelay(2);
    return;
}

/*****************************************************************
* Function Name : GetReg_Ext
* Description   : ��ȡ��չ�Ĵ���ֵ
* Input         : ExtRegAddr:��չ�Ĵ�����ַ   ExtRegData:��ȡ��ֵ
* Output        : None
* Notes         :
******************************************************************/
void GetReg_Ext(unsigned char ExtRegAddr,unsigned char* ExtRegData)
{
    SetReg(JREG_EXT_REG_ENTRANCE,JBIT_EXT_REG_RD_ADDR + ExtRegAddr);
    GetReg(JREG_EXT_REG_ENTRANCE,&(*ExtRegData));
    return;
}

/*****************************************************************
* Function Name : SetReg_Ext
* Description   : д��չ�Ĵ���
* Input         : ExtRegAddr:��չ�Ĵ�����ַ   ExtRegData:Ҫд���ֵ
* Output        : None
* Notes         :
******************************************************************/
void SetReg_Ext(unsigned char ExtRegAddr,unsigned char ExtRegData)
{
    SetReg(JREG_EXT_REG_ENTRANCE,JBIT_EXT_REG_WR_ADDR + ExtRegAddr);
    SetReg(JREG_EXT_REG_ENTRANCE,JBIT_EXT_REG_WR_DATA + ExtRegData);
    return;
}

/*****************************************************************
* Function Name : ModifyReg_Ext
* Description   : ��չ�Ĵ���λ����
* Input         : ExtRegAddr:Ŀ��Ĵ�����ַ   mask:Ҫ������λ
				: set:λ����ֵ  0:����  1����λ
* Output        : None
* Notes         :
******************************************************************/
void ModifyReg_Ext(unsigned char ExtRegAddr,unsigned char mask,unsigned char set)
{
    unsigned char regdata;

    GetReg_Ext(ExtRegAddr,&regdata);

    if(set)
    {
        regdata |= mask;
    }
    else
    {
        regdata &= ~(mask);
    }

    SetReg_Ext(ExtRegAddr,regdata);
    return;
}

/*****************************************************************
* Function Name : FM175XX_SoftReset
* Description   : FM175XX_�����λ
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
unsigned char FM175XX_SoftReset(void) //δʹ��
{
    unsigned char reg_data;
    SetReg(JREG_COMMAND,CMD_SOFT_RESET);
    mDelay(1);//FM175XXоƬ��λ��Ҫ1ms
    GetReg(JREG_COMMAND,&reg_data);
    if(reg_data == 0x20)
        return SUCCESS;
    else
        return ERROR;
}

//====================================================================================//

//==================================== The End===============================================/

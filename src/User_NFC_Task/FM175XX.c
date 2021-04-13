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
* Description   : ms延时函数 
* Input         : ms：延时毫秒数
* Output        : None
* Notes         :
******************************************************************/
void mDelay(u16 ms)
{
    SysTick->LOAD = 1000*8; 			//设置时间为1ms
    SysTick->VAL = 0x00;        	//清空计数器
    SysTick->CTRL = 0x01;      		//开始倒数

    while(ms)
    {
        while((SysTick->CTRL & 0x10000) == 0);
        ms--;
    }
    SysTick->CTRL=0x00;       //关闭计数器
    SysTick->VAL =0X00;       //清空计数器
}

/*****************************************************************
* Function Name : FM175XX_HPD
* Description   : 模式设置 
* Input         : mode：mode = 1 退出HPD模式 ，mode = 0 进入HPD模式
* Output        : None
* Notes         :
******************************************************************/
void FM175XX_HPD(unsigned char mode)
{
    if(mode == 0x0)
    {
        GPIO_ResetBits(PORT_NRST,PIN_NRST);//NPD = 0	进入HPD模式
    }
    else
    {
        GPIO_SetBits(PORT_NRST,PIN_NRST);	//NPD = 1 退出HPD模式
//        mDelay(1);
        delay_us(1000); //延时1ms，等待FM175XX启动
    }
    return;
}

/*****************************************************************
* Function Name : GetReg
* Description   : 读取寄存器值
* Input         : addr:目标寄存器地址   regdata:读取的值
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
* Description   : 写寄存器值
* Input         : addr:目标寄存器地址   regdata:要写入的值
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
* Description   : 修改寄存器的位
* Input         : addr:目标寄存器地址   mask:要改变的位
                : set:修改位的新值 0：清零 1：置位
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
* Description   : SPI接口连续写FIFO寄存器
* Input         : reglen：要写入的字节数
                : regbuf：要写入的数据buf地址
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
        SPI_SendData8(NFC_SPI,regdata);     /* Send addr_i i≥1*/
        while(SPI_I2S_GetFlagStatus(NFC_SPI, SPI_I2S_FLAG_RXNE) == RESET);
        regdata = SPI_ReceiveData8(NFC_SPI);/* not care data */
    }
    GPIO_SetBits(PORT_NSS,PIN_NSS);	        //NSS = 1;

    return;
}

/*****************************************************************
* Function Name : SPI_Read_FIFO
* Description   : SPI接口连续读FIFO
* Input         : reglen：要读出的字节数
                : regbuf：要读入的数据buf地址
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
* Description   : SPI清FIFO
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void Clear_FIFO(void)
{
    u8 regdata;

    GetReg(JREG_FIFOLEVEL,&regdata);
    if((regdata & 0x7F) != 0)			//FIFO如果不空，则FLUSH FIFO
    {
        SetReg(JREG_FIFOLEVEL,JBIT_FLUSHFIFO);
    }
    return ;
}

/*****************************************************************
* Function Name : FM175XX_HardReset
* Description   : FM175XX_硬件复位
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
* Description   : 读取扩展寄存器值
* Input         : ExtRegAddr:扩展寄存器地址   ExtRegData:读取的值
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
* Description   : 写扩展寄存器
* Input         : ExtRegAddr:扩展寄存器地址   ExtRegData:要写入的值
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
* Description   : 扩展寄存器位操作
* Input         : ExtRegAddr:目标寄存器地址   mask:要操作的位
				: set:位的新值  0:清零  1：置位
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
* Description   : FM175XX_软件复位
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
unsigned char FM175XX_SoftReset(void) //未使用
{
    unsigned char reg_data;
    SetReg(JREG_COMMAND,CMD_SOFT_RESET);
    mDelay(1);//FM175XX芯片复位需要1ms
    GetReg(JREG_COMMAND,&reg_data);
    if(reg_data == 0x20)
        return SUCCESS;
    else
        return ERROR;
}

//====================================================================================//

//==================================== The End===============================================/

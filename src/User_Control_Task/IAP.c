/******************** (C) COPYRIGHT 2020 Ninebot *******************************
* File Name     : IAP.c
* Author        : meihua
* Version       : V1.0.0
* Date          : 2020-11-13
* Description   : 
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "IAP.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
static const u8 check_code[16] __attribute__((at(0x8001100)))="Ebike_Booter_60k";

u32 g_FCtrlMap[FCTRL_MAP_LEN>>2] = {0};  		//内存控制表
u32 g_FCtrlMap_Default[FCTRL_MAP_LEN>>2] =
{
    0x505a,			//字头
    0,				//是否从临时程序区下载程序到正式程序区
    0,				//临时程序大小
};

u8 m_key[16] = {0};
u8 m_BinOldSliceForDecr[8] = {0};
u8 m_BinNewSliceForDecr[8] = {0};
u8 g_DecryptDataBuf[200]= {0};
_IAP_STATE_T iap_state;

/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/*****************************************************************
* Function Name : IAP_Reset
* Description   : IAP数据复位
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void IAP_Reset(void)
{
//    u8 key[16] = {0xfe,0x80,0x1c,0xb2,0xd1,0xef,0x41,0xa6,
//                  0xa4,0x17,0x31,0xf5,0xa0,0x68,0x24,0xf0
//                 };
    u8 key[16] = {0xfe,0x45,0x3a,0xb2,0xd1,0xef,0x41,0xa6,
                  0xa4,0x17,0x31,0xf5,0xa0,0x68,0x24,0xf0};   //Voi定制
        
    iap_state.m_bIAP_Busy = 0;
    iap_state.m_IAP_FileSize = 0;
    iap_state.m_IAPIndex = 0;
    iap_state.m_IAP_CRC = 0;
    iap_state.m_IAP_WritePos = 0;
    iap_state.m_bIAP_BinHeadisOK = 0;
    iap_state.m_bIAP_BinCheckCRCOK = 0;
    iap_state.m_bIAP_BinDownloadFinished = 0;
    memcpy(m_key, key, 16);		//设置初始密码
    memset(m_BinOldSliceForDecr, 0, 8);
    memset(m_BinNewSliceForDecr, 0, 8);
}

/*****************************************************************
* Function Name : IAP_SaveFCtrlmap
* Description   : IAP控制数据保存
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
s32 IAP_SaveFCtrlmap(void)
{
    s32 result = 0;
    Disable_Interrupt();   //关闭总中断
    result = Flash_Prepared(FADD_FCTRL_MAP, FCTRL_MAP_LEN);
    if(result)
        result = Flash_Write(FADD_FCTRL_MAP, (const u8*)g_FCtrlMap, FCTRL_MAP_LEN);
    Enable_Interrupt();		//开放总中断
    return result;
}

/*****************************************************************
* Function Name : IAP_Init
* Description   : IAP初始化
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void IAP_Init(void)
{
    IAP_Reset();
    //初始化flash控制表
    Flash_Read(FADD_FCTRL_MAP, (u8*)g_FCtrlMap, FCTRL_MAP_LEN);
    if(g_FCtrlMap[0] != g_FCtrlMap_Default[0])	 		//如果控制表不存在，则用缺省值填充
    {
        memcpy(g_FCtrlMap, g_FCtrlMap_Default, FCTRL_MAP_LEN);
        IAP_SaveFCtrlmap();
    }
}

/*****************************************************************
* Function Name : IAP_EraseSWFlash
* Description   : 擦除flash
* Input         : Address：擦除的起始地址 size：擦除的字节数
* Output        : result：擦除结果
* Notes         :
******************************************************************/
s32 IAP_EraseSWFlash(u32 Address, u32 size)
{
    s32 result = IAP_OK;

    __disable_irq();   //关闭总中断
    if(!Flash_Prepared(Address, size))
        result = IAPERROR_ERASE;
    __enable_irq();		//开放总中断

    return result;
}

/*****************************************************************
* Function Name : IAP_decrypt
* Description   : IAP数据解密
* Input         : pData：要解密的数据 key：密钥
* Output        : None
* Notes         :
******************************************************************/
void IAP_decrypt(u8 *pData, u8 *key)
{
    u32 y=0, z=0, sum=0xC6EF3720, i;
    u32 delta=0x9e3779b9; /* a key schedule constant */
    u32 a=0, b=0, c=0, d=0; /* cache key */

    memcpy(&y,&pData[0],4);
    memcpy(&z,&pData[4],4);

    memcpy(&a,&key[0],4);
    memcpy(&b,&key[4],4);
    memcpy(&c,&key[8],4);
    memcpy(&d,&key[12],4);
    for(i=0; i<32; i++)
    {
        /* basic cycle start */
        z -= ((y<<4) + c) ^ (y + sum) ^ ((y>>5) + d);
        y -= ((z<<4) + a) ^ (z + sum) ^ ((z>>5) + b);
        sum -= delta; /* end cycle */
    }

    memcpy(&pData[0],(u8*)&y,4);
    memcpy(&pData[4],(u8*)&z,4);

}

/*****************************************************************
* Function Name : IAP_encrypt
* Description   : IAP数据加密
* Input         : pData：要加密的数据 key：密钥
* Output        : None
* Notes         :
******************************************************************/
void IAP_encrypt(u8 *pData, u8 *key)
{
    u32 y=0, z=0, sum=0, i;
    u32 delta=0x9e3779b9; /* a key schedule constant */
    u32 a=0, b=0, c=0, d=0; /* cache key */

    memcpy(&y,&pData[0],4);
    memcpy(&z,&pData[4],4);

    memcpy(&a,&key[0],4);
    memcpy(&b,&key[4],4);
    memcpy(&c,&key[8],4);
    memcpy(&d,&key[12],4);

    for(i=0; i<32; i++)
    {
        /* basic cycle start */
        sum += delta;
        y += ((z<<4) + a) ^ (z + sum) ^ ((z>>5) + b);
        z += ((y<<4) + c) ^ (y + sum) ^ ((y>>5) + d);
        /* end cycle */
    }
    memcpy(&pData[0],(u8*)&y,4);
    memcpy(&pData[4],(u8*)&z,4);
}

/*****************************************************************
* Function Name : IAP_ParseFrame
* Description   : IAP数据解析及IAP执行
* Input         : pData：IAP数据 channel：通讯通道
* Output        : None
* Notes         :
******************************************************************/
void IAP_ParseFrame(u8* pData, u8 channel)
{
    u8 Index = pData[FRM_INDEX];
    s32 datalen = pData[FRM_LEN];
    u32 temp32 = 0;
	u32 s_TopStack_Temp;
    int i = 0;
    u8 j = 0;
    u8 result = IAP_OK;

    _IAP_STATE_T *p_iap = & iap_state;

    if(p_iap ->m_bIAP_Busy)	  	//防止重复进入
    {
        result = IAPERROR_BUSY;
        PushMiniFrame(MY_ID, pData[FRM_SRC_ID], 0, CMD_IAP_ACK, result, 0, channel);
        p_iap ->m_bIAP_Busy = 0;
        return;
    }

    p_iap ->m_bIAP_Busy = 1;

    switch(pData[FRM_CMD])
    {
    case CMD_IAP_BEGIN:		//开始IAP命令，传入固件大小，并准备flash空间（这里还要将Flash标志清零！）
        memcpy(&temp32, &pData[FRM_DATA], 4);	//固件大小
        IAP_Reset();
       
        if(datalen != 4)
        {
            result = IAPERROR_FORM;
        }
        else
        {
            if(temp32 > 0 && temp32 <= MAX_IAPSW_SIZE && (temp32%8) == 0 )	//新固件大小必须为8的倍数
            {
				g_FCtrlMap[FCTRL_DOWNLOAD] = 0;	 		//下载固件标志清零（新加）
				if(!IAP_SaveFCtrlmap())							//保存标志到Flash中（新加）
				{
					result = IAPERROR_WRITEFLASH;
				}
				else
				{
					p_iap ->m_IAP_FileSize = temp32;
					result = IAP_EraseSWFlash(FADD_TEMP_SW, p_iap ->m_IAP_FileSize+128);	  	//擦除flash，这里多准备128字节空间
				}
            }
            else
            {
                result = IAPERROR_SIZE;
            }
        }
        break;
    case CMD_IAP_WR:				//写入Flash命令
        if(p_iap ->m_IAP_FileSize == 0)	//固件大小是否正确
        {
            result = IAPERROR_OTHER;
            break;
        }
        if(p_iap ->m_IAPIndex == Index)   //内部索引正确
        {
            if(datalen%8 != 0 || datalen == 0)  //每帧的固件数据长度都必须是8的整数倍
            {
                result = IAPERROR_SIZE;
                break;
            }
            //数据解密
            for(i=0; i<datalen; i+=8)  //一次解密8字节
            {
                memcpy(m_BinOldSliceForDecr, m_BinNewSliceForDecr, 8);	   	//取出上次保存的密文
                memcpy(m_BinNewSliceForDecr, &pData[FRM_DATA+i], 8);		//保存当前待解密的8字节密文用以对后8字节数据解密
                IAP_decrypt((u8*)&pData[FRM_DATA+i], (u8*)m_key);  		//进行数据解密，解密后的数据保存在原数组中
                if(!(p_iap ->m_IAP_WritePos == 0 && i == 0))		//不是第一组8字节数据，解密后与前一组8位密文进行亦或得到真正的原文
                {
                    for(j=0; j<8; j++)
                        pData[FRM_DATA+i+j] = pData[FRM_DATA+i+j]^m_BinOldSliceForDecr[j];
                }
            }
			memcpy(g_DecryptDataBuf, pData, datalen);
            //发送的是第一帧数据，校验bin文件头格式
            if(Index == 0 && p_iap ->m_IAP_WritePos == 0)
            {
				 //检查栈顶地址是否合法 是否在0x2000 0000 - 0x2000 2000之间
				memcpy((u8*)&s_TopStack_Temp,&pData[FRM_DATA],4);
				if ((s_TopStack_Temp & 0x2FFE0000 ) == 0x20000000)  //这里判断固件首格式不同的芯片有所不同
				{
						p_iap ->m_bIAP_BinHeadisOK = 1;    //格式正确
				}								
				else
				{
						result = IAPERROR_FORM;
						break;
				}
            }
						
			if(Index == 2 && p_iap -> m_IAP_WritePos == 0x100) //第三帧数据的前16字节
			{
					for(i=0; i<16; i++)
					{
							if(g_DecryptDataBuf[FRM_DATA+i] != check_code[i])
							{
									result = IAPERROR_FORM;
									break;
							}
					}
			}

            //写入的数据是否超范围
            if(p_iap ->m_IAP_WritePos + datalen >= (p_iap ->m_IAP_FileSize+128))  	//最多在固件后拼接127个0
            {
                result = IAPERROR_SIZE;		//要写入的数据长度超范围
                break;
            }
            //固件头部格式是否验证过
            if(!p_iap ->m_bIAP_BinHeadisOK)
            {
                result = IAPERROR_FORM;
                break;
            }
            //验证完毕，写入Flash
            if(p_iap ->m_bIAP_BinDownloadFinished)
                break;

			Disable_Interrupt();   //关闭总中断
					
			if(!Flash_Write_Word(FADD_TEMP_SW + p_iap ->m_IAP_WritePos, &pData[FRM_DATA], datalen))
//			if(!Flash_Write(FADD_TEMP_SW + p_iap ->m_IAP_WritePos, &pData[IAP_FRM_DATA], datalen))
			{
					result = IAPERROR_WRITEFLASH;	 	//写入falsh失败
			}
							
						
            if(result == IAP_OK )   //数据写入正确，累加校验和
            {
                p_iap ->m_IAPIndex++;			  	//每收到一帧数据索引自动加1
//				
//				if((p_iap ->m_IAPIndex>=16)&&(channel == CAN_SEL))
//				{
//				   p_iap ->m_IAPIndex = 0;
//				}
				
                for(i=0; i<datalen; i+=4)
                {
//                    temp32 = *((u32*)&pData[IAP_FRM_DATA+i]);
					memcpy(&temp32, &pData[FRM_DATA+i], 4);
                    if(p_iap ->m_IAP_WritePos < p_iap ->m_IAP_FileSize-4)    //m_IAP_FileSize-4是需要校验的真实的固件大小
                    {
                        p_iap ->m_IAP_CRC += temp32;	 	//累加校验和
                        p_iap ->m_IAP_WritePos += 4;
                    }
                    else    //固件已经下载完成，进行内部校验
                    {
                        p_iap ->m_IAP_CRC = ~((p_iap ->m_IAP_CRC<<16)|(p_iap ->m_IAP_CRC>>16));   //高低16位互换后取反
                        p_iap ->m_bIAP_BinDownloadFinished = 1;
                        if(p_iap ->m_IAP_CRC == temp32)
                            p_iap ->m_bIAP_BinCheckCRCOK = 1;
                        else
                            result = IAPERROR_CRC;
                        break;   //无论校验是否成功，都退出for循环
                    }
                }
                if((p_iap ->m_IAP_WritePos & 0x000003FF) == 0)		//每1K数据后，改变解密密码
                {
                    for(i=0; i<16; i++)
                        m_key[i] += i;
                }   // */
            }
			Enable_Interrupt();		//开放总中断
        }
        else if(p_iap ->m_IAPIndex != Index)	//允许重发机制，如果发的是上一帧的索引，则忽略内容直接返回，否则报错
        {
            result = IAPERROR_INDEX;
        }
        break;
    case CMD_IAP_CRC:		//原来的校验命令，固件内部已经完成校验，这里不再校验
        if(datalen != 4 || !p_iap ->m_bIAP_BinDownloadFinished || !p_iap ->m_bIAP_BinCheckCRCOK)		//帧长度是否正确
        {
            result = IAPERROR_FORM;
            break;
        }
		g_FCtrlMap[FCTRL_TEMPSW_SIZE] = p_iap ->m_IAP_FileSize;		//固件大小
		g_FCtrlMap[FCTRL_DOWNLOAD] = 1;	 		//更新固件标志
		if(!IAP_SaveFCtrlmap())				//标志保存标志到Flash中
		{
			result = IAPERROR_WRITEFLASH;
		}
		else	  			//固件下载成功，重置标志
		{
			IAP_Reset();
		}
        break;
    case CMD_IAP_RESET:					//系统复位命令
		if(g_FCtrlMap[FCTRL_DOWNLOAD] && !p_iap ->m_bIAP_BinCheckCRCOK)		//程序下载完毕，且标志已重置（说明标志保存成功）
		{
			if(g_bool[B_BGN_LOOP])
			{
			  g_bool[B_RESET] = 1; 		//重启
			}
			else
			  NVIC_SystemReset();
		}

        break;
    default:
        result = IAPERROR_FORM;
        break;
    }
    if(result != IAP_OK)
    {
        IAP_Reset();
    }

    PushMiniFrame(pData[FRM_DST_ID], pData[FRM_SRC_ID], 0, CMD_IAP_ACK, result, 0, channel);
    p_iap ->m_bIAP_Busy = 0;
}

//====================================================================================//

//==================================== The End===============================================/

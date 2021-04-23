#include "iic.h"

//IO操作函数
#define IIC_SCL_0   GPIO_ResetBits(GPIOB, GPIO_Pin_0)
#define IIC_SCL_1   GPIO_SetBits(GPIOB, GPIO_Pin_0)
#define IIC_SDA_0   GPIO_ResetBits(GPIOB, GPIO_Pin_1)
#define IIC_SDA_1   GPIO_SetBits(GPIOB, GPIO_Pin_1)

#define READ_SDA    GPIOB->IDR  & GPIO_Pin_1   //输入SDA  

#define SDA_IN()   {GPIOB->MODER &= ~GPIO_MODER_MODER1;}
#define SDA_OUT()  {GPIOB->MODER &= ~GPIO_MODER_MODER1; GPIOB->MODER |= GPIO_MODER_MODER1_0;}    

_IIC_OB  iic_t;

/**@brief     Function for send start iic signal.
 * @details    
* @param[in]: iic_state
 */
//产生IIC起始信号
static void IIC_Start(_IIC_STATE_T *iic_state)
{
    switch(iic_state->I2C_step.start_status)
    {
    case 0:
        SDA_OUT();          //sda线输出
        IIC_SDA_1;
        IIC_SCL_1;
        iic_state->I2C_step.start_status = 1; //等待延时
        break;
    case 1:
        IIC_SDA_0;
        iic_state->I2C_step.start_status = 2; //等待延时 when CLK is high,DATA change form high to low
        break;
    case 2:
        iic_state->I2C_step.start_status = 3; //多等待125us
        break;
    case 3:
        IIC_SCL_0;//钳住I2C总线，准备发送或接收数据
        iic_state->I2C_step.start_status = 0;  //复位
        if(iic_t.iic_state.Lock_app_status == sending_status)
        {
            iic_state->I2c_dispose_status = SEND_ADDR_STATE;
        }
        else
        {
            iic_state->I2c_dispose_status = SEND_DATA_STATE;
        }
        break;
    default:
        iic_state->I2C_step.start_status = 0;  //复位
        break;

    }
}

/**@brief     Function for send stop iic signal.
 * @details    
* @param[in]: iic_state
 */
//产生IIC停止信号
static void IIC_Stop(_IIC_STATE_T *iic_state)
{
    switch(iic_state->I2C_step.stop_status)
    {
    case 0:
        SDA_OUT();//sda线输出
        IIC_SCL_0;
        IIC_SDA_0;//STOP:when CLK is high DATA change form low to high
        iic_state->I2C_step.stop_status = 1;  //延时
        break;
    case 1:
        IIC_SCL_1;
        IIC_SDA_1;//发送I2C总线结束信号

        iic_state->I2C_step.start_status = 0;                      //发送启动过程中的状态         三级
        iic_state->I2C_step.send_status = 0;                       //主机写从机时的写状态         三级
        iic_state->I2C_step.wait_ack_status = 0;                    //主机写从机时的等待ACK状态   三级
        iic_state->I2C_step.stop_status = 0;                       //发送停止信号状态             三级
        iic_state->I2C_step.read_status = 0;                       //主机读从机时的读状态         三级
        iic_state->I2C_step.ack_nack_status = 0;                   //主机读从机时的响应ACK或nack状态   三级

        iic_state->I2c_dispose_status = START_STATE;
    
        if(iic_state->Lock_app_status == set_data_command)
        {
            iic_state->Lock_app_status = sending_status;            //发送数据内容
        }
        else if(iic_state->Lock_app_status == sending_status)
        {
            iic_state->Lock_app_status = set_display_command;      //发送命令2
        }
        else
        {
            iic_state->Lock_app_status = IDEL_status;  //空闲状态
        }
        break;
    default:
        iic_state->I2C_step.stop_status = 0;
        break;
    }
}


/**@brief     Function for wait receive iic signal.
 * @details    
* @param[in]: iic_state
 */
//等待应答信号到来
static void IIC_Wait_Ack(_IIC_STATE_T *iic_state)
{
    static u8 timeout = 0;
    switch(iic_state->I2C_step.wait_ack_status)
    {
    case 0:
        SDA_IN();      //SDA设置为输入
        //IIC_SDA_1;
        iic_state->I2C_step.wait_ack_status = 1;  //延时
        break;
    case 1:
        IIC_SCL_1;
        iic_state->I2C_step.wait_ack_status = 2;  //延时  此时 从机收到上升沿进中断发ACK
        break;
    case 2:
        if(READ_SDA && (++timeout > 10))  //没收到ACK
        {
            timeout = 0;
            iic_state->I2C_step.wait_ack_status = 0;
            iic_state->I2c_dispose_status = STOP_STATE;  //进入STOP模式
        }
        else   //收到ACK
        {
            timeout = 0;
            IIC_SCL_0;
            iic_state->I2C_step.wait_ack_status = 0;
            
            if(iic_state->Lock_app_status == sending_status)
                iic_state->I2c_dispose_status = SEND_DATA_STATE;   //发送数据
            else
                iic_state->I2c_dispose_status = STOP_STATE;   //读数据
        }
        break;
    default:
        iic_state-> I2C_step.wait_ack_status = 0;
        break;
    }
}

/**@brief     Function for send replay .
 * @details 
 * @param[in]  iic_state
 */
//产生ACK应答    主机读从机时
static void IIC_Ack(_IIC_STATE_T *iic_state)
{
    switch(iic_state->I2C_step.ack_nack_status)
    {
    case 0:
        IIC_SCL_0;
        SDA_OUT();
        IIC_SDA_0;
        iic_state->I2C_step.ack_nack_status = 1;
        break;
    case 1:
        IIC_SCL_1;
        iic_state->I2C_step.ack_nack_status = 2;  //上升沿 延时 等待从机接收
        break;
    case 2:
        IIC_SCL_0;
        iic_state->I2C_step.ack_nack_status = 0;
        iic_state->I2c_dispose_status = READ_DATA_STATE; //读数据   //读下一个数据
        break;
    default:
        iic_state->I2C_step.ack_nack_status = 0;
        break;
    }
}

/**@brief     Function for No replay .
 * @details 
 * @param[in]  iic_state
 */
//不产生ACK应答		 主机读从机时
static void IIC_NAck(_IIC_STATE_T *iic_state)
{
    switch(iic_state->I2C_step.ack_nack_status)
    {
    case 0:
        IIC_SCL_0;
        SDA_OUT();
        IIC_SDA_1;
        iic_state->I2C_step.ack_nack_status = 1;
        break;
    case 1:
        IIC_SCL_1;
        iic_state->I2C_step.ack_nack_status = 2;  //上升沿 延时 等待从机接收
        break;
    case 2:
        IIC_SCL_0;
        iic_state->I2C_step.ack_nack_status = 0;
        iic_state->I2c_dispose_status = READ_DATA_STATE; //读数据   //读下一个数据
        break;
    default:
        iic_state->I2C_step.ack_nack_status = 0;
        break;
    }
}


/**@brief     Function for send 1 byte by iic.
 * @details 
 * @param[in]  txd 
 * @param[in]  iic_state
 */
//IIC发送一个字节
void IIC_Send_Byte(u8 txd,_IIC_STATE_T *iic_state)
{
    static u8 s_uCount,s_uTemp;
    switch(iic_state->I2C_step.send_status)
    {
    case 0:
        s_uCount = 0;               //清空
        SDA_OUT();
        IIC_SCL_0;                  //拉低时钟开始数据传输
        iic_state->I2C_step.send_status = 1;
        break;
    case 1:
        s_uTemp = (txd >> s_uCount);
    
        if((s_uTemp & 0x01) == 0x01)
            IIC_SDA_1;
        else
            IIC_SDA_0;
//        if((s_uTemp&0x80)>>7)
//            IIC_SDA_1;
//        else
//            IIC_SDA_0;
        s_uCount++;
        iic_state->I2C_step.send_status = 2; //延时
        break;
    case 2:
        IIC_SCL_1;
        iic_state->I2C_step.send_status = 3;   //延时  等待SLAV进入中断
        break;
    case 3:
        IIC_SCL_0;
        iic_state->I2C_step.send_status = 1;  //延时
        if(s_uCount >= 8) //发了8个bit 一个字节
        {
            iic_state->I2C_step.send_status = 0;
            iic_state->I2c_dispose_status = READ_ACK_STATE;  //发送完8个bit ，等ACK信号
        }
        break;
    default:
        iic_state->I2C_step.send_status = 0;
        break;
    }
}


/**@brief     Function for read 1 byte by iic.
* @details  
* @param[in]  *buf : container
* @param[in]  iic_state
 */
//读1个字节
static void  IIC_Read_Byte(u8 *buf,_IIC_STATE_T *iic_state)
{
    static u8 s_uCount,s_uReceive = 0;
    switch(iic_state->I2C_step.read_status)
    {
    case 0:
        s_uCount = 0;
        s_uReceive = 0;
        SDA_IN();//SDA设置为输入
        iic_state->I2C_step.read_status = 1;
        break;
    case 1:
        IIC_SCL_0;
        iic_state->I2C_step.read_status = 2; //延时
        break;
    case 2:
        IIC_SCL_1;
        iic_state->I2C_step.read_status = 3; //延时  上升沿 等待从机 响应发数据
        break;
    case 3:
        s_uCount++;
        s_uReceive <<= 1;
        if(READ_SDA) s_uReceive++;
        iic_state->I2C_step.read_status = 1;
        if(s_uCount >= 8)	 //读了8个bit
        {
            s_uCount = 0;
            iic_state->I2C_step.read_status = 0;
            *buf = s_uReceive;                    // 数据保存
            iic_state->I2c_dispose_status = WRITE_ACK_STATE; 	  //发送ACK 或非ACK
        }
        break;
    default:
        iic_state->I2C_step.read_status = 0;
        break;
    }
}


/**@brief     Function for read string by iic.
*@details  
*@param[in]  addr : read addr
*@param[in]  len  : data length
*@param[in]  *buf : data
*@param[in]  iic_state : iic state
*/

//===========读一串数据========================================
static void IIC_RED_Bytes(u8 addr,u8 len,u8 *buf,_IIC_STATE_T *iic_state )
{
    static u8 s_uData_num=0;
    switch(iic_state->I2c_dispose_status)
    {
    case START_STATE:
        s_uData_num = 0;
        IIC_Start(iic_state);
        break;
    case SEND_ADDR_STATE:
        IIC_Send_Byte((addr|0x01),iic_state);
        break;
    case READ_ACK_STATE:
        IIC_Wait_Ack(iic_state);
        break;
    case WRITE_ACK_STATE://回复ACK 或nack命令
        if(s_uData_num == len) IIC_NAck(iic_state);  //最后一个字节发完，
        else IIC_Ack(iic_state);
        break;
    case READ_DATA_STATE:
        if(s_uData_num >= len)
        {
            s_uData_num = 0;  //读完16个/8个数据
            iic_state->I2c_dispose_status = STOP_STATE;
            iic_state->s_uI2c_read_ok = I2C_READ_OK;	    //==============置接收成功标志位
        }
        else
        {
            IIC_Read_Byte(buf+s_uData_num,iic_state);
            if(iic_state->I2c_dispose_status == WRITE_ACK_STATE) //进入8次，一个字节读完
            {
                s_uData_num++;
            }
        }
        break;

    case STOP_STATE :
        IIC_Stop(iic_state);
        break;

    default:
        iic_state->I2c_dispose_status = START_STATE;
        iic_state->Lock_app_status = IDEL_status;
        break;
    }
}

/**@brief     Function for send string by iic.
*@details  
*@param[in]  addr : send addr
*@param[in]  len  : data length
*@param[in]  *buf : data
*@param[in]  iic_state : iic state
*/
//==========写一串数据==================================
static void IIC_Send_Bytes(u8 addr, u8 len, u8 *buf, _IIC_STATE_T *iic_state)
{
    static u8 s_uData_num = 0;
    switch(iic_state->I2c_dispose_status)
    {
    case START_STATE:
        s_uData_num = 0;
        IIC_Start(iic_state);
        break;
    case  SEND_ADDR_STATE:
        IIC_Send_Byte((addr|0x00),iic_state);
        break;
    case READ_ACK_STATE:
        IIC_Wait_Ack(iic_state);
        break;
    case SEND_DATA_STATE:
        if(s_uData_num >= len)
        {
            s_uData_num = 0;  //发送完16个数据
            iic_state->I2c_dispose_status = STOP_STATE;
            iic_state->g_uI2c_send_ok = I2C_SEND_OK;	    //==============置发送成功标志位
        }
        else
        {
            IIC_Send_Byte(*(buf+s_uData_num), iic_state);
            if(iic_state->I2c_dispose_status == READ_ACK_STATE) //进入8次，一个字节发送完
            {
                s_uData_num++;
            }
        }
        break;
    case STOP_STATE :
        IIC_Stop(iic_state);
        break;

    default:
        iic_state->I2c_dispose_status = START_STATE;
        iic_state->Lock_app_status =  IDEL_status;
        break;
    }
}

/**@brief     Function for send a command by iic.
*@details  
*@param[in]  command : send command
*@param[in]  iic_state : iic state
*/
//==========写一串数据==================================
static void IIC_Send_Command(u8 command, _IIC_STATE_T *iic_state)
{
    switch(iic_state->I2c_dispose_status)
    {
    case START_STATE:
        IIC_Start(iic_state);
        break;
    case READ_ACK_STATE:
        IIC_Wait_Ack(iic_state);
        break;
    case SEND_DATA_STATE:
        IIC_Send_Byte(command, iic_state);
        break;
    case STOP_STATE :
        IIC_Stop(iic_state);
        break;

    default:
        iic_state->I2c_dispose_status = START_STATE;
        iic_state->Lock_app_status =  IDEL_status;
        break;
    }
}

/**@brief     Function for Init iic gpio.
 * @details    
* @param[in]  
 */
//初始化IIC     PB0 SCL ; PB1 SDA
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;       //输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1); 
}

/**@brief     Function for iic process.
 * @details    
* @param[in]  iic state
 */
void IicProcess(void)
{
    switch(iic_t.iic_state.Lock_app_status)
    {
    case IDEL_status:			                                             	//空闲状态
        break;

    case sending_status:  
        //iic_t.iic_state.Lock_app_status = IDEL_status;
        IIC_Send_Bytes(iic_t.s_uWrite_slave_address, iic_t.s_uWrite_slave_len, iic_t.g_uI2c_write_buf, &iic_t.iic_state);
        break;
    case set_data_command:
        IIC_Send_Command(iic_t.command1, &iic_t.iic_state);
        break;
    case set_display_command:
        IIC_Send_Command(iic_t.command2, &iic_t.iic_state);
        break;
    default:
        iic_t.iic_state.Lock_app_status = IDEL_status;
        break;
    }
}

u8 IIC_PushFrame(u8 *pdata)
{
    u8 ret = 0;
    
    if( iic_t.iic_state.Lock_app_status == IDEL_status )
    {
        iic_t.command1 = 0x40;
        memcpy(iic_t.g_uI2c_write_buf, pdata, 7);
        iic_t.s_uWrite_slave_len = 6;
        iic_t.s_uWrite_slave_address = 0xc0;
        iic_t.command2 = 0x88 | 7;

        iic_t.iic_state.Lock_app_status = set_data_command;

        ret = 1;
    }
    
    return ret;
}

I2C_APP IicGetAppStatus(void)
{
    return iic_t.iic_state.Lock_app_status;
}

#include "unlockiic.h"
#include "IAP.h"
#include "LED_Others.h"

//======================IAP功能======================================================

u8 g_encrypt_key[16] = {0xfe,0x80,0x1c,0xb2,0xd1,0xef,0x41,0xa6,0xa4,0x17,0x31,0xf5,0xa0,0x68,0x24,0xf0};     //主机发送加密秘钥(对应从机接收解密秘钥)
u8 g_decrypt_key[16] = {'S','e','g','w','a','y','-','N','i','n','e','b','o','t','-','1'};                     //主机读 解密秘钥(对应从机发送加密秘钥


/**@brief     define battery lock object.
 */
_BATTERY_LOCK_OB _battery_lock_object;

/**@brief     Function for Init iic gpio.
 * @details
* @param[in]
 */
//初始化IIC     PB8 SCL ; PA12 SDA
static void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;   //推挽输出
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SCL_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(SCL_GPIO,SCL_PIN); 	//PA7, SCL

    GPIO_InitStructure.GPIO_Pin = SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;   //推挽输出
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SDA_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(SDA_GPIO, SDA_PIN); 	//PA6, SDA
}

static void SDA_IN(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  GPIO_InitStructure.GPIO_Pin = SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;   //浮空输入
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SDA_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(SDA_GPIO, SDA_PIN); 	//PA6, SDA
}

static void SDA_OUT(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  GPIO_InitStructure.GPIO_Pin = SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;   //推挽输出
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SDA_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(SDA_GPIO, SDA_PIN); 	//PA6, SDA
}

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
        DIO_H_BATLOCK;
        CLK_H_BATLOCK;
        iic_state->I2C_step.start_status = 1; //等待延时
        break;
    case 1:
        DIO_L_BATLOCK;
        iic_state->I2C_step.start_status = 2; //等待延时 when CLK is high,DATA change form high to low
        break;
    case 2:
        iic_state->I2C_step.start_status = 3; //多等待125us
        break;
    case 3:
        CLK_L_BATLOCK;//钳住I2C总线，准备发送或接收数据
        iic_state->I2C_step.start_status = 0;  //复位
        iic_state->I2c_dispose_status = SEND_ADDR_STATE;
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
        CLK_L_BATLOCK;
        DIO_L_BATLOCK;//STOP:when CLK is high DATA change form low to high
        iic_state->I2C_step.stop_status = 1;  //延时
        break;
    case 1:
        CLK_H_BATLOCK;
        DIO_H_BATLOCK;//发送I2C总线结束信号

        iic_state->I2C_step.start_status = 0;                      //发送启动过程中的状态         三级
        iic_state->I2C_step.send_status = 0;                       //主机写从机时的写状态         三级
        iic_state->I2C_step.wait_ack_status = 0;                    //主机写从机时的等待ACK状态   三级
        iic_state->I2C_step.stop_status = 0;                       //发送停止信号状态             三级
        iic_state->I2C_step.read_status = 0;                       //主机读从机时的读状态         三级
        iic_state->I2C_step.ack_nack_status = 0;                   //主机读从机时的响应ACK或nack状态   三级

        iic_state->I2c_dispose_status = START_STATE;
        iic_state->Lock_app_status = IDEL_status;  //空闲状态
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
    switch(iic_state->I2C_step.wait_ack_status)
    {
    case 0:
        SDA_IN();      //SDA设置为输入
        DIO_H_BATLOCK;
        iic_state->I2C_step.wait_ack_status = 1;  //延时
        break;
    case 1:
        CLK_H_BATLOCK;
        iic_state->I2C_step.wait_ack_status = 2;  //延时  此时 从机收到上升沿进中断发ACK
        break;
    case 2:
        if(DIO_read_BATLOCK)  //没收到ACK
        {
            iic_state->I2C_step.wait_ack_status = 0;
            iic_state->I2c_dispose_status = STOP_STATE;  //进入STOP模式
        }
        else   //收到ACK
        {
            CLK_L_BATLOCK;
            iic_state->I2C_step.wait_ack_status = 0;
            if(iic_state->Lock_app_status == sending_status)
                iic_state->I2c_dispose_status = SEND_DATA_STATE;   //发送数据
            else
                iic_state->I2c_dispose_status = READ_DATA_STATE;   //读数据
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
        CLK_L_BATLOCK;
        SDA_OUT();
        DIO_L_BATLOCK;
        iic_state->I2C_step.ack_nack_status = 1;
        break;
    case 1:
        CLK_H_BATLOCK;
        iic_state->I2C_step.ack_nack_status = 2;  //上升沿 延时 等待从机接收
        break;
    case 2:
        CLK_L_BATLOCK;
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
        CLK_L_BATLOCK;
        SDA_OUT();
        DIO_H_BATLOCK;
        iic_state->I2C_step.ack_nack_status = 1;
        break;
    case 1:
        CLK_H_BATLOCK;
        iic_state->I2C_step.ack_nack_status = 2;  //上升沿 延时 等待从机接收
        break;
    case 2:
        CLK_L_BATLOCK;
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
        s_uCount = 0;             //清空
        SDA_OUT();
        CLK_L_BATLOCK;       //拉低时钟开始数据传输
        iic_state->I2C_step.send_status = 1;
        break;
    case 1:
        s_uTemp = (txd<<s_uCount);
        if((s_uTemp&0x80)>>7)
            DIO_H_BATLOCK;
        else
            DIO_L_BATLOCK;
        s_uCount++;
        iic_state->I2C_step.send_status = 2; //延时
        break;
    case 2:
        CLK_H_BATLOCK;
        iic_state->I2C_step.send_status = 3;   //延时  等待SLAV进入中断
        break;
    case 3:
        CLK_L_BATLOCK;
        iic_state->I2C_step.send_status = 1;  //延时
        if(s_uCount>=8) //发了8个bit 一个字节
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
void  IIC_Read_Byte(u8 *buf,_IIC_STATE_T *iic_state)
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
        CLK_L_BATLOCK;
        iic_state->I2C_step.read_status = 2; //延时
        break;
    case 2:
        CLK_H_BATLOCK;
        iic_state->I2C_step.read_status = 3; //延时  上升沿 等待从机 响应发数据
        break;
    case 3:
        s_uCount++;
        s_uReceive <<= 1;
        if(DIO_read_BATLOCK) s_uReceive++;
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
void IIC_RED_Bytes(u8 addr,u8 len,u8 *buf,_IIC_STATE_T *iic_state )
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
void IIC_Send_Bytes(u8 addr,u8 len,u8 *buf,_IIC_STATE_T *iic_state)
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
            IIC_Send_Byte(*(buf+s_uData_num),iic_state);
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



/**@brief     Function for iic process.
 * @details
* @param[in]  iic state
 */
//==============================================================================================
//===================放入FOC中断中125us进入一次(一次执行时间最大不超过1us) ==============
void IicProcess(void)
{
    _BATTERY_LOCK_OB *p_block = &_battery_lock_object;
	
    switch(p_block->iic_t.iic_state.Lock_app_status)
    {
    case IDEL_status:			                                             	//空闲状态
        break;

    case sending_status:       
        IIC_Send_Bytes(p_block->iic_t.s_uWrite_slave_regnum,p_block->iic_t.s_uWrite_slave_data_len,p_block->iic_t.g_uI2c_write_buf,&p_block->iic_t.iic_state);    //主机写从机函数,开锁命令
        break;

    case reading_status:
        IIC_RED_Bytes(p_block->iic_t.s_uRead_slave_regnum,p_block->iic_t.s_uread_slave_data_len,p_block->iic_t.g_uI2c_read_buf,&p_block->iic_t.iic_state);
        //主机读从机 1内存区的16个 数据 (序列号、硬件软件版本号等)
        //主机读从机 2内存去的8个数据 锁位置状态 、
        //主机读从机16个字节  (判断其更新固件是否写入FLSH OK)
        break;

    default:
        p_block->iic_t.iic_state.Lock_app_status = IDEL_status;
        break;

    }
}

/**@brief     Function for update battery lock comming event.
 * @details   This function will be called by BattLockProcess.
* @param[in]  _BATTERY_LOCK_OB *p_block : battery_lock_object.
 */
static void Btlock_event_update(_BATTERY_LOCK_OB *p_block)
{
    if(p_block->iic_t.s_uGet_lock_ID == NULL)
    {
        p_block->s_lock_event =  B_READ_VERSION;
        //return;
    }
    if(p_block->s_set_client_id_flag == 1)
    {
        p_block->s_set_client_id_flag = 0;
        p_block->s_obtain_lock_info_swtich = SWITCH_ON;  //开启定时
        p_block->s_obtain_lock_info_period = 1400;   //即发生开锁命令后，xs内持续更新锁状态
        p_block->s_lock_event = B_SET_ID;
        return;    
    }
   // if(g_NBCmdMap[NB_CTL_IOT_UNLOCK] == 1 || g_bool[B_FACTORY_TEST_MODE] == 1)    //注 ：在Global.h文件中 g_CmdMapWR  0x8e设置成可读写
    if((LockSet.params.Coverlock_UnLock)    //注 ：在Global.h文件中 g_CmdMapWR  0x8e设置成可读写
       && (p_block->s_lock_event == B_NULL_STATE)) 
    {
        LockSet.params.Coverlock_UnLock = 0;
//        Beep_SendCmd(MINI_BEEP_CMD_ONCE_L);    //仪表响一声
        p_block->iic_t.s_uUnlock_cmd = GET_UNLOCK_CMD;   //获得开锁命令
        p_block->s_obtain_lock_info_swtich = SWITCH_ON;  //开启定时
        p_block->s_obtain_lock_info_period = 1400;   //即发生开锁命令后，xs内持续更新锁状态
        p_block->s_lock_event = B_UNLOCK_ACT;
        return;
    }

    if(p_block->iic_t.iic_state.s_uI2c_read_ok == I2C_READ_OK )  //若I2C读到数据
    {
        p_block->s_lock_event = B_BT_INFO_ANYS;
        return;

    }

    if(p_block->s_obtain_lock_info_swtich == SWITCH_ON)
    {
        p_block->s_lock_event = B_INQUIRE_ACT;
        //p_block->s_lock_timeout = 0;
        return;
    }


    if(p_block->s_lock_timeout >= 30*200) 
    {
        p_block->s_lock_event = B_INQUIRE_ACT;
        p_block->s_lock_timeout = 0;
    }
}

/**@brief     Function for unlock battery lock.
 * @details   This function will be called by BattLockProcess.
* @param[in]  _BATTERY_LOCK_OB *p_block : battery_lock_object.
 */
static void Unlock_btlock_process(_BATTERY_LOCK_OB *p_block)
{
    u8 i = 0;
    u16 u16CRC;

    if((p_block->iic_t.s_uGet_lock_ID == GET_LOCK_ID)&&(p_block->iic_t.s_Lock_ID_match == 1) && (p_block->s_lock_timeout >20))          //若已获得 锁CHIPID 且 锁的用户识别码 匹配
    {
        p_block->s_lock_timeout =0;
        p_block->iic_t.s_uUnlock_cmd = NULL;		 //清空开锁命令标志
        /*经测试 SysTick->VAL 该条函数 0x18=24个     24/8=3 us		*/
        memcpy(p_block->iic_t.g_uI2c_write_buf,p_block->iic_t.s_uI2c_chip_id,sizeof(p_block->iic_t.s_uI2c_chip_id));    //锁的CHIP ID 	12个字节
        p_block->iic_t.g_uI2c_write_buf[12] = lowByte(MASTER_ID);
        p_block->iic_t.g_uI2c_write_buf[13] = highByte(MASTER_ID);	   //用户ID密码		2字节
        u16CRC = 0xFFFF;
        for (i = 0; i < 14; i++)     //(16-2)个字节
        {
            u16CRC = crc16_update(u16CRC, p_block->iic_t.g_uI2c_write_buf[i]);
        }
        p_block->iic_t.g_uI2c_write_buf[14] = highByte(u16CRC);
        p_block->iic_t.g_uI2c_write_buf[15] = lowByte(u16CRC);
        p_block->iic_t.I2C_encrypt((u8*)&p_block->iic_t.g_uI2c_write_buf[0], (u8*)g_encrypt_key);        //加密低8字节  经测试  该条函数 0x7F=127个    127/8=16us
        p_block->iic_t.I2C_encrypt((u8*)&p_block->iic_t.g_uI2c_write_buf[8], (u8*)g_encrypt_key);  	    //加密高8字节  经测试  该条函数 0x7F=127个    127/8=16us
        p_block->iic_t.s_uWrite_slave_regnum = SEND_ADDR1;
		    p_block->iic_t.s_uWrite_slave_data_len = SEND_ADDR1_LEN;
        p_block->iic_t.iic_state.Lock_app_status = sending_status;	 //开启发送
    }
}

/**@brief     Function for set battery lock client id.
 * @details   This function will be called by BattLockProcess.
* @param[in]  _BATTERY_LOCK_OB *p_block : battery_lock_object.
 */
static void Set_Client_ID(_BATTERY_LOCK_OB *p_block)
{
        u8 i = 0;
        u16 u16CRC;    
        p_block->iic_t.g_uI2c_write_buf[0] = 0x5A;
        p_block->iic_t.g_uI2c_write_buf[1] = 0xA5;
        p_block->iic_t.g_uI2c_write_buf[2] = p_block->s_temp_id>>8;//原id
        p_block->iic_t.g_uI2c_write_buf[3] = p_block->s_temp_id;
        p_block->iic_t.g_uI2c_write_buf[4] = MASTER_ID;//新id
        p_block->iic_t.g_uI2c_write_buf[5] = MASTER_ID>>8;

        u16CRC = 0xFFFF;
        for (i = 0; i < 6; i++) 
        {
            u16CRC = crc16_update(u16CRC, p_block->iic_t.g_uI2c_write_buf[i]);
        }
        p_block->iic_t.g_uI2c_write_buf[6] = highByte(u16CRC);
        p_block->iic_t.g_uI2c_write_buf[7] = lowByte(u16CRC);
        p_block->iic_t.I2C_encrypt((u8*)&p_block->iic_t.g_uI2c_write_buf[0], (u8*)g_encrypt_key);        //加密低8字节  经测试  该条函数 0x7F=127个    127/8=16us
        p_block->iic_t.s_uWrite_slave_regnum = SEND_ADDR2;
		    p_block->iic_t.s_uWrite_slave_data_len = SEND_ADDR2_LEN;
        p_block->iic_t.iic_state.Lock_app_status = sending_status;	 //开启发送
}



/**@brief     Function for update battery lock current status.
 * @details   This function will be called by BattLockProcess.
* @param[in]  _BATTERY_LOCK_OB *p_block : battery_lock_object.
 */
static void Obtain_btlock_update_state_process(_BATTERY_LOCK_OB *p_block)   //查询状态 1s或者30s
{
    if((p_block->s_lock_timeout %200) == 0)
    {
        p_block->iic_t.s_uRead_slave_regnum = READ_ADDR2;       //读寄存器2 的信息
        p_block->iic_t.s_uread_slave_data_len = READ_ADDR2_LEN;
        p_block->iic_t.iic_state.Lock_app_status = reading_status;		   //启动读
        p_block->s_lock_commu_cnt++;                         //????+1
    }
}

/**@brief     Function for get battery lock init information.
 * @details   This function will be called by BattLockProcess.
* @param[in]  _BATTERY_LOCK_OB *p_block : battery_lock_object.
 */
static void Obtain_btlock_Init_info(_BATTERY_LOCK_OB *p_block)      //获取版本
{
    if((p_block->s_lock_timeout %200) == 0)
    {
        p_block->iic_t.s_uRead_slave_regnum = READ_ADDR1;        //读寄存器1地址
        p_block->iic_t.s_uread_slave_data_len = READ_ADDR1_LEN;
        p_block->iic_t.iic_state.Lock_app_status = reading_status;		    //启动读
        p_block->s_lock_commu_cnt++;
        if( p_block->s_lock_commu_cnt > 5)
        {
            p_block->iic_t.s_uGet_lock_ID =0xFF;
        }
        p_block->s_lock_event = B_NULL_STATE;
    }
}

/**@brief     Function for battery error status.
 * @details   This function will be called by BattLockProcess.
* @param[in]  _BATTERY_LOCK_OB *p_block : battery_lock_object.
 */
static void btlock_IDEL_process(_BATTERY_LOCK_OB *p_block)
{
    if(p_block->s_lock_commu_cnt >= 5)                           //锁通讯故障 连续5次    20190807增加
    {
        p_block->s_lock_commu_cnt = 0;
//        SetErrorByte(MINI_LOCK_ERROR_COM);
        p_block->s_obtain_lock_info_swtich = SWITCH_ON;  //开启定时
        p_block->s_obtain_lock_info_period = 1400;   //即发生开锁命令后，xs内持续更新锁状态
    }
}
/**@brief     Function for analysis battery lock information.
 * @details   This function will be called by BattLockProcess.
* @param[in]  _BATTERY_LOCK_OB *p_block : battery_lock_object.
 */
static void Analysis_bt_lock_info(_BATTERY_LOCK_OB *p_block)
{
	
    static u8 s_uLock_ack_num = 0;
    u8 i = 0;
    u16 u16CRC;

    p_block->iic_t.iic_state.s_uI2c_read_ok = NULL;

    p_block->s_lock_commu_cnt = 0 ; //清空通讯计数
    ClearErrorByte(MINI_LOCK_ERROR_COM);      //清锁与主控通讯故障

    if(p_block->iic_t.s_uGet_lock_ID == NULL)  //若未获得 锁的 版本信息
    {
        p_block->iic_t.I2C_decrypt(&p_block->iic_t.g_uI2c_read_buf[0], g_decrypt_key);
        p_block->iic_t.I2C_decrypt(&p_block->iic_t.g_uI2c_read_buf[8], g_decrypt_key);  //解密
//			  g_NBCmdMap[NB_SCOOTER_LOCK] = p_block->iic_t.g_uI2c_read_buf[0] ;
        u16CRC = 0xFFFF;
        for (i = 0; i < READ_ADDR1_LEN-2; i++)     //(16-2)个字节
        {
            u16CRC = crc16_update(u16CRC, p_block->iic_t.g_uI2c_read_buf[i]);
        }
        if ((p_block->iic_t.g_uI2c_read_buf[READ_ADDR1_LEN-2] == highByte(u16CRC))
                &&(p_block->iic_t.g_uI2c_read_buf[READ_ADDR1_LEN-1] == lowByte(u16CRC))) //CRC校验正确
        {
            memcpy(p_block->iic_t.s_uI2c_chip_id,p_block->iic_t.g_uI2c_read_buf,CHIP_ID_LENG);    //注：锁的软件版本号预留，
            VER_BMSLOCK = p_block->iic_t.g_uI2c_read_buf[13]+(u16)(p_block->iic_t.g_uI2c_read_buf[12]<<8);
            p_block->iic_t.s_uGet_lock_ID = GET_LOCK_ID;                           //获得 LOCK从机的 CHIPID 版本号等值
            p_block->s_obtain_lock_info_swtich = SWITCH_ON;           //锁或主控上电第一次后 需要读。
            p_block->s_obtain_lock_info_period = 1400;
        }
    }
    else
    {
        p_block->iic_t.I2C_decrypt(&p_block->iic_t.g_uI2c_read_buf[0], g_decrypt_key);
			  
//			  g_NBCmdMap[NB_SCOOTER_LOCK] = p_block->iic_t.g_uI2c_read_buf[0] ;
        u16CRC = 0xFFFF;
        for (i = 0; i < READ_ADDR2_LEN-2; i++)     //(8-2)个字节
        {
            u16CRC = crc16_update(u16CRC, p_block->iic_t.g_uI2c_read_buf[i]);
        }
        if ((p_block->iic_t.g_uI2c_read_buf[READ_ADDR2_LEN-2] == highByte(u16CRC))
                &&(p_block->iic_t.g_uI2c_read_buf[READ_ADDR2_LEN-1] == lowByte(u16CRC))) //CRC校验正确
        {

            if(((p_block->iic_t.g_uI2c_read_buf[0]&0x01) == 0x01)&&((p_block->iic_t.g_uI2c_read_buf[0]&0x04) == 0x00))		 //若锁已开，且开锁过程已结束
            {
//                if( g_bool[B_FACTORY_TEST_MODE] == 1)
//                    ClearErrorByte(MINI_LOCK_OPENED_STATE);
//                SetErrorByte(MINI_LOCK_OPENED_STATE);
                p_block->s_obtain_lock_info_swtich = SWITCH_ON;           //锁或主控上电第一次后 需要读。
                p_block->s_obtain_lock_info_period = 1400;
            }
            else if((p_block->iic_t.g_uI2c_read_buf[0]&0x01) == 0x00)  //已上锁
            {
                ClearErrorByte(MINI_LOCK_OPENED_STATE);      //清锁开报警标志
            }

            if (((MASTER_ID&0xFF) != p_block->iic_t.g_uI2c_read_buf[1])||(((MASTER_ID>>8)&0xFF) != p_block->iic_t.g_uI2c_read_buf[2]))
            {
                if((VER_BMSLOCK == (p_block->iic_t.g_uI2c_read_buf[2] + (p_block->iic_t.g_uI2c_read_buf[1]<<8)))
                    && p_block->s_set_client_id_cnt < 5)           
                {
                    p_block->s_temp_id =  (p_block->iic_t.g_uI2c_read_buf[1] << 8) + p_block->iic_t.g_uI2c_read_buf[2];                   
                    p_block->s_set_client_id_flag = 1;
                    p_block->s_set_client_id_cnt++;
                }
                else
                {               
//                    SetErrorByte(MINI_LOCK_PANEL_NOT_MATCH);              //主控与锁不匹配
                    p_block->iic_t.s_Lock_ID_match = 0;                                  //用户ID密码不匹配
                    p_block->iic_t.g_uI2c_read_buf[0] |= 0x20;                           //用户ID不匹配
                }
                p_block->s_obtain_lock_info_swtich = SWITCH_ON;
                p_block->s_obtain_lock_info_period = 1400;
            }
            else
            {
                ClearErrorByte(MINI_LOCK_PANEL_NOT_MATCH);
                p_block->iic_t.s_Lock_ID_match = 1;                                   //用户ID密码匹配
                p_block->iic_t.g_uI2c_read_buf[0] &= 0xDF;
            }

            s_uLock_ack_num++;                                       //读的信息NUM更新  以此判断是否通讯故障
            LockSet_State[Coverlock] = p_block->iic_t.g_uI2c_read_buf[0]+(u16)(s_uLock_ack_num<<8);      //锁的信息 反馈到该内存区
        }
    }
    memset(p_block->iic_t.g_uI2c_read_buf,0,sizeof(p_block->iic_t.g_uI2c_read_buf));    //清空接收
    p_block->s_lock_event = B_NULL_STATE;
}

//1.初始化读取锁id及版本号
//2.开锁
//3.解析锁信息
//4.查询锁状态
//5.报错状态

void (*btlock_work_p[PRO_NM])(_BATTERY_LOCK_OB *p_block) = {Obtain_btlock_Init_info,Set_Client_ID,Unlock_btlock_process,Analysis_bt_lock_info,Obtain_btlock_update_state_process,btlock_IDEL_process};

/**@brief     Function for  battery lock work process.
 * @details   This function will be called by main 5ms period.
* @param[in]  elected_version : identify current hardware version.
 */
static void BattLockProcess(void)  //每隔5ms 进入一次(最大执行时间40us),16字节按照127us的需要传输 23ms
{
    _BATTERY_LOCK_OB *p_block = &_battery_lock_object;
    Btlock_event_update(p_block);
	
    if(( p_block->iic_t.iic_state.Lock_app_status == IDEL_status )&&(g_bool[B_IN_IAPMODE] != 1))  //空闲状态下且不在升级状态
    {
        if(p_block->s_obtain_lock_info_swtich == SWITCH_ON)
        {
            if(p_block->s_obtain_lock_info_period > 0)
            {
                p_block->s_obtain_lock_info_period--;
            }
            else
            {
                p_block->s_obtain_lock_info_swtich = SWITCH_OFF;
            }
        }
        p_block->s_lock_timeout++;
        btlock_work_p[p_block->s_lock_event](p_block); //线程
    }
}

/**@brief     Function for battery lock object init.
* @details    This function will be called by InitAllPeripherals init.
* @param[in]  _BATTERY_LOCK_OB *p_block : battery_lock_object.
*/
void _Battery_Lock_Object_Malloc(_BATTERY_LOCK_OB *p_this)
{
//#ifndef DEBUGGER_FLASH
    p_this ->battery_lock_pro  = BattLockProcess;
//#endif
    p_this ->iic_dispose_pro   = IicProcess;
    p_this ->iic_t.I2C_decrypt = IAP_decrypt;
    p_this ->iic_t.I2C_encrypt = IAP_encrypt;
    //p_this ->iic_t.I2C_IAP_Reset = IAP_Reset;
    p_this ->iic_t.iic_init    = IIC_Init;
    p_this ->iic_t.iic_init();
}



//==============================================================================================
//=====================CRC校验============================================

/** @ingroup util_crc16
    Processor-independent CRC-16 calculation.
    Polynomial: x^16 + x^15 + x^2 + 1 (0xA001)<br>
    Initial value: 0xFFFF
*/
u16 crc16_update(u16 crc, u8 a)
{
    u8 i;
    crc ^= a;
    for (i = 0; i < 8; ++i)
    {
        if (crc & 1)
            crc = (crc >> 1) ^ 0xA001;
        else
            crc = (crc >> 1);
    }

    return crc;
}

//=======低8位===================
u8 lowByte(u16 ww)
{
    return (uint8_t) ((ww) & 0x00FF);
}

//========高8位===========
u8 highByte(u16 ww)
{
    return (uint8_t) ((ww) >> 8);
}


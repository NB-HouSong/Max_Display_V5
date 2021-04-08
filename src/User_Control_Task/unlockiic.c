#include "unlockiic.h"
#include "IAP.h"
#include "LED_Others.h"

//======================IAP����======================================================

u8 g_encrypt_key[16] = {0xfe,0x80,0x1c,0xb2,0xd1,0xef,0x41,0xa6,0xa4,0x17,0x31,0xf5,0xa0,0x68,0x24,0xf0};     //�������ͼ�����Կ(��Ӧ�ӻ����ս�����Կ)
u8 g_decrypt_key[16] = {'S','e','g','w','a','y','-','N','i','n','e','b','o','t','-','1'};                     //������ ������Կ(��Ӧ�ӻ����ͼ�����Կ


/**@brief     define battery lock object.
 */
_BATTERY_LOCK_OB _battery_lock_object;

/**@brief     Function for Init iic gpio.
 * @details
* @param[in]
 */
//��ʼ��IIC     PB8 SCL ; PA12 SDA
static void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;   //�������
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SCL_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(SCL_GPIO,SCL_PIN); 	//PA7, SCL

    GPIO_InitStructure.GPIO_Pin = SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;   //�������
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
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;   //��������
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
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;   //�������
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
//����IIC��ʼ�ź�
static void IIC_Start(_IIC_STATE_T *iic_state)
{
    switch(iic_state->I2C_step.start_status)
    {
    case 0:
        SDA_OUT();          //sda�����
        DIO_H_BATLOCK;
        CLK_H_BATLOCK;
        iic_state->I2C_step.start_status = 1; //�ȴ���ʱ
        break;
    case 1:
        DIO_L_BATLOCK;
        iic_state->I2C_step.start_status = 2; //�ȴ���ʱ when CLK is high,DATA change form high to low
        break;
    case 2:
        iic_state->I2C_step.start_status = 3; //��ȴ�125us
        break;
    case 3:
        CLK_L_BATLOCK;//ǯסI2C���ߣ�׼�����ͻ��������
        iic_state->I2C_step.start_status = 0;  //��λ
        iic_state->I2c_dispose_status = SEND_ADDR_STATE;
        break;
    default:
        iic_state->I2C_step.start_status = 0;  //��λ
        break;
    }
}

/**@brief     Function for send stop iic signal.
 * @details
* @param[in]: iic_state
 */
//����IICֹͣ�ź�
static void IIC_Stop(_IIC_STATE_T *iic_state)
{
    switch(iic_state->I2C_step.stop_status)
    {
    case 0:
        SDA_OUT();//sda�����
        CLK_L_BATLOCK;
        DIO_L_BATLOCK;//STOP:when CLK is high DATA change form low to high
        iic_state->I2C_step.stop_status = 1;  //��ʱ
        break;
    case 1:
        CLK_H_BATLOCK;
        DIO_H_BATLOCK;//����I2C���߽����ź�

        iic_state->I2C_step.start_status = 0;                      //�������������е�״̬         ����
        iic_state->I2C_step.send_status = 0;                       //����д�ӻ�ʱ��д״̬         ����
        iic_state->I2C_step.wait_ack_status = 0;                    //����д�ӻ�ʱ�ĵȴ�ACK״̬   ����
        iic_state->I2C_step.stop_status = 0;                       //����ֹͣ�ź�״̬             ����
        iic_state->I2C_step.read_status = 0;                       //�������ӻ�ʱ�Ķ�״̬         ����
        iic_state->I2C_step.ack_nack_status = 0;                   //�������ӻ�ʱ����ӦACK��nack״̬   ����

        iic_state->I2c_dispose_status = START_STATE;
        iic_state->Lock_app_status = IDEL_status;  //����״̬
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
//�ȴ�Ӧ���źŵ���
static void IIC_Wait_Ack(_IIC_STATE_T *iic_state)
{
    switch(iic_state->I2C_step.wait_ack_status)
    {
    case 0:
        SDA_IN();      //SDA����Ϊ����
        DIO_H_BATLOCK;
        iic_state->I2C_step.wait_ack_status = 1;  //��ʱ
        break;
    case 1:
        CLK_H_BATLOCK;
        iic_state->I2C_step.wait_ack_status = 2;  //��ʱ  ��ʱ �ӻ��յ������ؽ��жϷ�ACK
        break;
    case 2:
        if(DIO_read_BATLOCK)  //û�յ�ACK
        {
            iic_state->I2C_step.wait_ack_status = 0;
            iic_state->I2c_dispose_status = STOP_STATE;  //����STOPģʽ
        }
        else   //�յ�ACK
        {
            CLK_L_BATLOCK;
            iic_state->I2C_step.wait_ack_status = 0;
            if(iic_state->Lock_app_status == sending_status)
                iic_state->I2c_dispose_status = SEND_DATA_STATE;   //��������
            else
                iic_state->I2c_dispose_status = READ_DATA_STATE;   //������
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
//����ACKӦ��    �������ӻ�ʱ
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
        iic_state->I2C_step.ack_nack_status = 2;  //������ ��ʱ �ȴ��ӻ�����
        break;
    case 2:
        CLK_L_BATLOCK;
        iic_state->I2C_step.ack_nack_status = 0;
        iic_state->I2c_dispose_status = READ_DATA_STATE; //������   //����һ������
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
//������ACKӦ��		 �������ӻ�ʱ
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
        iic_state->I2C_step.ack_nack_status = 2;  //������ ��ʱ �ȴ��ӻ�����
        break;
    case 2:
        CLK_L_BATLOCK;
        iic_state->I2C_step.ack_nack_status = 0;
        iic_state->I2c_dispose_status = READ_DATA_STATE; //������   //����һ������
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
//IIC����һ���ֽ�
void IIC_Send_Byte(u8 txd,_IIC_STATE_T *iic_state)
{
    static u8 s_uCount,s_uTemp;
    switch(iic_state->I2C_step.send_status)
    {
    case 0:
        s_uCount = 0;             //���
        SDA_OUT();
        CLK_L_BATLOCK;       //����ʱ�ӿ�ʼ���ݴ���
        iic_state->I2C_step.send_status = 1;
        break;
    case 1:
        s_uTemp = (txd<<s_uCount);
        if((s_uTemp&0x80)>>7)
            DIO_H_BATLOCK;
        else
            DIO_L_BATLOCK;
        s_uCount++;
        iic_state->I2C_step.send_status = 2; //��ʱ
        break;
    case 2:
        CLK_H_BATLOCK;
        iic_state->I2C_step.send_status = 3;   //��ʱ  �ȴ�SLAV�����ж�
        break;
    case 3:
        CLK_L_BATLOCK;
        iic_state->I2C_step.send_status = 1;  //��ʱ
        if(s_uCount>=8) //����8��bit һ���ֽ�
        {
            iic_state->I2C_step.send_status = 0;
            iic_state->I2c_dispose_status = READ_ACK_STATE;  //������8��bit ����ACK�ź�
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
//��1���ֽ�
void  IIC_Read_Byte(u8 *buf,_IIC_STATE_T *iic_state)
{
    static u8 s_uCount,s_uReceive = 0;
    switch(iic_state->I2C_step.read_status)
    {
    case 0:
        s_uCount = 0;
        s_uReceive = 0;
        SDA_IN();//SDA����Ϊ����
        iic_state->I2C_step.read_status = 1;
        break;
    case 1:
        CLK_L_BATLOCK;
        iic_state->I2C_step.read_status = 2; //��ʱ
        break;
    case 2:
        CLK_H_BATLOCK;
        iic_state->I2C_step.read_status = 3; //��ʱ  ������ �ȴ��ӻ� ��Ӧ������
        break;
    case 3:
        s_uCount++;
        s_uReceive <<= 1;
        if(DIO_read_BATLOCK) s_uReceive++;
        iic_state->I2C_step.read_status = 1;
        if(s_uCount >= 8)	 //����8��bit
        {
            s_uCount = 0;
            iic_state->I2C_step.read_status = 0;
            *buf = s_uReceive;                    // ���ݱ���
            iic_state->I2c_dispose_status = WRITE_ACK_STATE; 	  //����ACK ���ACK
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

//===========��һ������========================================
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
    case WRITE_ACK_STATE://�ظ�ACK ��nack����
        if(s_uData_num == len) IIC_NAck(iic_state);  //���һ���ֽڷ��꣬
        else IIC_Ack(iic_state);
        break;
    case READ_DATA_STATE:
        if(s_uData_num >= len)
        {
            s_uData_num = 0;  //����16��/8������
            iic_state->I2c_dispose_status = STOP_STATE;
            iic_state->s_uI2c_read_ok = I2C_READ_OK;	    //==============�ý��ճɹ���־λ
        }
        else
        {
            IIC_Read_Byte(buf+s_uData_num,iic_state);
            if(iic_state->I2c_dispose_status == WRITE_ACK_STATE) //����8�Σ�һ���ֽڶ���
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
//==========дһ������==================================
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
            s_uData_num = 0;  //������16������
            iic_state->I2c_dispose_status = STOP_STATE;
            iic_state->g_uI2c_send_ok = I2C_SEND_OK;	    //==============�÷��ͳɹ���־λ
        }
        else
        {
            IIC_Send_Byte(*(buf+s_uData_num),iic_state);
            if(iic_state->I2c_dispose_status == READ_ACK_STATE) //����8�Σ�һ���ֽڷ�����
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
//===================����FOC�ж���125us����һ��(һ��ִ��ʱ����󲻳���1us) ==============
void IicProcess(void)
{
    _BATTERY_LOCK_OB *p_block = &_battery_lock_object;
	
    switch(p_block->iic_t.iic_state.Lock_app_status)
    {
    case IDEL_status:			                                             	//����״̬
        break;

    case sending_status:       
        IIC_Send_Bytes(p_block->iic_t.s_uWrite_slave_regnum,p_block->iic_t.s_uWrite_slave_data_len,p_block->iic_t.g_uI2c_write_buf,&p_block->iic_t.iic_state);    //����д�ӻ�����,��������
        break;

    case reading_status:
        IIC_RED_Bytes(p_block->iic_t.s_uRead_slave_regnum,p_block->iic_t.s_uread_slave_data_len,p_block->iic_t.g_uI2c_read_buf,&p_block->iic_t.iic_state);
        //�������ӻ� 1�ڴ�����16�� ���� (���кš�Ӳ������汾�ŵ�)
        //�������ӻ� 2�ڴ�ȥ��8������ ��λ��״̬ ��
        //�������ӻ�16���ֽ�  (�ж�����¹̼��Ƿ�д��FLSH OK)
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
        p_block->s_obtain_lock_info_swtich = SWITCH_ON;  //������ʱ
        p_block->s_obtain_lock_info_period = 1400;   //���������������xs�ڳ���������״̬
        p_block->s_lock_event = B_SET_ID;
        return;    
    }
   // if(g_NBCmdMap[NB_CTL_IOT_UNLOCK] == 1 || g_bool[B_FACTORY_TEST_MODE] == 1)    //ע ����Global.h�ļ��� g_CmdMapWR  0x8e���óɿɶ�д
    if((LockSet.params.Coverlock_UnLock)    //ע ����Global.h�ļ��� g_CmdMapWR  0x8e���óɿɶ�д
       && (p_block->s_lock_event == B_NULL_STATE)) 
    {
        LockSet.params.Coverlock_UnLock = 0;
//        Beep_SendCmd(MINI_BEEP_CMD_ONCE_L);    //�Ǳ���һ��
        p_block->iic_t.s_uUnlock_cmd = GET_UNLOCK_CMD;   //��ÿ�������
        p_block->s_obtain_lock_info_swtich = SWITCH_ON;  //������ʱ
        p_block->s_obtain_lock_info_period = 1400;   //���������������xs�ڳ���������״̬
        p_block->s_lock_event = B_UNLOCK_ACT;
        return;
    }

    if(p_block->iic_t.iic_state.s_uI2c_read_ok == I2C_READ_OK )  //��I2C��������
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

    if((p_block->iic_t.s_uGet_lock_ID == GET_LOCK_ID)&&(p_block->iic_t.s_Lock_ID_match == 1) && (p_block->s_lock_timeout >20))          //���ѻ�� ��CHIPID �� �����û�ʶ���� ƥ��
    {
        p_block->s_lock_timeout =0;
        p_block->iic_t.s_uUnlock_cmd = NULL;		 //��տ��������־
        /*������ SysTick->VAL �������� 0x18=24��     24/8=3 us		*/
        memcpy(p_block->iic_t.g_uI2c_write_buf,p_block->iic_t.s_uI2c_chip_id,sizeof(p_block->iic_t.s_uI2c_chip_id));    //����CHIP ID 	12���ֽ�
        p_block->iic_t.g_uI2c_write_buf[12] = lowByte(MASTER_ID);
        p_block->iic_t.g_uI2c_write_buf[13] = highByte(MASTER_ID);	   //�û�ID����		2�ֽ�
        u16CRC = 0xFFFF;
        for (i = 0; i < 14; i++)     //(16-2)���ֽ�
        {
            u16CRC = crc16_update(u16CRC, p_block->iic_t.g_uI2c_write_buf[i]);
        }
        p_block->iic_t.g_uI2c_write_buf[14] = highByte(u16CRC);
        p_block->iic_t.g_uI2c_write_buf[15] = lowByte(u16CRC);
        p_block->iic_t.I2C_encrypt((u8*)&p_block->iic_t.g_uI2c_write_buf[0], (u8*)g_encrypt_key);        //���ܵ�8�ֽ�  ������  �������� 0x7F=127��    127/8=16us
        p_block->iic_t.I2C_encrypt((u8*)&p_block->iic_t.g_uI2c_write_buf[8], (u8*)g_encrypt_key);  	    //���ܸ�8�ֽ�  ������  �������� 0x7F=127��    127/8=16us
        p_block->iic_t.s_uWrite_slave_regnum = SEND_ADDR1;
		    p_block->iic_t.s_uWrite_slave_data_len = SEND_ADDR1_LEN;
        p_block->iic_t.iic_state.Lock_app_status = sending_status;	 //��������
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
        p_block->iic_t.g_uI2c_write_buf[2] = p_block->s_temp_id>>8;//ԭid
        p_block->iic_t.g_uI2c_write_buf[3] = p_block->s_temp_id;
        p_block->iic_t.g_uI2c_write_buf[4] = MASTER_ID;//��id
        p_block->iic_t.g_uI2c_write_buf[5] = MASTER_ID>>8;

        u16CRC = 0xFFFF;
        for (i = 0; i < 6; i++) 
        {
            u16CRC = crc16_update(u16CRC, p_block->iic_t.g_uI2c_write_buf[i]);
        }
        p_block->iic_t.g_uI2c_write_buf[6] = highByte(u16CRC);
        p_block->iic_t.g_uI2c_write_buf[7] = lowByte(u16CRC);
        p_block->iic_t.I2C_encrypt((u8*)&p_block->iic_t.g_uI2c_write_buf[0], (u8*)g_encrypt_key);        //���ܵ�8�ֽ�  ������  �������� 0x7F=127��    127/8=16us
        p_block->iic_t.s_uWrite_slave_regnum = SEND_ADDR2;
		    p_block->iic_t.s_uWrite_slave_data_len = SEND_ADDR2_LEN;
        p_block->iic_t.iic_state.Lock_app_status = sending_status;	 //��������
}



/**@brief     Function for update battery lock current status.
 * @details   This function will be called by BattLockProcess.
* @param[in]  _BATTERY_LOCK_OB *p_block : battery_lock_object.
 */
static void Obtain_btlock_update_state_process(_BATTERY_LOCK_OB *p_block)   //��ѯ״̬ 1s����30s
{
    if((p_block->s_lock_timeout %200) == 0)
    {
        p_block->iic_t.s_uRead_slave_regnum = READ_ADDR2;       //���Ĵ���2 ����Ϣ
        p_block->iic_t.s_uread_slave_data_len = READ_ADDR2_LEN;
        p_block->iic_t.iic_state.Lock_app_status = reading_status;		   //������
        p_block->s_lock_commu_cnt++;                         //????+1
    }
}

/**@brief     Function for get battery lock init information.
 * @details   This function will be called by BattLockProcess.
* @param[in]  _BATTERY_LOCK_OB *p_block : battery_lock_object.
 */
static void Obtain_btlock_Init_info(_BATTERY_LOCK_OB *p_block)      //��ȡ�汾
{
    if((p_block->s_lock_timeout %200) == 0)
    {
        p_block->iic_t.s_uRead_slave_regnum = READ_ADDR1;        //���Ĵ���1��ַ
        p_block->iic_t.s_uread_slave_data_len = READ_ADDR1_LEN;
        p_block->iic_t.iic_state.Lock_app_status = reading_status;		    //������
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
    if(p_block->s_lock_commu_cnt >= 5)                           //��ͨѶ���� ����5��    20190807����
    {
        p_block->s_lock_commu_cnt = 0;
//        SetErrorByte(MINI_LOCK_ERROR_COM);
        p_block->s_obtain_lock_info_swtich = SWITCH_ON;  //������ʱ
        p_block->s_obtain_lock_info_period = 1400;   //���������������xs�ڳ���������״̬
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

    p_block->s_lock_commu_cnt = 0 ; //���ͨѶ����
    ClearErrorByte(MINI_LOCK_ERROR_COM);      //����������ͨѶ����

    if(p_block->iic_t.s_uGet_lock_ID == NULL)  //��δ��� ���� �汾��Ϣ
    {
        p_block->iic_t.I2C_decrypt(&p_block->iic_t.g_uI2c_read_buf[0], g_decrypt_key);
        p_block->iic_t.I2C_decrypt(&p_block->iic_t.g_uI2c_read_buf[8], g_decrypt_key);  //����
//			  g_NBCmdMap[NB_SCOOTER_LOCK] = p_block->iic_t.g_uI2c_read_buf[0] ;
        u16CRC = 0xFFFF;
        for (i = 0; i < READ_ADDR1_LEN-2; i++)     //(16-2)���ֽ�
        {
            u16CRC = crc16_update(u16CRC, p_block->iic_t.g_uI2c_read_buf[i]);
        }
        if ((p_block->iic_t.g_uI2c_read_buf[READ_ADDR1_LEN-2] == highByte(u16CRC))
                &&(p_block->iic_t.g_uI2c_read_buf[READ_ADDR1_LEN-1] == lowByte(u16CRC))) //CRCУ����ȷ
        {
            memcpy(p_block->iic_t.s_uI2c_chip_id,p_block->iic_t.g_uI2c_read_buf,CHIP_ID_LENG);    //ע����������汾��Ԥ����
            VER_BMSLOCK = p_block->iic_t.g_uI2c_read_buf[13]+(u16)(p_block->iic_t.g_uI2c_read_buf[12]<<8);
            p_block->iic_t.s_uGet_lock_ID = GET_LOCK_ID;                           //��� LOCK�ӻ��� CHIPID �汾�ŵ�ֵ
            p_block->s_obtain_lock_info_swtich = SWITCH_ON;           //���������ϵ��һ�κ� ��Ҫ����
            p_block->s_obtain_lock_info_period = 1400;
        }
    }
    else
    {
        p_block->iic_t.I2C_decrypt(&p_block->iic_t.g_uI2c_read_buf[0], g_decrypt_key);
			  
//			  g_NBCmdMap[NB_SCOOTER_LOCK] = p_block->iic_t.g_uI2c_read_buf[0] ;
        u16CRC = 0xFFFF;
        for (i = 0; i < READ_ADDR2_LEN-2; i++)     //(8-2)���ֽ�
        {
            u16CRC = crc16_update(u16CRC, p_block->iic_t.g_uI2c_read_buf[i]);
        }
        if ((p_block->iic_t.g_uI2c_read_buf[READ_ADDR2_LEN-2] == highByte(u16CRC))
                &&(p_block->iic_t.g_uI2c_read_buf[READ_ADDR2_LEN-1] == lowByte(u16CRC))) //CRCУ����ȷ
        {

            if(((p_block->iic_t.g_uI2c_read_buf[0]&0x01) == 0x01)&&((p_block->iic_t.g_uI2c_read_buf[0]&0x04) == 0x00))		 //�����ѿ����ҿ��������ѽ���
            {
//                if( g_bool[B_FACTORY_TEST_MODE] == 1)
//                    ClearErrorByte(MINI_LOCK_OPENED_STATE);
//                SetErrorByte(MINI_LOCK_OPENED_STATE);
                p_block->s_obtain_lock_info_swtich = SWITCH_ON;           //���������ϵ��һ�κ� ��Ҫ����
                p_block->s_obtain_lock_info_period = 1400;
            }
            else if((p_block->iic_t.g_uI2c_read_buf[0]&0x01) == 0x00)  //������
            {
                ClearErrorByte(MINI_LOCK_OPENED_STATE);      //������������־
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
//                    SetErrorByte(MINI_LOCK_PANEL_NOT_MATCH);              //����������ƥ��
                    p_block->iic_t.s_Lock_ID_match = 0;                                  //�û�ID���벻ƥ��
                    p_block->iic_t.g_uI2c_read_buf[0] |= 0x20;                           //�û�ID��ƥ��
                }
                p_block->s_obtain_lock_info_swtich = SWITCH_ON;
                p_block->s_obtain_lock_info_period = 1400;
            }
            else
            {
                ClearErrorByte(MINI_LOCK_PANEL_NOT_MATCH);
                p_block->iic_t.s_Lock_ID_match = 1;                                   //�û�ID����ƥ��
                p_block->iic_t.g_uI2c_read_buf[0] &= 0xDF;
            }

            s_uLock_ack_num++;                                       //������ϢNUM����  �Դ��ж��Ƿ�ͨѶ����
            LockSet_State[Coverlock] = p_block->iic_t.g_uI2c_read_buf[0]+(u16)(s_uLock_ack_num<<8);      //������Ϣ ���������ڴ���
        }
    }
    memset(p_block->iic_t.g_uI2c_read_buf,0,sizeof(p_block->iic_t.g_uI2c_read_buf));    //��ս���
    p_block->s_lock_event = B_NULL_STATE;
}

//1.��ʼ����ȡ��id���汾��
//2.����
//3.��������Ϣ
//4.��ѯ��״̬
//5.����״̬

void (*btlock_work_p[PRO_NM])(_BATTERY_LOCK_OB *p_block) = {Obtain_btlock_Init_info,Set_Client_ID,Unlock_btlock_process,Analysis_bt_lock_info,Obtain_btlock_update_state_process,btlock_IDEL_process};

/**@brief     Function for  battery lock work process.
 * @details   This function will be called by main 5ms period.
* @param[in]  elected_version : identify current hardware version.
 */
static void BattLockProcess(void)  //ÿ��5ms ����һ��(���ִ��ʱ��40us),16�ֽڰ���127us����Ҫ���� 23ms
{
    _BATTERY_LOCK_OB *p_block = &_battery_lock_object;
    Btlock_event_update(p_block);
	
    if(( p_block->iic_t.iic_state.Lock_app_status == IDEL_status )&&(g_bool[B_IN_IAPMODE] != 1))  //����״̬���Ҳ�������״̬
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
        btlock_work_p[p_block->s_lock_event](p_block); //�߳�
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
//=====================CRCУ��============================================

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

//=======��8λ===================
u8 lowByte(u16 ww)
{
    return (uint8_t) ((ww) & 0x00FF);
}

//========��8λ===========
u8 highByte(u16 ww)
{
    return (uint8_t) ((ww) >> 8);
}


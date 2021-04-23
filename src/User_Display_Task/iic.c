#include "iic.h"

//IO��������
#define IIC_SCL_0   GPIO_ResetBits(GPIOB, GPIO_Pin_0)
#define IIC_SCL_1   GPIO_SetBits(GPIOB, GPIO_Pin_0)
#define IIC_SDA_0   GPIO_ResetBits(GPIOB, GPIO_Pin_1)
#define IIC_SDA_1   GPIO_SetBits(GPIOB, GPIO_Pin_1)

#define READ_SDA    GPIOB->IDR  & GPIO_Pin_1   //����SDA  

#define SDA_IN()   {GPIOB->MODER &= ~GPIO_MODER_MODER1;}
#define SDA_OUT()  {GPIOB->MODER &= ~GPIO_MODER_MODER1; GPIOB->MODER |= GPIO_MODER_MODER1_0;}    

_IIC_OB  iic_t;

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
        IIC_SDA_1;
        IIC_SCL_1;
        iic_state->I2C_step.start_status = 1; //�ȴ���ʱ
        break;
    case 1:
        IIC_SDA_0;
        iic_state->I2C_step.start_status = 2; //�ȴ���ʱ when CLK is high,DATA change form high to low
        break;
    case 2:
        iic_state->I2C_step.start_status = 3; //��ȴ�125us
        break;
    case 3:
        IIC_SCL_0;//ǯסI2C���ߣ�׼�����ͻ��������
        iic_state->I2C_step.start_status = 0;  //��λ
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
        IIC_SCL_0;
        IIC_SDA_0;//STOP:when CLK is high DATA change form low to high
        iic_state->I2C_step.stop_status = 1;  //��ʱ
        break;
    case 1:
        IIC_SCL_1;
        IIC_SDA_1;//����I2C���߽����ź�

        iic_state->I2C_step.start_status = 0;                      //�������������е�״̬         ����
        iic_state->I2C_step.send_status = 0;                       //����д�ӻ�ʱ��д״̬         ����
        iic_state->I2C_step.wait_ack_status = 0;                    //����д�ӻ�ʱ�ĵȴ�ACK״̬   ����
        iic_state->I2C_step.stop_status = 0;                       //����ֹͣ�ź�״̬             ����
        iic_state->I2C_step.read_status = 0;                       //�������ӻ�ʱ�Ķ�״̬         ����
        iic_state->I2C_step.ack_nack_status = 0;                   //�������ӻ�ʱ����ӦACK��nack״̬   ����

        iic_state->I2c_dispose_status = START_STATE;
    
        if(iic_state->Lock_app_status == set_data_command)
        {
            iic_state->Lock_app_status = sending_status;            //������������
        }
        else if(iic_state->Lock_app_status == sending_status)
        {
            iic_state->Lock_app_status = set_display_command;      //��������2
        }
        else
        {
            iic_state->Lock_app_status = IDEL_status;  //����״̬
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
//�ȴ�Ӧ���źŵ���
static void IIC_Wait_Ack(_IIC_STATE_T *iic_state)
{
    static u8 timeout = 0;
    switch(iic_state->I2C_step.wait_ack_status)
    {
    case 0:
        SDA_IN();      //SDA����Ϊ����
        //IIC_SDA_1;
        iic_state->I2C_step.wait_ack_status = 1;  //��ʱ
        break;
    case 1:
        IIC_SCL_1;
        iic_state->I2C_step.wait_ack_status = 2;  //��ʱ  ��ʱ �ӻ��յ������ؽ��жϷ�ACK
        break;
    case 2:
        if(READ_SDA && (++timeout > 10))  //û�յ�ACK
        {
            timeout = 0;
            iic_state->I2C_step.wait_ack_status = 0;
            iic_state->I2c_dispose_status = STOP_STATE;  //����STOPģʽ
        }
        else   //�յ�ACK
        {
            timeout = 0;
            IIC_SCL_0;
            iic_state->I2C_step.wait_ack_status = 0;
            
            if(iic_state->Lock_app_status == sending_status)
                iic_state->I2c_dispose_status = SEND_DATA_STATE;   //��������
            else
                iic_state->I2c_dispose_status = STOP_STATE;   //������
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
        IIC_SCL_0;
        SDA_OUT();
        IIC_SDA_0;
        iic_state->I2C_step.ack_nack_status = 1;
        break;
    case 1:
        IIC_SCL_1;
        iic_state->I2C_step.ack_nack_status = 2;  //������ ��ʱ �ȴ��ӻ�����
        break;
    case 2:
        IIC_SCL_0;
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
        IIC_SCL_0;
        SDA_OUT();
        IIC_SDA_1;
        iic_state->I2C_step.ack_nack_status = 1;
        break;
    case 1:
        IIC_SCL_1;
        iic_state->I2C_step.ack_nack_status = 2;  //������ ��ʱ �ȴ��ӻ�����
        break;
    case 2:
        IIC_SCL_0;
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
        s_uCount = 0;               //���
        SDA_OUT();
        IIC_SCL_0;                  //����ʱ�ӿ�ʼ���ݴ���
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
        iic_state->I2C_step.send_status = 2; //��ʱ
        break;
    case 2:
        IIC_SCL_1;
        iic_state->I2C_step.send_status = 3;   //��ʱ  �ȴ�SLAV�����ж�
        break;
    case 3:
        IIC_SCL_0;
        iic_state->I2C_step.send_status = 1;  //��ʱ
        if(s_uCount >= 8) //����8��bit һ���ֽ�
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
static void  IIC_Read_Byte(u8 *buf,_IIC_STATE_T *iic_state)
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
        IIC_SCL_0;
        iic_state->I2C_step.read_status = 2; //��ʱ
        break;
    case 2:
        IIC_SCL_1;
        iic_state->I2C_step.read_status = 3; //��ʱ  ������ �ȴ��ӻ� ��Ӧ������
        break;
    case 3:
        s_uCount++;
        s_uReceive <<= 1;
        if(READ_SDA) s_uReceive++;
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
            s_uData_num = 0;  //������16������
            iic_state->I2c_dispose_status = STOP_STATE;
            iic_state->g_uI2c_send_ok = I2C_SEND_OK;	    //==============�÷��ͳɹ���־λ
        }
        else
        {
            IIC_Send_Byte(*(buf+s_uData_num), iic_state);
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

/**@brief     Function for send a command by iic.
*@details  
*@param[in]  command : send command
*@param[in]  iic_state : iic state
*/
//==========дһ������==================================
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
//��ʼ��IIC     PB0 SCL ; PB1 SDA
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;       //���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //��©���
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
    case IDEL_status:			                                             	//����״̬
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

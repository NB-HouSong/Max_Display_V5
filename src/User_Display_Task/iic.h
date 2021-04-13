#ifndef __IIC_H
#define __IIC_H
#include "main.h"

#define  I2C_SEND_OK     1      //I2C����һ֡���ݳɹ���־
#define  I2C_READ_OK     1      //I2C��ȡһ֡���ݳɹ�

//=================================================================================
#define SEND_ADDR1    0xA4      //д�ӻ���ַ1 (������������)
#define SEND_ADDR2    0xA6      //д�ӻ���ַ2  (�����û�ʶ����)
#define READ_ADDR1    0xA4      //���ӻ��Ĵ�1��ַ  CHIP ID, �̼��汾��
#define READ_ADDR2    0xA6      //���ӻ��Ĵ�2��ַ  ��״̬������״̬����Ϣ

#define SEND_ADDR1_LEN  16      //16���ֽ�
#define SEND_ADDR2_LEN  8       //8���ֽ�
#define READ_ADDR1_LEN  16      //16���ֽ� 
#define READ_ADDR2_LEN  8       //8���ֽ�

#define READ_REG1 0             //���ӻ��Ĵ���1
#define READ_REG2 1             //���ӻ��Ĵ���2 
#define READ_REG3 2             //��IAP��Ϣ

#define IIC_WRITE_BUF_LEN   10

//====================״̬����=========================================================

typedef enum
{
    IDEL_status,            //����״̬
    sending_status,         //������ʾ���ݺ͵�ַ
    set_data_command,       //��������
    set_display_command     //������ʾ
} I2C_APP;                  //��������               һ��״̬

typedef enum
{
    START_STATE,         //���������ź�
    SEND_ADDR_STATE,     //���� �ӻ���ַ|(��/д)
    READ_ACK_STATE,      //�ȴӻ��� ACK�ź�
    SEND_DATA_STATE,	 //д����
    READ_DATA_STATE,     //������
    WRITE_ACK_STATE,     //��ӻ����� ACK ���ACK
    STOP_STATE,          //����ֹͣ�ź�
} I2C_STATE;             //I2C����ʱ��            ����״̬


typedef struct
{
    u8  start_status;                   //�������������е�״̬
    u8  send_status;                    //����д�ӻ�ʱ��д״̬
    u8  wait_ack_status;                //����д�ӻ�ʱ�ĵȴ�ACK״̬
    u8  stop_status;                    //����ֹͣ�ź�״̬
    u8  read_status;                    //�������ӻ�ʱ�Ķ�״̬
    u8  ack_nack_status;                //�������ӻ�ʱ����ӦACK��nack״̬
} I2C_STEP;                             //I2C����ʱ���о���ÿһ�����µ� ״̬     ����


typedef struct IIC_STATE
{
    I2C_APP     Lock_app_status ;        //������״̬             һ��
    I2C_STATE   I2c_dispose_status ;     //I2C �����ĸ�����״̬     ����
    I2C_STEP    I2C_step;                //I2C����ʱ���и��������µ�  ����
    u8          g_uI2c_send_ok ;                //д�ɹ���־������������������
    u8          s_uI2c_read_ok ;                //���ɹ���־������������������  
}_IIC_STATE_T;

typedef struct IIC_struct
{
    u8 command1;
    u8 command2;
    u8 g_uI2c_write_buf[IIC_WRITE_BUF_LEN];
    
    u8 s_uWrite_slave_address;
    u8 s_uWrite_slave_len;
    _IIC_STATE_T iic_state;
}_IIC_OB;


void IIC_Init(void);
    
void IicProcess(void);

u8 IIC_PushFrame(u8 *pdata);

I2C_APP IicGetAppStatus(void);

#endif

















#ifndef __UNLOCK_H
#define __UNLOCK_H

//#include "stm32f0xx.h"
#include "main.h"

#define SCL_PIN						    GPIO_Pin_7
#define SCL_GPIO						  GPIOA
#define SDA_PIN						    GPIO_Pin_6
#define SDA_GPIO						  GPIOA

#define CLK_H_BATLOCK         SCL_GPIO->BSRR |= SCL_PIN /*  */
#define CLK_L_BATLOCK         SCL_GPIO->BRR  |= SCL_PIN /*  */

#define DIO_H_BATLOCK         SDA_GPIO->BSRR |= SDA_PIN /*  */
#define DIO_L_BATLOCK         SDA_GPIO->BRR  |= SDA_PIN /*  */

#define CLK_read_BATLOCK     	SCL_GPIO->IDR  & SCL_PIN /*  */
#define DIO_read_BATLOCK      SDA_GPIO->IDR  & SDA_PIN /* */


#define  GET_UNLOCK_CMD  1               //��ÿ�������
#define  GET_LOCK_ID     1               //��ôӻ����� CHIP ID
#define  I2C_SEND_OK     1               //I2C����һ֡���ݳɹ���־
#define  I2C_READ_OK     1               //I2C��ȡһ֡���ݳɹ�

#define SWITCH_OFF (0)
#define SWITCH_ON  (1)
#define PRO_NM     (6)

#ifndef  NULL
#define  NULL  0
#endif
//=================����״̬===========================================
#define UNLOCK_STATUS                   1          //��λ���ؼ�⵽���ѿ�
#define LOCKED_STATUS                   0          //��λ���ؼ�⵽������
#define CAM_HOME_STATUS                 1          //���͹����ԭ��λ��
#define CAM_AWAY_STATUS                 0          //���͹��û��ԭλ

#define UNLOCK_MOTOR_WORKING            1  //���ڿ���������		
#define UNLOCK_OVER                     0  //���������ѽ���
#define RECEIVE_UNLOCK_CMD              1  //���յ���������
#define IDLE_UNLOCK_CMD                 0  //δ�յ���������

#define ACTION_FAIL                     1   //����ʧ��
#define ACTION_SUCCESS                  0   //�����ɹ�

//=================================================================================
#define SEND_ADDR1    0xA4     //д�ӻ���ַ1 (������������)
#define SEND_ADDR2    0xA6     //д�ӻ���ַ2  (�����û�ʶ����)
#define READ_ADDR1    0xA4     //���ӻ��Ĵ�1��ַ  CHIP ID, �̼��汾��
#define READ_ADDR2    0xA6     //���ӻ��Ĵ�2��ַ  ��״̬������״̬����Ϣ

#define IAP_ADDR       0x02       //�ӻ� IAP ��д �ĵ�ַ
#define READ_IAP_LEN   16         //24���ֽ�
#define SEND_IAP_LEN   72         //72���ֽ�

#define SEND_ADDR1_LEN  16   //16���ֽ�
#define SEND_ADDR2_LEN  8   //8���ֽ�
#define READ_ADDR1_LEN  16   //16���ֽ� 
#define READ_ADDR2_LEN  8    //8���ֽ�

#define READ_REG1 0    //���ӻ��Ĵ���1
#define READ_REG2 1    //���ӻ��Ĵ���2 
#define READ_REG3 2    //��IAP��Ϣ

#define CHIP_ID_LENG 12 //��оƬSTM8��chip ID����
//===============================================================================================
//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))
//IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+12)  //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12)  //0x40010C0C 
#define GPIOA_IDR_Addr    (GPIOA_BASE+8)   //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8)   //0x40010C08 
//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

////IO��������
//#define SDA_IN()  {SDA_GPIO->MODER&=0XFFF0FFFF;SDA_GPIO->CRH|=(u32)8<<16;}   //PA12  ����/��������ģʽ
//#define SDA_OUT() {SDA_GPIO->CRH&=0XFFF0FFFF;SDA_GPIO->CRH|=(u32)3<<16;}     //PA12   50MHz�������ģʽ

//IO��������
#define IIC_SCL    PAout(6)   //SCL              
#define IIC_SDA    PAout(7)   //SDA	        
#define READ_SDA   PAin(7)    //����SDA    


//====================״̬����=========================================================

typedef enum
{
    IDEL_status,           //����״̬
    sending_status,        //����д�ӻ� 16�ֽ�����
    reading_status         //�������ӻ� 16�ֽ�����
} I2C_APP;                //��������               һ��״̬

typedef enum
{
    START_STATE,         //���������ź�
    SEND_ADDR_STATE,     //���� �ӻ���ַ|(��/д)
    READ_ACK_STATE,      //�ȴӻ��� ACK�ź�
    SEND_DATA_STATE,	   //д����
    READ_DATA_STATE,     //������
    WRITE_ACK_STATE,     //��ӻ����� ACK ���ACK
    STOP_STATE,          //����ֹͣ�ź�
} I2C_STATE;             //I2C����ʱ��            ����״̬

typedef enum{
    B_READ_VERSION,
    B_SET_ID,
    B_UNLOCK_ACT,
    B_BT_INFO_ANYS,
    B_INQUIRE_ACT,   
    B_NULL_STATE,
}BT_LOCK_EVENT;

typedef struct
{
    u8  start_status;                    //�������������е�״̬
    u8  send_status;                     //����д�ӻ�ʱ��д״̬
    u8  wait_ack_status;                 //����д�ӻ�ʱ�ĵȴ�ACK״̬
    u8  stop_status;                     //����ֹͣ�ź�״̬
    u8  read_status;                     //�������ӻ�ʱ�Ķ�״̬
    u8  ack_nack_status;                 //�������ӻ�ʱ����ӦACK��nack״̬
} I2C_STEP;                            //I2C����ʱ���о���ÿһ�����µ� ״̬     ����


typedef struct IIC_STATE
{
    I2C_APP    Lock_app_status ;              //������״̬             һ��
    I2C_STATE  I2c_dispose_status ;           //I2C �����ĸ�����״̬     ����
    I2C_STEP   I2C_step;                      //I2C����ʱ���и��������µ�  ����
    u8  g_uI2c_send_ok ;          //д�ɹ���־������������������
    u8  s_uI2c_read_ok ;          //���ɹ���־������������������  
}_IIC_STATE_T;

typedef struct IIC_struct{

    u8 s_uI2c_chip_id[CHIP_ID_LENG]; //���� chipID
    u8 g_uI2c_write_buf[72];
    u8 g_uI2c_read_buf[16];
    u8 s_uGet_lock_ID ;    // �յ����� CHIPID�� �汾����Ϣ
    u8 s_uUnlock_cmd ;     //���������־
    u8 s_Lock_ID_match ;   //�û�ID����ƥ��
    u8 s_uRead_slave_regnum ;     //���ӻ��� 1�ڴ��� 2�ڴ���
    u8 s_uread_slave_data_len;
    u8 s_uWrite_slave_regnum;    //д�ӻ��� 1�ڴ��� 2�ڴ���
    u8 s_uWrite_slave_data_len;
    _IIC_STATE_T iic_state;
    void (*iic_init)(void);
    //void (*iic_driver)(void); 
    //void (*iic_commu)(u8 addr,u8 len,u8 *buf);
    void (*I2C_decrypt)(u8 *pData, u8 *key) ;  //����
    void (*I2C_encrypt)(u8 *pData, u8 *key) ;  //����
    //void (*I2C_IAP_Reset)(void);
}_IIC_OB;

typedef struct BATTERY_LOCK{

   _IIC_OB  iic_t; 
   u8 s_obtain_lock_info_swtich;
   u16 s_obtain_lock_info_period;
   u8 s_lock_commu_cnt;
   u8 s_lock_event;
   u16 s_lock_timeout;
   u8 s_set_client_id_flag;
   u8 s_set_client_id_cnt;
   u16 s_temp_id;
   void (*iic_dispose_pro)(void);
   void (*battery_lock_pro)(void);     
}_BATTERY_LOCK_OB;

extern _BATTERY_LOCK_OB _battery_lock_object;
//=============ģ������I2C����======================================================
//IIC���в�������
//void BattLockProcess(u16 elected_version );                          //�������� ����5ms������
void IicProcess(void);                                //I2Cд �� ���� ����125us��
//===========�ӽ���==============================================================


//===========CRCУ��======================================
u16 crc16_update(u16 crc, u8 a);
u8 lowByte(u16 ww);
u8 highByte(u16 ww);
//=================����״̬==========================================================
/*
typedef struct
{
    u8 lock_button_status;  //������λ����״̬
    u8 cam_button_status;   //���͹�����λ����״̬

    u8 unlock_cmd_flg;     //�Ƿ��յ���������
    u8 wait_result_flg;    //�Ƿ��ڽ���������
	  u8 action_result_flg;  //ִ�н����Ľ�� :�ɹ� ʧ��
}LOCK_PARAM_TYPE;

extern LOCK_PARAM_TYPE g_stLockStatus;
*/

//=================IAP���======================================
//void IAP_I2C(u8* pData, u8 channel);

void _Battery_Lock_Object_Malloc(_BATTERY_LOCK_OB *p_this);
#define IAP2_FRM_LEN			0
#define IAP2_FRM_SRC_ID	        1
#define IAP2_FRM_DST_ID	        2
#define IAP2_FRM_CMD			3
#define IAP2_FRM_INDEX		    4
#define IAP2_FRM_DATA		    5

//IAP״̬����
#define IAP2_OK					0
#define IAP2ERROR_SIZE			1 			//�̼���С����Χ
#define IAP2ERROR_ERASE			2 			//����Flashʧ��
#define IAP2ERROR_WRITEFLASH	3		//д��Flashʧ��
#define IAP2ERROR_UNLOCK		4			//û�д�������״̬
#define IAP2ERROR_INDEX			5			  //��������
#define IAP2ERROR_BUSY			6			  //����æ
#define IAP2ERROR_FORM			7 			//���ݸ�ʽ����
#define IAP2ERROR_CRC			8			    //У�����
#define IAP2ERROR_OTHER			9			  //��������

#define MAX_IAP2SW_SIZE			0x1800 		//���̼���С  6K

#endif

















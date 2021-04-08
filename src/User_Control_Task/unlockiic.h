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


#define  GET_UNLOCK_CMD  1               //获得开锁命令
#define  GET_LOCK_ID     1               //或得从机锁的 CHIP ID
#define  I2C_SEND_OK     1               //I2C发送一帧数据成功标志
#define  I2C_READ_OK     1               //I2C读取一帧数据成功

#define SWITCH_OFF (0)
#define SWITCH_ON  (1)
#define PRO_NM     (6)

#ifndef  NULL
#define  NULL  0
#endif
//=================锁的状态===========================================
#define UNLOCK_STATUS                   1          //限位开关检测到锁已开
#define LOCKED_STATUS                   0          //限位开关检测到已上锁
#define CAM_HOME_STATUS                 1          //电机凸点在原点位置
#define CAM_AWAY_STATUS                 0          //电机凸点没回原位

#define UNLOCK_MOTOR_WORKING            1  //还在开锁过程中		
#define UNLOCK_OVER                     0  //开锁过程已结束
#define RECEIVE_UNLOCK_CMD              1  //已收到开锁命令
#define IDLE_UNLOCK_CMD                 0  //未收到开锁命令

#define ACTION_FAIL                     1   //解锁失败
#define ACTION_SUCCESS                  0   //解锁成功

//=================================================================================
#define SEND_ADDR1    0xA4     //写从机地址1 (即：开锁命令)
#define SEND_ADDR2    0xA6     //写从机地址2  (设置用户识别码)
#define READ_ADDR1    0xA4     //读从机寄存1地址  CHIP ID, 固件版本号
#define READ_ADDR2    0xA6     //读从机寄存2地址  锁状态、错误状态等信息

#define IAP_ADDR       0x02       //从机 IAP 读写 的地址
#define READ_IAP_LEN   16         //24个字节
#define SEND_IAP_LEN   72         //72个字节

#define SEND_ADDR1_LEN  16   //16个字节
#define SEND_ADDR2_LEN  8   //8个字节
#define READ_ADDR1_LEN  16   //16个字节 
#define READ_ADDR2_LEN  8    //8个字节

#define READ_REG1 0    //读从机寄存器1
#define READ_REG2 1    //读从机寄存器2 
#define READ_REG3 2    //读IAP信息

#define CHIP_ID_LENG 12 //锁芯片STM8的chip ID长度
//===============================================================================================
//位带操作,实现51类似的GPIO控制功能
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12)  //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12)  //0x40010C0C 
#define GPIOA_IDR_Addr    (GPIOA_BASE+8)   //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8)   //0x40010C08 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

////IO方向设置
//#define SDA_IN()  {SDA_GPIO->MODER&=0XFFF0FFFF;SDA_GPIO->CRH|=(u32)8<<16;}   //PA12  上拉/下拉输入模式
//#define SDA_OUT() {SDA_GPIO->CRH&=0XFFF0FFFF;SDA_GPIO->CRH|=(u32)3<<16;}     //PA12   50MHz推挽输出模式

//IO操作函数
#define IIC_SCL    PAout(6)   //SCL              
#define IIC_SDA    PAout(7)   //SDA	        
#define READ_SDA   PAin(7)    //输入SDA    


//====================状态定义=========================================================

typedef enum
{
    IDEL_status,           //空闲状态
    sending_status,        //主机写从机 16字节数据
    reading_status         //主机读从机 16字节数据
} I2C_APP;                //处理任务               一级状态

typedef enum
{
    START_STATE,         //发送启动信号
    SEND_ADDR_STATE,     //发送 从机地址|(读/写)
    READ_ACK_STATE,      //等从机的 ACK信号
    SEND_DATA_STATE,	   //写数据
    READ_DATA_STATE,     //读数据
    WRITE_ACK_STATE,     //向从机发送 ACK 或非ACK
    STOP_STATE,          //发送停止信号
} I2C_STATE;             //I2C操作时序            二级状态

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
    u8  start_status;                    //发送启动过程中的状态
    u8  send_status;                     //主机写从机时的写状态
    u8  wait_ack_status;                 //主机写从机时的等待ACK状态
    u8  stop_status;                     //发送停止信号状态
    u8  read_status;                     //主机读从机时的读状态
    u8  ack_nack_status;                 //主机读从机时的响应ACK或nack状态
} I2C_STEP;                            //I2C操作时序中具体每一步骤下的 状态     三级


typedef struct IIC_STATE
{
    I2C_APP    Lock_app_status ;              //主任务状态             一级
    I2C_STATE  I2c_dispose_status ;           //I2C 处于哪个操作状态     二级
    I2C_STEP   I2C_step;                      //I2C操作时序中各个步骤下的  三级
    u8  g_uI2c_send_ok ;          //写成功标志，需主函数处理后清空
    u8  s_uI2c_read_ok ;          //读成功标志，需主函数处理后清空  
}_IIC_STATE_T;

typedef struct IIC_struct{

    u8 s_uI2c_chip_id[CHIP_ID_LENG]; //锁的 chipID
    u8 g_uI2c_write_buf[72];
    u8 g_uI2c_read_buf[16];
    u8 s_uGet_lock_ID ;    // 收到锁的 CHIPID、 版本号信息
    u8 s_uUnlock_cmd ;     //开锁命令标志
    u8 s_Lock_ID_match ;   //用户ID密码匹配
    u8 s_uRead_slave_regnum ;     //读从机的 1内存区 2内存区
    u8 s_uread_slave_data_len;
    u8 s_uWrite_slave_regnum;    //写从机的 1内存区 2内存区
    u8 s_uWrite_slave_data_len;
    _IIC_STATE_T iic_state;
    void (*iic_init)(void);
    //void (*iic_driver)(void); 
    //void (*iic_commu)(u8 addr,u8 len,u8 *buf);
    void (*I2C_decrypt)(u8 *pData, u8 *key) ;  //解密
    void (*I2C_encrypt)(u8 *pData, u8 *key) ;  //加密
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
//=============模拟主机I2C程序======================================================
//IIC所有操作函数
//void BattLockProcess(u16 elected_version );                          //锁处理函数 放入5ms任务中
void IicProcess(void);                                //I2C写 读 函数 放入125us中
//===========加解密==============================================================


//===========CRC校验======================================
u16 crc16_update(u16 crc, u8 a);
u8 lowByte(u16 ww);
u8 highByte(u16 ww);
//=================锁的状态==========================================================
/*
typedef struct
{
    u8 lock_button_status;  //锁的限位开关状态
    u8 cam_button_status;   //电机凸点的限位开关状态

    u8 unlock_cmd_flg;     //是否收到解锁命令
    u8 wait_result_flg;    //是否还在解锁过程中
	  u8 action_result_flg;  //执行解锁的结果 :成功 失败
}LOCK_PARAM_TYPE;

extern LOCK_PARAM_TYPE g_stLockStatus;
*/

//=================IAP相关======================================
//void IAP_I2C(u8* pData, u8 channel);

void _Battery_Lock_Object_Malloc(_BATTERY_LOCK_OB *p_this);
#define IAP2_FRM_LEN			0
#define IAP2_FRM_SRC_ID	        1
#define IAP2_FRM_DST_ID	        2
#define IAP2_FRM_CMD			3
#define IAP2_FRM_INDEX		    4
#define IAP2_FRM_DATA		    5

//IAP状态定义
#define IAP2_OK					0
#define IAP2ERROR_SIZE			1 			//固件大小超范围
#define IAP2ERROR_ERASE			2 			//擦除Flash失败
#define IAP2ERROR_WRITEFLASH	3		//写入Flash失败
#define IAP2ERROR_UNLOCK		4			//没有处在锁车状态
#define IAP2ERROR_INDEX			5			  //索引错误
#define IAP2ERROR_BUSY			6			  //总线忙
#define IAP2ERROR_FORM			7 			//数据格式错误
#define IAP2ERROR_CRC			8			    //校验错误
#define IAP2ERROR_OTHER			9			  //其他错误

#define MAX_IAP2SW_SIZE			0x1800 		//最大固件大小  6K

#endif

















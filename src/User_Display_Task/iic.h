#ifndef __IIC_H
#define __IIC_H
#include "main.h"

#define  I2C_SEND_OK     1      //I2C发送一帧数据成功标志
#define  I2C_READ_OK     1      //I2C读取一帧数据成功

//=================================================================================
#define SEND_ADDR1    0xA4      //写从机地址1 (即：开锁命令)
#define SEND_ADDR2    0xA6      //写从机地址2  (设置用户识别码)
#define READ_ADDR1    0xA4      //读从机寄存1地址  CHIP ID, 固件版本号
#define READ_ADDR2    0xA6      //读从机寄存2地址  锁状态、错误状态等信息

#define SEND_ADDR1_LEN  16      //16个字节
#define SEND_ADDR2_LEN  8       //8个字节
#define READ_ADDR1_LEN  16      //16个字节 
#define READ_ADDR2_LEN  8       //8个字节

#define READ_REG1 0             //读从机寄存器1
#define READ_REG2 1             //读从机寄存器2 
#define READ_REG3 2             //读IAP信息

#define IIC_WRITE_BUF_LEN   10

//====================状态定义=========================================================

typedef enum
{
    IDEL_status,            //空闲状态
    sending_status,         //发送显示内容和地址
    set_data_command,       //设置数据
    set_display_command     //设置显示
} I2C_APP;                  //处理任务               一级状态

typedef enum
{
    START_STATE,         //发送启动信号
    SEND_ADDR_STATE,     //发送 从机地址|(读/写)
    READ_ACK_STATE,      //等从机的 ACK信号
    SEND_DATA_STATE,	 //写数据
    READ_DATA_STATE,     //读数据
    WRITE_ACK_STATE,     //向从机发送 ACK 或非ACK
    STOP_STATE,          //发送停止信号
} I2C_STATE;             //I2C操作时序            二级状态


typedef struct
{
    u8  start_status;                   //发送启动过程中的状态
    u8  send_status;                    //主机写从机时的写状态
    u8  wait_ack_status;                //主机写从机时的等待ACK状态
    u8  stop_status;                    //发送停止信号状态
    u8  read_status;                    //主机读从机时的读状态
    u8  ack_nack_status;                //主机读从机时的响应ACK或nack状态
} I2C_STEP;                             //I2C操作时序中具体每一步骤下的 状态     三级


typedef struct IIC_STATE
{
    I2C_APP     Lock_app_status ;        //主任务状态             一级
    I2C_STATE   I2c_dispose_status ;     //I2C 处于哪个操作状态     二级
    I2C_STEP    I2C_step;                //I2C操作时序中各个步骤下的  三级
    u8          g_uI2c_send_ok ;                //写成功标志，需主函数处理后清空
    u8          s_uI2c_read_ok ;                //读成功标志，需主函数处理后清空  
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

















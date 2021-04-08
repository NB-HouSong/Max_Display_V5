/**
 * @file UsartProtocol.h
 *
 * @brief This file describes the public UsartProtocol functions and data types.
 */

#ifndef USARTPROTOCOL_H
#define USARTPROTOCOL_H

#include <stdbool.h>
#include "UART_3.h"

//#define V6_SCOOTER_ID           0x20        //SCOOTER?ID
//#define V6_SCOOTER_DIS_ID       0x21        //Voi SCOOTER????ID   ???0x21
//#define V6_MY_ID		        V6_SCOOTER_DIS_ID

////滑板车专用指令，中舱以20ms的周期发送
//#define  CMD_SCO_RD             0x61        //scooter专用读，数据段含有油门和刹车等控制数据
//#define  CMD_SCO_CTL            0x64        //scooter专用，数据段只含有油门和刹车等控制数据，主控需要返回电量、前灯控制等数据
//#define  CMD_SCO_CTL_NR         0x65        //scooter专用，数据段只含有油门和刹车等控制数据，无需主控返回信息

////数据下载相关指令
//#define CMD_IAP_BEGIN			0x07		//IAP开启指令
//#define CMD_IAP_WR				0x08		//IAP写固件指令
//#define CMD_IAP_CRC				0x09		//IAP校验固件指令
//#define CMD_IAP_RESET			0x0A		//系统复位指令
//#define CMD_IAP_ACK				0x0B		//应答帧

////帧相关宏定义
//#define FRM_LEN			        0
//#define FRM_SRC_ID	            1
//#define FRM_DST_ID	            2
//#define FRM_CMD			        3
//#define FRM_INDEX		        4
//#define FRM_DATA		        5
/**
 * @brief Initialize the UsartProtocol module, this function should be executed before any other functions.
 * @param no.
 */
void usartProtocolInitialize(void);

///**
// * @brief Handle a received UsartMessage from the usart bus.
// * This function will execute the appropriate callback function.
// * @param message The UsartMessage.
// */
//void usartProtocolHandleData(const uint8_t *message);

/**
 * @brief Create a Iap message.
 * @param message 
 */
void usartProtocolIapResponse(uint8_t *message);

void usartProtocolToScooterResponse(UsartMessage *message);


//void scooterUpDataSetCallback(void (*dataHandler)(const uint8_t *pData));

//void scooterUpgradeDataSetCallback(void (*dataHandler)(const uint8_t *pData));
#endif

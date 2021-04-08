#ifndef UART4_H
#define UART4_H
#include "stm32f0xx.h"

void UART4_Int(void);
void UART4_CheckReceive(void);
void UART4_CheckSend(void);
void UART4_PushFrame(u8 src_id, u8 dst_id, u8 DataLen, u8 Cmd, u8 Index, u8*pdata);
void UART4_PopFrame(void);
#endif

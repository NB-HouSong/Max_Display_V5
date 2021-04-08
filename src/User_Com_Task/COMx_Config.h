#ifndef _COMX_CONFIG_H_
#define _COMX_CONFIG_H_
#include "main.h"

void COMX_Drive_Init(void);
void COMX_Analysis_Frame(void);
void COMX_Transmit_Frame(void);
void COMX_Push_Frame(_PUSH_DATA_STRC push_data);

#endif


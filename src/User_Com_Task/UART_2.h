#ifndef UART2_H
#define UART2_H 


#define DMX512Number 2   
#define TM1804Number 3   
#define BufferLength (DMX512Number*TM1804Number)


#ifdef SMT_TEST_MODE
#define UART2_TX_H         	GPIOA->BSRR |= GPIO_Pin_2 /* GPIO_SetBits(GPIOB , GPIO_Pin_11)   */
#define UART2_TX_L        	GPIOA->BRR  |= GPIO_Pin_2 /* GPIO_ResetBits(GPIOB , GPIO_Pin_11) */
void UART2_TEST_Int(void);
#endif

//#define EXUART_BRR_9600					0x0EA6
//#define EXUART_BRR_76800				0x01D5
//#define EXUART_BRR_115200				0x0138

//#define UART_SetBaudTo9600()	(EXT_UART->BRR = EXUART_BRR_9600)
//#define UART_SetBaudTo76800()	(EXT_UART->BRR = EXUART_BRR_76800)
//#define UART_SetBaudTo115200()	(EXT_UART->BRR = EXUART_BRR_115200)

void UART2_Int(void);				//UART相关初始化
void UART2_CheckReceive(void);
void UART2_CheckSend(void);	
void UART2_PushFrame(u8 src_id, u8 dst_id, u8 DataLen, u8 Cmd, u8 Index, u8*pdata);
void UART2_PopFrame(void);

void DMX512_Send_DataFrame_Init(void);   //发送的第一幁为假数据
void DMX512_Send_DataFrame(void);
void uart2_init(u32 bound);

void Recover_Tx_AF(void);
void Wake_Up_Iot(void);


#endif

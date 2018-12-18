#ifndef UART_H
#define UART_H

/* Defines -------------------------------------------------------------------*/
#define UART_RBUF_SIZE			0x100

/* Type declarations ---------------------------------------------------------*/
typedef struct{
    uint16_t RxInCnt,RxOutCnt;
    uint16_t TxInCnt,TxOutCnt;
    uint8_t RxBuf[UART_RBUF_SIZE];
}StructUart;

void USART1_UART_Init(void);
void UART1_PutChar(uint8_t data);
void UART1_PutStr(uint8_t *pStr);

#endif

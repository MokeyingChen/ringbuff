#ifndef __DRIVER_UART_4G_H__
#define __DRIVER_UART_4G_H__

#include "main.h"

#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;

#define U2_Tx_Size 1024*2
#define U2_Rx_Size 1024*5
#define U2_DMARx_Size 896
#define U2_ptf_Size 512
#define URxNum 15

// 定义函数指针类型
typedef void (*U_DataEvent_t)(uint8_t *data, uint16_t length);



typedef struct
{
    uint8_t *startPtr;
    uint8_t *endPtr;
    uint16_t UCounter;
}UCB_UBuffPtr;//环形缓冲区指针

typedef struct
{
    // UCB_UBuffPtr UTxDataBufPtr[U1TxNum];
    // UCB_UBuffPtr *UTxDataINPtr;
    // UCB_UBuffPtr *UTxDataOUTPtr;
    // UCB_UBuffPtr *UTxDataENDPtr;

    UCB_UBuffPtr URxDataBufPtr[URxNum];
    UCB_UBuffPtr *URxDataINPtr;
    UCB_UBuffPtr *URxDataOUTPtr;
    UCB_UBuffPtr *URxDataENDPtr;
}UCB_CB;//环形缓冲区结构体


//extern stc_usart_uart_init_t stcUart1InitCfg;
//extern stc_uartCfg_t stcUart1Cfg;
extern UCB_CB U2_RX_CB;
extern UCB_CB U2_TX_CB;

extern uint8_t u2_databuf[U2_DMARx_Size];
extern uint8_t u2_TxBuffer[U2_Tx_Size];
extern uint8_t u2_RxBuffer[U2_Rx_Size];
extern uint8_t u2_DMA_RxBuffer[U2_DMARx_Size];
extern uint8_t u2_ptfbuf[U2_ptf_Size];

void U2_Init(void);
void U2Rx_Buff_Init(void);         
void U2Tx_Buff_Init(void);
void U2_Printf(char *format, ...);

uint16_t Command_WriteCclbuf(uint8_t *data, uint16_t length,UCB_CB *pUcb,uint8_t *u_RxBuffer,uint16_t bufsize);
void U_ProcessCclbuf(UCB_CB *pUcb,uint8_t *u_RxBuffer,uint16_t rxbufsize,uint8_t *u_databuf,uint16_t databuf_Size,U_DataEvent_t DataEvent);
void U2_RxDataEvent(uint8_t *data, uint16_t length);
void U2_TxDataEvent(uint8_t *data, uint16_t length);
void U2_Receiving_processing(void);
#endif






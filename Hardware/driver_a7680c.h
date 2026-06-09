#ifndef __DRIVER_A7680C_H__
#define __DRIVER_A7680C_H__

#include "main.h"
#include "driver_uart_4g.h"

/***********************************A7680C——uart************************************/

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;

#define U2_Tx_Size 1024*2
#define U2_Rx_Size 1024*5
#define U2_DMARx_Size 896
#define U2_ptf_Size 512

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
void U2_RxDataEvent(uint8_t *data, uint16_t length);
void U2_TxDataEvent(uint8_t *data, uint16_t length);
void U2_Receiving_processing(void);

/************************************A7680C——app************************************/

extern volatile uint8_t a7680c_flag;    
extern char mqtt_data[256];
extern char mqtt_data_req[256];

void a7680c_Init(void);
uint8_t a7680c_Cmd(const char *cmd, const char *resp, uint16_t timeout);
uint8_t a7680c_rst(void);
uint8_t a7680c_Connect_IoTServer(void);
void a7680c_Report(void);
void a7680c_PropertyPost(char * postdata);
void a7680c_power_on(void);
void a7680c_power_off(void);
void a7680c_reconnect(void);

void a7680c_Task(void);
#endif /* __DRIVER_A7680C_H__ */






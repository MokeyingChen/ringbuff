#ifndef __MIDDLE_RINGBUFF_H__
#define __MIDDLE_RINGBUFF_H__

#include <stdint.h>
#include <string.h>

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


uint16_t Command_WriteCclbuf(uint8_t *data, uint16_t length,UCB_CB *pUcb,uint8_t *u_RxBuffer,uint16_t bufsize);
void U_ProcessCclbuf(UCB_CB *pUcb,uint8_t *u_RxBuffer,uint16_t rxbufsize,uint8_t *u_databuf,uint16_t databuf_Size,U_DataEvent_t DataEvent);

#endif




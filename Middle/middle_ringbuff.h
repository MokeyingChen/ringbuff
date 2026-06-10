#ifndef __MIDDLE_RINGBUFF_H__
#define __MIDDLE_RINGBUFF_H__

#include <stdint.h>
#include <string.h>


// 定义函数指针类型
typedef void (*RingBuff_Event_t)(uint8_t *data, uint16_t length);

typedef struct
{
    uint8_t *startPtr;
    uint8_t *endPtr;
    uint16_t count;
}RingBuff_Slot;//环形缓冲区指针

typedef struct
{
    RingBuff_Slot *slot;
    RingBuff_Slot *in;
    RingBuff_Slot *out;
    RingBuff_Slot *end;

    uint8_t *buffer;      // ← 新增：环形缓冲首地址
    uint16_t bufSize;     // ← 新增：环形缓冲大小
    uint16_t slotNum;     // ← 新增：环形缓冲槽槽数
}RingBuff_CB;//环形缓冲区结构体

void RingBuff_Init(RingBuff_CB *rb, uint8_t *buffer, uint16_t bufSize,RingBuff_Slot *slotArray, uint16_t slotNum);
uint16_t RingBuff_Write(RingBuff_CB *rb,uint8_t *data, uint16_t length);
void RingBuff_Process(RingBuff_CB *rb,uint8_t *buf,uint16_t size,RingBuff_Event_t callback);
#endif




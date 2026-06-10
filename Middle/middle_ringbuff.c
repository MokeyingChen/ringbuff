#include "middle_ringbuff.h"

void RingBuff_Init(RingBuff_CB *rb, uint8_t *buffer, uint16_t bufSize,
                   RingBuff_Slot *slotArray, uint16_t slotNum)
{
    memset(slotArray, 0, sizeof(RingBuff_Slot) * slotNum);
    memset(rb, 0, sizeof(RingBuff_CB));

    rb->buffer = buffer;
    rb->bufSize = bufSize;
    rb->slot = slotArray;
    rb->slotNum = slotNum;

    rb->in = &slotArray[0];
    rb->out = &slotArray[0];
    rb->end = &slotArray[slotNum - 1];

    rb->in->startPtr = buffer;
    rb->in->endPtr = buffer;
    rb->in->count = 0;
}

/**
* @brief 计算接收缓冲区未处理的数据长度
* @param pUcb UCB_CB结构体指针
* @param u_RxBuffer 接收环形缓冲区指针
* @param bufsize 环形缓冲区大小
* @return 未处理的数据长度
*/
uint16_t RingBuff_GetLength(RingBuff_CB *rb)
{
    uint16_t in = rb->in->startPtr - rb->buffer;
    uint16_t out = rb->out->startPtr - rb->buffer;
    if (in >= out)
        return in - out;
    else
        return rb->bufSize - out + in;
}

/**
* @brief 计算接收缓冲区剩余空间
* @param pUcb UCB_CB结构体指针
* @param u_RxBuffer 接收环形缓冲区指针
* @param bufsize 环形缓冲区大小
* @return 剩余空间
* @retval 0 环形缓冲区已满
* @retval 1~bufsize-1 剩余空间
* @retval bufsize 环形缓冲区为空
*/
uint16_t RingBuff_GetRemain(RingBuff_CB *rb) {
    return rb->bufSize - RingBuff_GetLength(rb);
}

/**
* @brief 向接收环形缓冲区写入数据
* @param data 要写入的数据指针
* @param length 要写入的数据长度
* @param pUcb UCB_CB结构体指针
* @param u_RxBuffer 接收环形缓冲区指针
* @param bufsize 环形缓冲区大小
* @return 写入的数据长度
*/
uint16_t RingBuff_Write(RingBuff_CB *rb,uint8_t *data, uint16_t length) {
    // 如果缓冲区不足 则不写入数据 返回0
    if (RingBuff_GetRemain(rb) < length) {
        //U1_Printf("RingBuff no space\r\n");
        return 0;
    }
    rb->in->count = length;
    
    // 使用memcpy函数将数据写入缓冲区
    if ((rb->in->startPtr - rb->buffer) + length < rb->bufSize) {
        memcpy(rb->in->startPtr, data, length);
        uint8_t *startPtr=rb->in->startPtr+length;
        rb->in->endPtr = startPtr - 1;
        rb->in++;
        if(rb->in == rb->end)
        {
            rb->in = rb->slot;
        }
        rb->in->startPtr = startPtr;

    } else {
        uint16_t firstLength = rb->bufSize - (rb->in->startPtr - rb->buffer);
        memcpy(rb->in->startPtr, data, firstLength);
        memcpy(rb->buffer, data + firstLength, length - firstLength);
         uint8_t *startPtr = &rb->buffer[length - firstLength];
         if(length - firstLength==0)
         {
             rb->in->endPtr = &rb->buffer[rb->bufSize-1];
         }else{
             rb->in->endPtr = startPtr - 1;
         }
          rb->in++;
        if(rb->in == rb->end)
        {
            rb->in = rb->slot;
        }
        rb->in->startPtr = startPtr;
    }
    return length;
}

/**
* @brief 从接收环形缓冲区读取数据
* @param rb 接收环形缓冲区指针
* @param dst 读取数据指针
* @param maxLen 读取数据最大长度
* @return 读取的数据长度
*/
uint16_t RingBuff_Read(RingBuff_CB *rb, uint8_t *dst, uint16_t maxLen)
{
    if (rb->in == rb->out) return 0;

    uint16_t len = rb->out->count;
    if (len > maxLen) len = maxLen;

    uint16_t offset = rb->out->startPtr - rb->buffer;
    uint16_t first  = rb->bufSize - offset;

    if (len <= first) {
        memcpy(dst, rb->out->startPtr, len);
    } else {
        memcpy(dst, rb->out->startPtr, first);
        memcpy(dst + first, rb->buffer, len - first);
    }

    rb->out->startPtr = rb->buffer + (offset + len) % rb->bufSize;
    rb->out++;
    if (rb->out == rb->end) rb->out = rb->slot;
    return len;
}

/**
* @brief 处理接收数据事件
* @param rb 接收环形缓冲区指针
* @param buf 接收数据指针
* @param size 接收数据缓冲区大小
* @param callback 数据事件函数回调
*/
void RingBuff_Process(RingBuff_CB *rb, uint8_t *buf, uint16_t size, RingBuff_Event_t callback)
{
    uint16_t len = RingBuff_Read(rb, buf, size);
    if (len > 0) callback(buf, len);
}

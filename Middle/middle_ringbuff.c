#include "middle_ringbuff.h"


/**
* @brief 计算接收缓冲区未处理的数据长度
* @param pUcb UCB_CB结构体指针
* @param u_RxBuffer 接收环形缓冲区指针
* @param bufsize 环形缓冲区大小
* @return 未处理的数据长度
*/
uint16_t Command_GetRxLength(UCB_CB *pUcb,uint8_t *u_RxBuffer,uint16_t bufsize) {
    return ((pUcb->URxDataINPtr->startPtr-u_RxBuffer) +bufsize- (pUcb->URxDataOUTPtr->startPtr-u_RxBuffer)) % bufsize;
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
uint16_t Command_GetRxRemain(UCB_CB *pUcb,uint8_t *u_RxBuffer,uint16_t bufsize) {
    return bufsize - Command_GetRxLength(pUcb,u_RxBuffer,bufsize);
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
uint16_t Command_WriteCclbuf(uint8_t *data, uint16_t length,UCB_CB *pUcb,uint8_t *u_RxBuffer,uint16_t bufsize) {
    // 如果缓冲区不足 则不写入数据 返回0
    if (Command_GetRxRemain(pUcb,u_RxBuffer,bufsize) < length) {
        //U1_Printf("u1RXbuff no space\r\n");
        return 0;
    }
    pUcb->URxDataINPtr->UCounter = length;
    
    // 使用memcpy函数将数据写入缓冲区
    if ((pUcb->URxDataINPtr->startPtr-u_RxBuffer) + length < bufsize) {
        memcpy(pUcb->URxDataINPtr->startPtr, data, length);
        uint8_t *startPtr=pUcb->URxDataINPtr->startPtr+length;
        pUcb->URxDataINPtr->endPtr = startPtr - 1;
        pUcb->URxDataINPtr++;
        if(pUcb->URxDataINPtr == pUcb->URxDataENDPtr)
        {
            pUcb->URxDataINPtr = pUcb->URxDataBufPtr;
        }
        pUcb->URxDataINPtr->startPtr = startPtr;

    } else {
        uint16_t firstLength = bufsize - (pUcb->URxDataINPtr->startPtr-u_RxBuffer);
        memcpy(pUcb->URxDataINPtr->startPtr, data, firstLength);
        memcpy(u_RxBuffer, data + firstLength, length - firstLength);
         uint8_t *startPtr = &u_RxBuffer[length - firstLength];
         if(length - firstLength==0)
         {
             pUcb->URxDataINPtr->endPtr = &u_RxBuffer[bufsize-1];
         }else{
             pUcb->URxDataINPtr->endPtr = startPtr - 1;
         }
          pUcb->URxDataINPtr++;
        if(pUcb->URxDataINPtr == pUcb->URxDataENDPtr)
        {
            pUcb->URxDataINPtr = pUcb->URxDataBufPtr;
        }
        pUcb->URxDataINPtr->startPtr = startPtr;
    }
    return length;
}



/**
* @brief 处理接收数据事件
* @param pUcb UCB_CB结构体指针
* @param u_RxBuffer 接收环形缓冲区指针
* @param rxbufsize 接收环形缓冲区大小
* @param u_databuf 接收数据指针
* @param databuf_Size 接收数据缓冲区大小
* @param DataEvent 数据事件函数指针
*/
void U_ProcessCclbuf(UCB_CB *pUcb,uint8_t *u_RxBuffer,uint16_t rxbufsize,uint8_t *u_databuf,uint16_t databuf_Size,U_DataEvent_t DataEvent)
{
        if(pUcb->URxDataINPtr != pUcb->URxDataOUTPtr)
        {
            memset(u_databuf,0,databuf_Size);
            //if(pUcb->URxDataOUTPtr->endPtr-)
             for(uint16_t i=0;i<pUcb->URxDataOUTPtr->UCounter;i++)
             {
                 if (pUcb->URxDataOUTPtr->startPtr-u_RxBuffer+i<rxbufsize){
                     u_databuf[i]=pUcb->URxDataOUTPtr->startPtr[i];
                     //U1_Printf("%02x ",pUcb->URxDataOUTPtr->startPtr[i]);                
                 }else{
                     u_databuf[i]=pUcb->URxDataOUTPtr->startPtr[i-rxbufsize];
                     //U1_Printf("%02x ",pUcb->URxDataOUTPtr->startPtr[i-rxbufsize]);
                 }              
             }
            //U1_Printf("%s",u_databuf);
            //U1_Printf("\r\n\r\n");

            DataEvent(u_databuf,pUcb->URxDataOUTPtr->UCounter);//处理数据

            pUcb->URxDataOUTPtr++;
           if(pUcb->URxDataOUTPtr == pUcb->URxDataENDPtr)
          {
            pUcb->URxDataOUTPtr = pUcb->URxDataBufPtr;
          }
        }
}




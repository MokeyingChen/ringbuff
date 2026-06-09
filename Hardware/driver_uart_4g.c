#include "driver_uart_4g.h"




uint8_t u2_databuf[U2_DMARx_Size];//要处理接收数据缓冲区
uint8_t u2_TxBuffer[U2_Tx_Size];
uint8_t u2_RxBuffer[U2_Rx_Size];//接收环形缓冲区
uint8_t u2_DMA_RxBuffer[U2_DMARx_Size];//接收缓冲区
uint8_t u2_ptfbuf[U2_ptf_Size];
UCB_CB U2_RX_CB;//环形缓冲区结构体
UCB_CB U2_TX_CB;//环形缓冲区结构体

void U2_Init(void)
{
    U2Rx_Buff_Init();
    U2Tx_Buff_Init();

    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, u2_DMA_RxBuffer, U2_DMARx_Size);
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
}

 void U2Rx_Buff_Init(void)
 {	
    //memset(U2_CB.URxDataBufPtr,0,sizeof(U2_CB.URxDataBufPtr));  
 	U2_RX_CB.URxDataINPtr  = &U2_RX_CB.URxDataBufPtr[0];       
 	U2_RX_CB.URxDataOUTPtr =  U2_RX_CB.URxDataINPtr;             
    U2_RX_CB.URxDataENDPtr = &U2_RX_CB.URxDataBufPtr[URxNum-1];  
 	U2_RX_CB.URxDataINPtr->startPtr = u2_RxBuffer;                     
 	U2_RX_CB.URxDataINPtr->endPtr = u2_RxBuffer;      
    U2_RX_CB.URxDataINPtr->UCounter = 0;                                      
 	          
 }          


 void U2Tx_Buff_Init(void)
 {	
    //memset(U2_CB.UTxDataBufPtr,0,sizeof(U2_CB.UTxDataBufPtr)); 
 	U2_TX_CB.URxDataINPtr  = &U2_TX_CB.URxDataBufPtr[0];       
 	U2_TX_CB.URxDataOUTPtr =  U2_TX_CB.URxDataINPtr;             
    U2_TX_CB.URxDataENDPtr = &U2_TX_CB.URxDataBufPtr[URxNum-1];  
 	U2_TX_CB.URxDataINPtr->startPtr = u2_TxBuffer;                   
 	U2_TX_CB.URxDataINPtr->endPtr = u2_TxBuffer;      
    U2_TX_CB.URxDataINPtr->UCounter = 0;                                      
 }

void U2_Printf(char *format, ...)
{
	va_list arg;
	va_start(arg, format);
	vsprintf((char*)u2_ptfbuf, format, arg);
	va_end(arg);
	HAL_UART_Transmit(&huart2, u2_ptfbuf, strlen((const char*)u2_ptfbuf), HAL_MAX_DELAY);
}

void U2_TxData(uint8_t *data, uint16_t size)
{
    HAL_UART_Transmit(&huart2, data, size, HAL_MAX_DELAY);
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(huart == &huart2)
    {
         Command_WriteCclbuf(u2_DMA_RxBuffer,Size,&U2_RX_CB,u2_RxBuffer,U2_Rx_Size);
         HAL_UARTEx_ReceiveToIdle_DMA(&huart2, u2_DMA_RxBuffer, U2_DMARx_Size);
         __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);

    }
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if(huart == &huart2)
    {
            HAL_UART_AbortReceive(&huart2);
            HAL_UARTEx_ReceiveToIdle_DMA(&huart2, u2_DMA_RxBuffer, U2_DMARx_Size);
            __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
    }
}







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


/**
* @brief 处理接收数据事件
* @param data 接收数据指针
* @param length 接收数据长度
*/
void U2_RxDataEvent(uint8_t *data, uint16_t length)
{
    static uint8_t error_count = 0;
    // U2_Printf("本次接收%d字节数据\r\n",length);
    // U2_TxData(data,length);
    if(a7680c_flag == 1)
    {
    if (strstr((const char *)data, "{\"switch\":\"on\"}"))
    {
        memset(mqtt_data, 0, 256);
        sprintf((char *)mqtt_data, "{\"services\":[{\"service_id\":\"Light\",\"properties\":{\"switch\":\"%s\"}}]}\r\n", "on");
        memset(mqtt_data_req, 0, 256);
        sprintf((char *)mqtt_data_req, "AT+CMQTTPUB=0,\"$oc/devices/6a20f775c00ccb6d4b598e66_myLight/sys/properties/report\",0,%d\r\n", strlen((char *)mqtt_data)-2);
        a7680c_PropertyPost(mqtt_data_req);
        a7680c_PropertyPost(mqtt_data);
    }
    else if (strstr((const char *)data, "{\"switch\":\"off\"}"))
    {

        memset(mqtt_data, 0, 256);
        sprintf((char *)mqtt_data, "{\"services\":[{\"service_id\":\"Light\",\"properties\":{\"switch\":\"%s\"}}]}\r\n", "off");
        memset(mqtt_data_req, 0, 256);
        sprintf((char *)mqtt_data_req, "AT+CMQTTPUB=0,\"$oc/devices/6a20f775c00ccb6d4b598e66_myLight/sys/properties/report\",0,%d\r\n", strlen((char *)mqtt_data)-2);
        a7680c_PropertyPost(mqtt_data_req);
        a7680c_PropertyPost(mqtt_data);
    }
		else if (strstr((const char *)data, "ERROR")||strstr((const char *)data, "READY"))
		{
            error_count++;
            if(error_count>=2)
            {
                error_count=0;
                a7680c_flag = 0;
            }
		}
    }
    else if(a7680c_flag == 2)
    {
        if(strstr((const char *)data, "+CGEV: EPS PDN ACT 1"))
        {
            a7680c_flag = 0;
            return;
        }
    }
}

/**
* @brief 处理发送数据事件
* @param data 发送数据指针
* @param length 发送数据长度
*/
void U2_TxDataEvent(uint8_t *data, uint16_t length)
{
    // U2_Printf("本次发送%d字节数据\r\n",length);
    // U2_TxData(data,length);
    U2_TxData(data, length);
    
}

/**
* @brief 处理接收发送数据事件
* @param None
* @return None
*/
void U2_Receiving_processing(void)
{
    static uint32_t rx_tick = 0;
	static uint32_t tx_tick = 0;

    if(HAL_GetTick()-rx_tick>=500)
    {
        
         U_ProcessCclbuf(&U2_RX_CB,u2_RxBuffer,U2_Rx_Size,u2_databuf,U2_DMARx_Size,U2_RxDataEvent);
         rx_tick=HAL_GetTick();
    }
		if(HAL_GetTick()-tx_tick>=2000)
    {
        
         U_ProcessCclbuf(&U2_TX_CB,u2_TxBuffer,U2_Tx_Size,u2_databuf,U2_DMARx_Size,U2_TxDataEvent);
         tx_tick=HAL_GetTick();
    }
   
}




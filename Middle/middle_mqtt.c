#include "middle_mqtt.h"
#include "main.h"


uint8_t PING_FLAG=0;
MQTT_CB IOT_mqtt;
uint32_t MQTT_StatusFlag = 0;
IOT_InfoCB IOT_Info;

void Iot_Init(void)
{
    memset(&IOT_Info,0,sizeof(IOT_Info));
    sprintf(IOT_Info.IOT_ClientID,ClientID);
    sprintf(IOT_Info.IOT_ProductID,ProductID);
    sprintf(IOT_Info.IOT_USERID,UserID);
    sprintf(IOT_Info.IOT_DeviceKey,DeviceKey);
}


/*----------------------------------------------------------*/
/*函数名：MQTT CONNECT报文 鉴权连接                         */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_ConectPack(MQTT_CB *mqtt, IOT_InfoCB *iot_info)
{	
	mqtt->Message_ID = 1;  //报文标识符清零，CONNECT报文虽然不需要添加报文标识符，但是CONNECT报文是第一个发送的报文，在此清零报文标识符，为后续报文做准备
	mqtt->Fixed_len = 1;                                                                                                        //CONNECT报文，固定报头长度暂定为1
	mqtt->Variable_len = 10;                                                                                                    //CONNECT报文，可变报头长度=10
	mqtt->Payload_len = 2 + strlen(iot_info->IOT_ClientID) + 2 + strlen(iot_info->IOT_ProductID) + 2 + strlen((char *)PassWord); //CONNECT报文，计算负载长度      
	mqtt->Remaining_len = mqtt->Variable_len + mqtt->Payload_len;                                                               //剩余长度=可变报头长度+负载长度
	
	mqtt->Pack_buff[0]=0x10;                                   //CONNECT报文 固定报头第1个字节 ：0x10	
	do{         
        if(mqtt->Remaining_len/128==0){
             mqtt->Pack_buff[mqtt->Fixed_len+0]=mqtt->Remaining_len;
        }else{
             mqtt->Pack_buff[mqtt->Fixed_len+0]=(mqtt->Remaining_len%128)|0x80;
        }
        mqtt->Fixed_len++;
        mqtt->Remaining_len=mqtt->Remaining_len/128;
	}while(mqtt->Remaining_len);                                         //如果Remaining_len>0的话，再次进入循环
	
	mqtt->Pack_buff[mqtt->Fixed_len+0]=0x00;             //CONNECT报文，可变报头第1个字节 ：固定0x00	            
	mqtt->Pack_buff[mqtt->Fixed_len+1]=0x04;             //CONNECT报文，可变报头第2个字节 ：固定0x04
	mqtt->Pack_buff[mqtt->Fixed_len+2]=0x4D;	         //CONNECT报文，可变报头第3个字节 ：固定0x4D
	mqtt->Pack_buff[mqtt->Fixed_len+3]=0x51;	         //CONNECT报文，可变报头第4个字节 ：固定0x51
	mqtt->Pack_buff[mqtt->Fixed_len+4]=0x54;	         //CONNECT报文，可变报头第5个字节 ：固定0x54
	mqtt->Pack_buff[mqtt->Fixed_len+5]=0x54;	         //CONNECT报文，可变报头第6个字节 ：固定0x54
	mqtt->Pack_buff[mqtt->Fixed_len+6]=0x04;	         //CONNECT报文，可变报头第7个字节 ：固定0x04
	mqtt->Pack_buff[mqtt->Fixed_len+7]=0xC2;	         //CONNECT报文，可变报头第8个字节 ：使能用户名和密码校验，不使用遗嘱功能，不保留会话功能    
	mqtt->Pack_buff[mqtt->Fixed_len+8]=0x00; 	         //CONNECT报文，可变报头第9个字节 ：保活时间高字节 0x00
	mqtt->Pack_buff[mqtt->Fixed_len+9]=0x64;	         //CONNECT报文，可变报头第10个字节：保活时间高字节 0x64   最终值=100s
	
	/*     CLIENT_ID      */
	mqtt->Pack_buff[mqtt->Fixed_len+10] = strlen(iot_info->IOT_ClientID)/256;                			  		                                           //客户端ID长度高字节
	mqtt->Pack_buff[mqtt->Fixed_len+11] = strlen(iot_info->IOT_ClientID)%256;               			  		                                           //客户端ID长度低字节
	memcpy(&mqtt->Pack_buff[mqtt->Fixed_len+12],iot_info->IOT_ClientID,strlen(iot_info->IOT_ClientID));                                                    //复制过来客户端ID字串	
	/*     用户名        */ 
	mqtt->Pack_buff[mqtt->Fixed_len+12+strlen(iot_info->IOT_ClientID)] = strlen(iot_info->IOT_ProductID)/256; 	                                               //用户名长度高字节
	mqtt->Pack_buff[mqtt->Fixed_len+13+strlen(iot_info->IOT_ClientID)] = strlen(iot_info->IOT_ProductID)%256; 		                                           //用户名长度低字节
	memcpy(&mqtt->Pack_buff[mqtt->Fixed_len+14+strlen(iot_info->IOT_ClientID)],iot_info->IOT_ProductID,strlen(iot_info->IOT_ProductID));                           //复制过来用户名字串	
	/*      密码        */
	mqtt->Pack_buff[mqtt->Fixed_len+14+strlen(iot_info->IOT_ClientID)+strlen(iot_info->IOT_ProductID)] = strlen((char *)PassWord)/256;	                        //密码长度高字节
	mqtt->Pack_buff[mqtt->Fixed_len+15+strlen(iot_info->IOT_ClientID)+strlen(iot_info->IOT_ProductID)] = strlen((char *)PassWord)%256;	                        //密码长度低字节
	memcpy(&mqtt->Pack_buff[mqtt->Fixed_len+16+strlen(iot_info->IOT_ClientID)+strlen(iot_info->IOT_ProductID)],(char *)PassWord,strlen((char *)PassWord));   //复制过来密码字串
   
    Command_WriteCclbuf(mqtt->Pack_buff, mqtt->Fixed_len + mqtt->Variable_len + mqtt->Payload_len,&U2_TX_CB, u2_TxBuffer, U2_Tx_Size); //发送CONNECT报文
}
/**
 * @brief 订阅主题
 * 
 * @param Topic 主题字符串
 */
void MQTT_SubcribPack(char *Topic,MQTT_CB *mqtt)
{
	//mqtt->Message_ID = 1;
	mqtt->Fixed_len = 1;                                                                                                        //CONNECT报文，固定报头长度暂定为1
	mqtt->Variable_len = 2;                                                                                                    //CONNECT报文，可变报头长度=10
	mqtt->Payload_len = 2 + strlen(Topic) + 1; //CONNECT报文，计算负载长度      
	mqtt->Remaining_len = mqtt->Variable_len + mqtt->Payload_len;   
	
	mqtt->Pack_buff[0]=0x82;                                  
	do{         
        if(mqtt->Remaining_len/128==0){
             mqtt->Pack_buff[mqtt->Fixed_len+0]=mqtt->Remaining_len;
        }else{
             mqtt->Pack_buff[mqtt->Fixed_len+0]=(mqtt->Remaining_len%128)|0x80;
        }
        mqtt->Fixed_len++;
        mqtt->Remaining_len=mqtt->Remaining_len/128;
	}while(mqtt->Remaining_len);

	mqtt->Pack_buff[mqtt->Fixed_len+0]=mqtt->Message_ID/256;             	            
	mqtt->Pack_buff[mqtt->Fixed_len+1]=mqtt->Message_ID%256;  
	mqtt->Message_ID++;
	
	mqtt->Pack_buff[mqtt->Fixed_len+2]=strlen(Topic)/256;             	            
	mqtt->Pack_buff[mqtt->Fixed_len+3]=strlen(Topic)%256;             	            
	memcpy(&mqtt->Pack_buff[mqtt->Fixed_len+4],Topic,strlen(Topic));
	mqtt->Pack_buff[mqtt->Fixed_len+4+strlen(Topic)]=0x00;  
	
	Command_WriteCclbuf(mqtt->Pack_buff, mqtt->Fixed_len + mqtt->Variable_len + mqtt->Payload_len,&U2_TX_CB, u2_TxBuffer, U2_Tx_Size); //发送SUBSCRIBE报文
}
/**
 * @brief 处理发布数据事件
 * 
 * @param data 数据指针
 * @param length 数据长度
 */
void MQTT_DealPublishData(uint8_t *data, uint16_t length,MQTT_CB *mqtt)
{
	uint8_t i;

	for(i=1;i<5;i++)
	{
		if((data[i]&0x80)==0) break;
	}
	memset(mqtt->cmd_buf,0,sizeof(mqtt->cmd_buf));
	memcpy(mqtt->cmd_buf,&data[i+2+1],length-i-2-1);
}
/**
 * @brief 发布数据到指定主题（QoS 0）
 * 
 * @param Topic 主题字符串
 * @param data 数据指针
 */
void MQTT_PublishDataQs0(char *Topic,char *data,uint16_t length,MQTT_CB *mqtt)
{
	mqtt->Fixed_len = 1;                                                                                                        //CONNECT报文，固定报头长度暂定为1
	mqtt->Variable_len = 2+strlen(Topic);                                                                                                    //CONNECT报文，可变报头长度=10
	mqtt->Payload_len = length; //CONNECT报文，计算负载长度      
	mqtt->Remaining_len = mqtt->Variable_len + mqtt->Payload_len;   
	
	mqtt->Pack_buff[0]=0x30;                                  
	do{         
        if(mqtt->Remaining_len/128==0){
             mqtt->Pack_buff[mqtt->Fixed_len+0]=mqtt->Remaining_len;
        }else{
             mqtt->Pack_buff[mqtt->Fixed_len+0]=(mqtt->Remaining_len%128)|0x80;
        }
        mqtt->Fixed_len++;
        mqtt->Remaining_len=mqtt->Remaining_len/128;
	}while(mqtt->Remaining_len);

	mqtt->Pack_buff[mqtt->Fixed_len+0]=strlen(Topic)/256;             	            
	mqtt->Pack_buff[mqtt->Fixed_len+1]=strlen(Topic)%256; 
	memcpy(&mqtt->Pack_buff[mqtt->Fixed_len+2],Topic,strlen(Topic));
           	            
	memcpy(&mqtt->Pack_buff[mqtt->Fixed_len+2+strlen(Topic)],data,length);
	
	Command_WriteCclbuf(mqtt->Pack_buff, mqtt->Fixed_len + mqtt->Variable_len + mqtt->Payload_len,&U2_TX_CB, u2_TxBuffer, U2_Tx_Size);

}

/**
 * @brief 发布数据到指定主题（QoS 1）
 * 
 * @param Topic 主题字符串
 * @param data 数据指针
 */
void MQTT_PublishDataQs1(char *Topic,char *data,uint16_t length,MQTT_CB *mqtt)
{
	mqtt->Fixed_len = 1;                                                                                                        //CONNECT报文，固定报头长度暂定为1
	mqtt->Variable_len = 2+strlen(Topic);                                                                                                    //CONNECT报文，可变报头长度=10
	mqtt->Payload_len = length; //CONNECT报文，计算负载长度      
	mqtt->Remaining_len = mqtt->Variable_len + mqtt->Payload_len;   
	
	mqtt->Pack_buff[0]=0x32;                                  
	do{         
        if(mqtt->Remaining_len/128==0){
             mqtt->Pack_buff[mqtt->Fixed_len+0]=mqtt->Remaining_len;
        }else{
             mqtt->Pack_buff[mqtt->Fixed_len+0]=(mqtt->Remaining_len%128)|0x80;
        }
        mqtt->Fixed_len++;
        mqtt->Remaining_len=mqtt->Remaining_len/128;
	}while(mqtt->Remaining_len);

	mqtt->Pack_buff[mqtt->Fixed_len+0]=strlen(Topic)/256;             	            
	mqtt->Pack_buff[mqtt->Fixed_len+1]=strlen(Topic)%256; 
	memcpy(&mqtt->Pack_buff[mqtt->Fixed_len+2],Topic,strlen(Topic));

	mqtt->Pack_buff[mqtt->Fixed_len+2]=mqtt->Message_ID/256;             	            
	mqtt->Pack_buff[mqtt->Fixed_len+3]=mqtt->Message_ID%256;  
	mqtt->Message_ID++;
           	            
	memcpy(&mqtt->Pack_buff[mqtt->Fixed_len+3+strlen(Topic)],data,length);
	
	Command_WriteCclbuf(mqtt->Pack_buff, mqtt->Fixed_len + mqtt->Variable_len + mqtt->Payload_len,&U2_TX_CB, u2_TxBuffer, U2_Tx_Size);

}

/*----------------------------------------------------------*/
/*函数名：MQTT PING报文 保活心跳包                          */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_PingREQ(void)
{
	uint8_t packbuf[2] = {0xC0,0x00};
	Command_WriteCclbuf(packbuf,2,&U2_TX_CB, u2_TxBuffer, U2_Tx_Size); //发送PINGREQ报文
}


void WiFi_PropertyPost(char * postdata,MQTT_CB *mqtt,IOT_InfoCB *iot_info)
{	
	char topicdatabuff[64];       //用于构建发送topic的缓冲区

	memset(topicdatabuff,0,64);                                                          //清空临时缓冲区	
	sprintf(topicdatabuff,"$sys/%s/%s/thing/property/post",iot_info->IOT_ProductID,iot_info->IOT_ClientID); //构建发送topic					
	MQTT_PublishDataQs0(topicdatabuff,postdata,strlen(postdata),mqtt);                            //等级1的PUBLISH报文，加入发送缓冲区   	
}





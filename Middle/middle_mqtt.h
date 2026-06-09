#ifndef __MIDDLE_MQTT_H__
#define __MIDDLE_MQTT_H__

#include <stdint.h>

#define MQTT_CONNECT_OK 0x00000001
#define PassWord "version=2018-10-31&res=products%2FN6aM3jN64d%2Fdevices%2FZ001&et=1919945104&method=md5&sign=2op5PPeAS4rnloQiPWF7qQ%3D%3D"
#define TIMESTAMP "1919945104"
#define ClientID "Z001"
#define ProductID "N6aM3jN64d"
#define UserID "admin"
#define DeviceKey "2op5PPeAS4rnloQiPWF7qQ"


typedef struct
{
  char IOT_ClientID[8];
  char IOT_ProductID[16];
  char IOT_DeviceKey[48];
  char IOT_USERID[8];
  char IOT_OTA_ACCESSKEY[40];
}IOT_InfoCB;

typedef struct 
{
   uint8_t Pack_buff[512];
   uint16_t Message_ID;//报文标识符
   uint16_t Fixed_len;//固定
   uint16_t Variable_len;//可变
   uint16_t Payload_len;//负载
   uint16_t Remaining_len;//剩余长度
   uint8_t cmd_buf[256];

//    uint32_t OTA_Size;//OTA文件大小
//    uint32_t OTA_tid;//OTA任务ID
//    uint8_t OTA_Ver[OTA_VER_MAX_LEN];//OTA版本号
}MQTT_CB;
extern MQTT_CB IOT_mqtt;
extern IOT_InfoCB IOT_Info;
extern uint32_t MQTT_StatusFlag;
extern uint8_t PING_FLAG;

void Iot_Init(void);

void MQTT_ConectPack(MQTT_CB *mqtt, IOT_InfoCB *iot_info);
void MQTT_SubcribPack(char *Topic,MQTT_CB *mqtt);
void MQTT_DealPublishData(uint8_t *data, uint16_t length,MQTT_CB *mqtt);

void MQTT_PublishDataQs0(char *Topic,char *data,uint16_t length,MQTT_CB *mqtt);
void MQTT_PublishDataQs1(char *Topic,char *data,uint16_t length,MQTT_CB *mqtt);

void MQTT_PingREQ(void);

void WiFi_PropertyPost(char * postdata,MQTT_CB *mqtt,IOT_InfoCB *iot_info);


#endif



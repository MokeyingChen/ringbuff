#ifndef __DRIVER_A7680C_H__
#define __DRIVER_A7680C_H__

#include <stdint.h>

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






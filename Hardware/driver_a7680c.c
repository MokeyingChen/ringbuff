#include "driver_a7680c.h"
#include "main.h"

volatile uint8_t a7680c_flag = 0;

char mqtt_data[256];
char mqtt_data_req[256];


/**
 * @brief 初始化A7680C模块
 * 
 * @param None
 * @return None
 */
void a7680c_Init(void)
{

	uint16_t timeout=300;
	a7680c_power_on();
	U2_Init();
    HAL_Delay(5000);
	a7680c_rst();
	while(timeout--)
	{
		HAL_Delay(100);
		if(strstr((char *)u2_DMA_RxBuffer, "+CGEV: EPS PDN ACT 1"))
		{
			break;
		}
	}
	HAL_Delay(1000);
	a7680c_Connect_IoTServer();
}

/**
 * @brief 判断字符串是否包含子字符串
 * 
 * @param str 主字符串
 * @param substr 子字符串
 * @return uint8_t 1表示包含，0表示不包含
 */
uint8_t contains_substring(const uint8_t *str, const char *substr)
{
    if(strstr((const char*)str, substr) != NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief 发送指令并等待响应
 * 
 * @param cmd 指令字符串
 * @param resp 响应字符串
 * @param timeout 超时时间（单位：100ms）
 * @return uint8_t 0表示成功，1表示超时
 */
uint8_t a7680c_Cmd(const char *cmd, const char *resp, uint16_t timeout)
{
    memset(u2_DMA_RxBuffer, 0, U2_DMARx_Size);
    U2_Printf("%s\r\n", cmd);
    while (timeout--)
    {
        HAL_Delay(100);
        if(strstr((char *)u2_DMA_RxBuffer, resp))
        {
            return 0;
        }

    }
    return 1;

}

/**
 * @brief 重置A7680C模块
 * 
 * @param None
 * @return 
 */
uint8_t  a7680c_rst(void)
{
	
	// HAL_Delay(3000);
	while(a7680c_Cmd("AT+CRESET", "OK", 30));
	

	HAL_UART_AbortReceive(&huart2);
	U2Rx_Buff_Init();
	U2Tx_Buff_Init();
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, u2_DMA_RxBuffer, U2_DMARx_Size);
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
	
    return 0;
}

void a7680c_power_on(void)
{
	HAL_Delay(2000);
    HAL_GPIO_WritePin(PWRKEY_GPIO_Port, PWRKEY_Pin, GPIO_PIN_RESET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(PWRKEY_GPIO_Port, PWRKEY_Pin, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(PWRKEY_GPIO_Port, PWRKEY_Pin, GPIO_PIN_RESET);
	HAL_Delay(300);
	HAL_GPIO_WritePin(PWRKEY_GPIO_Port, PWRKEY_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
}


void a7680c_power_off(void)
{
	HAL_GPIO_WritePin(PWRKEY_GPIO_Port, PWRKEY_Pin, GPIO_PIN_RESET);
	HAL_Delay(3000);
	HAL_GPIO_WritePin(PWRKEY_GPIO_Port, PWRKEY_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
}


/*-------------------------------------------------*/
/*函数名：4G连接服务器                           */
/*参  数：无                                       */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
uint8_t a7680c_Connect_IoTServer(void)
{
	
	if (a7680c_Cmd("ATi", "OK", 20))
	{
		if (a7680c_Cmd("ATi", "OK", 20))
		{
			return 1;
		}
	}
	HAL_Delay(1000);
	if (a7680c_Cmd("AT+CIMI", "OK", 20))
	{
		if (a7680c_Cmd("AT+CIMI", "OK", 20))
		{
			return 1;
		}
	}

	HAL_Delay(1000);
	if (a7680c_Cmd("AT+CICCID", "OK", 20))
	{
		if (a7680c_Cmd("AT+CICCID", "OK", 20))
		{
			return 1;
		}
	}

	HAL_Delay(1000);
	if (a7680c_Cmd("AT+CGSN", "OK", 20))
	{
		if (a7680c_Cmd("AT+CGSN", "OK", 20))
		{
			return 1;
		}
	}
	HAL_Delay(1000);
	if (a7680c_Cmd("AT+CPIN?", "OK", 20))
	{
		if (a7680c_Cmd("AT+CPIN?", "OK", 20))
		{
			return 1;
		}
	}
	HAL_Delay(1000);
	if (a7680c_Cmd("AT+CSQ", "OK", 20))
	{
		if (a7680c_Cmd("AT+CSQ", "OK", 20))
		{
			return 1;
		}
	}
	HAL_Delay(1000);
	if (a7680c_Cmd("AT+CGATT?", "OK", 20))
	{
		if (a7680c_Cmd("AT+CGATT?", "OK", 20))
		{
			return 1;
		}
	}
	HAL_Delay(1000);
	if (a7680c_Cmd("AT+CREG?", "OK", 20))
	{
		if (a7680c_Cmd("AT+CREG?", "OK", 20))
		{
			return 1;
		}
	}
	HAL_Delay(1000);
	if (a7680c_Cmd("AT+CEREG?", "OK", 20))
	{
		if (a7680c_Cmd("AT+CEREG?", "OK", 20))
		{
			return 1;
		}
	}
	HAL_Delay(1000);
	if (a7680c_Cmd("AT+CGPADDR", "OK", 20))
	{
		if (a7680c_Cmd("AT+CGPADDR", "OK", 20))
		{
			return 1;
		}
	}
	HAL_Delay(1000);
	if (a7680c_Cmd("AT+CMGF=1", "OK", 20))
	{
		if (a7680c_Cmd("AT+CMGF=1", "OK", 20))
		{
			return 1;
		}
	}

	HAL_Delay(1000);
	if (a7680c_Cmd("AT+CMQTTSTART", "OK", 20))
	{
		if (a7680c_Cmd("AT+CMQTTSTART", "OK", 20))
		{
			return 1;
		}
	}
	HAL_Delay(1000);
	a7680c_Cmd("AT+CMQTTACCQ=0,\"6a20f775c00ccb6d4b598e66_myLight_0_0_2026060503\",0", "OK", 30);
	HAL_Delay(1000);
	if (a7680c_Cmd("AT+CMQTTCFG=\"argtopic\",0,1", "OK", 30))
	{
		return 1;
	}
	HAL_Delay(1000);
	if (a7680c_Cmd("AT+CMQTTCFG=\"version\",0,4", "OK", 20))
	{
		return 1;
	}
	HAL_Delay(1000);
	if (a7680c_Cmd("AT+CMQTTCONNECT=0,\"tcp://2d35e1d04e.st1.iotda-device.cn-south-1.myhuaweicloud.com:1883\",60,1,\"6a20f775c00ccb6d4b598e66_myLight\",\"e12e8a28d1ef966d813622ae9f592e0b8efa32ea7fda33a291705b00fa99246c\"", "OK", 20))
	{
		return 1;
	}
	HAL_Delay(1000);
	if (a7680c_Cmd("AT+CMQTTSUB=0,\"$oc/devices/6a20f775c00ccb6d4b598e66_myLight/sys/commands/#\",0", "OK", 20))
	{
		return 1;
	}
	a7680c_flag = 1;
	HAL_Delay(5000);
	U2Rx_Buff_Init();
	U2Tx_Buff_Init();

	U2_RX_CB.URxDataOUTPtr = U2_RX_CB.URxDataENDPtr - 1;

	// MQTT_ConectPack();
	return 0;
}

/**
 * @brief 发布属性
 * 
 * @param postdata 属性数据
 */
void a7680c_PropertyPost(char * postdata)
{
    memset(u2_ptfbuf, 0, U2_ptf_Size);
	strcpy((char *)u2_ptfbuf,postdata);
	Command_WriteCclbuf(u2_ptfbuf,strlen((char *)u2_ptfbuf),&U2_TX_CB,u2_TxBuffer,U2_Tx_Size);
}

/**
 * @brief 上报属性
 * 
 * @param None
 * @return None
 */
void a7680c_Report(void)
{

	static uint32_t tick = 0;
	static uint8_t step = 0;
	static uint8_t light = 0;

	if(a7680c_flag == 0)
	{
		tick=HAL_GetTick();
		return;
	}
	else if(a7680c_flag == 1)
	{
		switch(step)
	{
	case 0:
	    light++;
		memset(mqtt_data, 0, 256);
		sprintf((char *)mqtt_data,"{\"services\":[{\"service_id\":\"Light\",\"properties\":{\"light\":%d}}]}\r\n",light);
		memset(mqtt_data_req, 0, 256);
		sprintf((char *)mqtt_data_req,"AT+CMQTTPUB=0,\"$oc/devices/6a20f775c00ccb6d4b598e66_myLight/sys/properties/report\",0,%d\r\n",strlen((char *)mqtt_data)-2);
	    a7680c_PropertyPost(mqtt_data_req);
        a7680c_PropertyPost(mqtt_data);
	    tick=HAL_GetTick();
	    step=2;
		break;
	case 1:
	    if(HAL_GetTick()-tick>=1000)
	    {	
		  memset(mqtt_data, 0, 256);
		  sprintf((char *)mqtt_data,"{\"services\":[{\"service_id\":\"Light\",\"properties\":{\"light\":%d}}]}\r\n",light);
	      a7680c_PropertyPost(mqtt_data);
	      tick=HAL_GetTick();
		  step=2;
	    }
		break;
	case 2:
	    if(HAL_GetTick()-tick>=30000)
	    {
		  step=0;
	    }
		break;
	}
	}
	
}


/**
 * @brief 重新连接到物联网服务器
 * 
 * @param None
 * @return None
 */
void a7680c_reconnect(void)
{
	static uint32_t timeout=5000;
	static uint32_t tick = 0;
	if (HAL_GetTick() - tick >= timeout)
	{
		if (a7680c_flag == 0)
		{
			if(a7680c_Connect_IoTServer())
			{
				a7680c_rst();
				// a7680c_power_off();
				// a7680c_power_on();
				timeout=30000;
				a7680c_flag=2;
			}else{
				timeout=5000;
			}
		}
		else if(a7680c_flag == 2)
		{
			a7680c_rst();
		}
		tick = HAL_GetTick();
	}
}

/**
 * @brief 任务函数
 * 
 * @param None
 * @return None
 */
void a7680c_Task(void)
{
	U2_Receiving_processing();
	a7680c_Report();
	a7680c_reconnect();
}

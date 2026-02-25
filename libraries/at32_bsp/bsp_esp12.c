/*
*********************************************************************************************************
*
*	模块名称 : ESP12 串口WIFI模块驱动程序
*	文件名称 : bsp_esp12.c
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "bsp_uart.h"
#include "bsp_esp12.h"

#define COM_ESP12  COM3

#define AT_CR		'\r'
#define AT_LF		'\n'



char g_EspBuf[2048];	/* 用于解码 */


/**
  * @brief  this function handles usart2 handler.
  * @param  none
  * @retval none
  */
#if ESP12_DEBUG
void USART1_IRQHandler(void)
{
  if(USART1->ctrl1_bit.rdbfien != RESET)
  {
    if(usart_flag_get(USART1, USART_RDBF_FLAG) != RESET)
    { 
			usart_data_transmit(ESP12_UART, usart_data_receive(USART1));

    }
  }
}
#endif


/*
*********************************************************************************************************
*	函 数 名: bsp_InitESP12
*	功能说明: 配置无线模块相关的GPIO,  该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitESP12(void)
{
	bsp_InitUart();
}

/*
*********************************************************************************************************
*	函 数 名: ESP12_PrintRxData
*	功能说明: 打印STM32从ESP12收到的数据到COM1串口，主要用于跟踪调试
*	形    参: _ch : 收到的数据
*	返 回 值: 无
*********************************************************************************************************
*/
void ESP12_PrintRxData(uint8_t _ch)
{
	#ifdef ESP12_PRINTF_EN
		comSendChar(COM1, _ch);		/* 将接收到数据打印到调试串口1 */
	#endif
}



/*
*********************************************************************************************************
*	函 数 名: ESP12_WaitResponse
*	功能说明: 等待ESP12返回指定的应答字符串, 可以包含任意字符。只要接收齐全即可返回。
*	形    参: _pAckStr : 应答的字符串， 长度不得超过255
*			 _usTimeOut : 命令执行超时，0表示一直等待. >０表示超时时间，单位1ms
*	返 回 值: 1 表示成功  0 表示失败
*********************************************************************************************************
*/
uint8_t ESP12_WaitResponse(char *_pAckStr, uint16_t _usTimeOut)
{
	uint8_t ucData;
	uint16_t pos = 0;
	uint32_t len;
	uint8_t ret;

	len = strlen(_pAckStr);
	if (len > 255)
	{
		return 0;
	}

	/* _usTimeOut == 0 表示无限等待 */
	if (_usTimeOut > 0)
	{
		bsp_StartTimer(ESP12_TMR_ID, _usTimeOut);		/* 使用软件定时器3，作为超时控制 */
	}
	while (1)
	{
		bsp_Idle();				/* CPU空闲执行的操作， 见 bsp.c 和 bsp.h 文件 */

		if (_usTimeOut > 0)
		{
			if (bsp_CheckTimer(ESP12_TMR_ID))
			{
				ret = 0;	/* 超时 */
				break;
			}
		}

		if (comGetChar(COM_ESP12, &ucData))
		{
			ESP12_PrintRxData(ucData);		/* 将接收到数据打印到调试串口1 */

			if (ucData == _pAckStr[pos])
			{
				pos++;
				
				if (pos == len)
				{
					ret = 1;	/* 收到指定的应答数据，返回成功 */
					break;
				}
			}
			else
			{
				pos = 0;
			}
		}
	}
	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: ESP12_ReadLine
*	功能说明: 读取ESP12返回的一行应答字符串(0x0D 0x0A结束)。该函数根据字符间超时判断结束。 本函数需要紧跟AT命令发送函数。
*	形    参: _pBuf : 存放模块返回的完整字符串
*			  _usBufSize : 缓冲区最大长度
*			 _usTimeOut : 命令执行超时，0表示一直等待. >0 表示超时时间，单位1ms
*	返 回 值: 0 表示错误（超时）  > 0 表示应答的数据长度
*********************************************************************************************************
*/
uint16_t ESP12_ReadLine(char *_pBuf, uint16_t _usBufSize, uint16_t _usTimeOut)
{
	uint8_t ucData;
	uint16_t pos = 0;
	uint8_t ret;

	/* _usTimeOut == 0 表示无限等待 */
	if (_usTimeOut > 0)
	{
		bsp_StartTimer(ESP12_TMR_ID, _usTimeOut);		/* 使用软件定时器作为超时控制 */
	}
	while (1)
	{
		bsp_Idle();				/* CPU空闲执行的操作， 见 bsp.c 和 bsp.h 文件 */

		if (bsp_CheckTimer(ESP12_TMR_ID))
		{
			_pBuf[pos] = 0;	/* 结尾加0， 便于函数调用者识别字符串结束 */
			ret = pos;		/* 成功。 返回数据长度 */
			break;
		}

		if (comGetChar(COM_ESP12, &ucData))
		{
			ESP12_PrintRxData(ucData);		/* 将接收到数据打印到调试串口1 */

			bsp_StartTimer(ESP12_TMR_ID, 500);
			_pBuf[pos++] = ucData;		/* 保存接收到的数据 */
			if (ucData == 0x0A)
			{
				_pBuf[pos] = 0;
				ret = pos;		/* 成功。 返回数据长度 */
				break;
			}
		}
	}
	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: ESP12_SendAT
*	功能说明: 向模块发送AT命令。 本函数自动在AT字符串口增加<CR>字符
*	形    参: _Str : AT命令字符串，不包括末尾的回车<CR>. 以字符0结束
*	返 回 值: 无T
*********************************************************************************************************
*/
void ESP12_SendAT(char *_Cmd)
{
	comSendBuf(COM_ESP12, (uint8_t *)_Cmd, strlen(_Cmd));
	comSendBuf(COM_ESP12, (uint8_t *)"\r\n", 2);
}

/*
*********************************************************************************************************
*	函 数 名: ESP12_SetWiFiMode
*	功能说明: 设置WiFi模块工作模式
*	形    参: _mode : 1 = Station模式,  2 = AP模式,  3 = AP兼Station模式
*	返 回 值: 0 表示失败。 1表示成功
*********************************************************************************************************
*/
uint8_t ESP12_SetWiFiMode(uint8_t _mode)
{
	char cmd_buf[30];
	
	if (_mode == 0 || _mode > 3)
	{
		_mode = 3;
	}
	sprintf(cmd_buf, "AT+CWMODE=%d", _mode);
	ESP12_SendAT(cmd_buf);
	if (ESP12_WaitResponse("OK\r\n", 2000) == 0)
	{
		return 0;
	}
	
	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: ESP12_CIPMUX
*	功能说明: 启动多连接模式
*	形    参: _mode : 0,表示关闭， 1表示启动
*	返 回 值: 0 表示失败。 1表示成功
*********************************************************************************************************
*/
uint8_t ESP12_CIPMUX(uint8_t _mode)
{
	char cmd_buf[30];
	
	if (_mode > 0)
	{
		_mode = 1;
	}
	sprintf(cmd_buf, "AT+CIPMUX=%d", _mode);
	ESP12_SendAT(cmd_buf);
	if (ESP12_WaitResponse("OK\r\n", 200) == 0)
	{
		return 0;
	}
	
	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: ESP12_Set_AP_IP
*	功能说明: AT+CIPAP 设置 AP 的 IP 地
*	形    参: _ip :AP的IP地址，标准字符串
*	返 回 值: 0 表示失败。 1表示成功
*********************************************************************************************************
*/
uint8_t ESP12_Set_AP_IP(char *_ip)
{
	char cmd_buf[30];
	
	sprintf(cmd_buf, "AT+CIPAP=\"%s\"", _ip);
	ESP12_SendAT(cmd_buf);
	if (ESP12_WaitResponse("OK\r\n", 500) == 0)
	{
		return 0;
	}
	
	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: ESP12_Set_AP_NamePass
*	功能说明: 设置SoftAP的名字，加密方式和密码.  加密方式为 。 
*	形    参: _name :AP的名字，字符串参数，密码最长 64 字节 ASCII
*			 _pwd : AP的密码，字符串参数，密码最长 64 字节 ASCII
*			 _ch : 通道号
*	返 回 值: 0 表示失败。 1表示成功
*********************************************************************************************************
*/
uint8_t ESP12_Set_AP_NamePass(char *_name, char * _pwd, uint8_t _ch, uint8_t _ecn)
{
	char cmd_buf[40];

	/* AT+CWSAP="ESP12","1234567890",5,3 */	
	sprintf(cmd_buf, "AT+CWSAP=\"%s\",\"%s\",%d,%d", _name, _pwd, _ch, _ecn);
	ESP12_SendAT(cmd_buf);
	if (ESP12_WaitResponse("OK\r\n", 500) == 0)
	{
		return 0;
	}
	
	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: ESP12_CreateTCPServer
*	功能说明: 创建一个TCP服务端。 必须在连接到AP之后才行。 需要先启用多连接
*	形    参：_TcpPort : TCP 端口号
*	返 回 值: 0 表示失败。 1表示创建TCP成功
*********************************************************************************************************
*/
uint8_t ESP12_CreateTCPServer(uint16_t _TcpPort)
{
	char cmd_buf[30];
	
	ESP12_SendAT("AT+CIPMUX=1");	/* 启动多连接 */
	if (ESP12_WaitResponse("OK", 2000) == 0)
	{
		return 0;
	}
	
	/* 开启TCP server, 端口为 _TcpPort */
	sprintf(cmd_buf, "AT+CIPSERVER=1,%d", _TcpPort);
	ESP12_SendAT(cmd_buf);	
	if (ESP12_WaitResponse("OK", 2000) == 0)
	{
		return 0;
	}

	ESP12_SendAT("ATE0");		/* 关闭回显功能，主机发送的字符，模块无需返回 */
	if (ESP12_WaitResponse("OK", 10000) == 0)
	{
		return 0;
	}
	
	return 1;
}


/*
*********************************************************************************************************
*	函 数 名: ESP12_CreateUDPServer
*	功能说明: 创建一个UDP服务端。 必须在连接到AP之后才行。 需要先启用多连接
*	形    参:   _id : 连接ID, 0-4
*				_LaocalPort : UDP 端口号
*	返 回 值: 0 表示失败。 1表示创建UDP成功
*********************************************************************************************************
*/
uint8_t ESP12_CreateUDPServer(uint8_t _id, uint16_t _LaocalPort)
{
	char cmd_buf[64];

	ESP12_SendAT("AT+CIPMUX=1");	/* 启动多连接 */
	if (ESP12_WaitResponse("OK", 2000) == 0)
	{
		return 0;
	}
	
	/* 多连接 UDP */
	//AT+CIPSTART=0,"UDP","255.255.255.255",8080,8080,0 
	sprintf(cmd_buf, "AT+CIPSTART=%d,\"UDP\",\"255.255.255.255\",8080,%d, 0", _id, _LaocalPort);
	
	//AT+CIPSTART="UDP","255.255.255.255",8080,8080,1	
	//sprintf(cmd_buf, "AT+CIPSTART=\"UDP\",\"255.255.255.255\",8080,%d,2", _LaocalPort);
	
	ESP12_SendAT(cmd_buf);	
	if (ESP12_WaitResponse("OK\r\n", 3000) == 0)
	{
		return 0;
	}
	
	ESP12_SendAT("ATE0");		/* 关闭回显功能，主机发送的字符，模块无需返回 */
	if (ESP12_WaitResponse("OK", 10000) == 0)
	{
		return 0;
	}
	
	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: ESP12_LinkTCPServer
*	功能说明: 连接到一个TCP服务端。  仅(+CIPMUX=1) 多连接模式。
*	形    参: _id : 连接的id号
*			  _server_ip : 服务器域名或IP地址
*			  _TcpPort : TCP 端口号
*	返 回 值: 0 表示失败。 1表示创建TCP成功
*********************************************************************************************************
*/
uint8_t ESP12_LinkTCPServer(uint8_t _id, char *_server_ip, uint16_t _TcpPort)
{
	char cmd_buf[30];
	
#if 0	/* 单连接 */
	//AT+CIPSTART="TCP","192.168.101.110",1000
	sprintf(cmd_buf, "AT+CIPSTART=\"TCP\",\"%s\",%d",_server_ip, _TcpPort);
#else	/* 多连接 */
	//AT+CIPSTART=0, "TCP","192.168.101.110",1000
	sprintf(cmd_buf, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d", _id, _server_ip, _TcpPort);
#endif	
	ESP12_SendAT(cmd_buf);	
	if (ESP12_WaitResponse("OK\r\n", 3000) == 0)
	{
		return 0;
	}
	
	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: ESP12_SendTcpUdp
*	功能说明: 发送TCP或UDP数据包
*	形    参: _id : 多连接时，连接ID （0-4）
*			  _databuf 数据
*			  _len 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void ESP12_SendTcpUdp(uint8_t _id, uint8_t *_databuf, uint16_t _len)
{
	char buf[32];

	if (_len > 2048)
	{
		_len = 2048;
	}

	sprintf(buf, "AT+CIPSEND=%d,%d\r\n", _id, _len);
	comSendBuf(COM_ESP12, (uint8_t *)buf, strlen(buf));

	ESP12_WaitResponse(">", 1000);

	comSendBuf(COM_ESP12, _databuf, _len);
	ESP12_WaitResponse("SEND OK", 8000);
}

/*
*********************************************************************************************************
*	函 数 名: ESP12_CloseTcpUdp
*	功能说明: 关闭TCP或UDP连接. 用于多路连接
*	形    参: _databuf 数据
*			 _len 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void ESP12_CloseTcpUdp(uint8_t _id)
{
	char buf[32];

	ESP12_SendAT("ATE1");		/* 打开回显功能 */
	ESP12_WaitResponse("SEND OK", 50);
	
	sprintf(buf, "AT+CIPCLOSE=%d", _id);
	ESP12_SendAT(buf);	
	ESP12_WaitResponse("OK", 200);
}

/*
*********************************************************************************************************
*	函 数 名: ESP12_GetLocalIP
*	功能说明: 查询本机IP地址和MAC
*	形    参: _ssid : AP名字字符串
*			  _pwd : 密码字符串
*	返 回 值: 1 表示OK， 0 表示未知
*********************************************************************************************************
*/
uint8_t ESP12_GetLocalIP(char *_ip, char *_mac)
{
	char buf[64];
	uint8_t i, m;
	uint8_t ret = 0;
	uint8_t temp;
	
	ESP12_SendAT("AT+CIFSR");
	
	/*　模块将应答:
		
	+CIFSR:STAIP,"192.168.1.18"
	+CIFSR:STAMAC,"18:fe:34:a6:44:75"
	
	OK	
	*/
	
	_ip[0] = 0;
	_mac[0] = 0;
	for (i = 0; i < 4; i++)
	{
		ESP12_ReadLine(buf, sizeof(buf), 500);
		if (memcmp(buf, "+CIFSR:STAIP", 12) == 0)
		{
			
			for (m = 0; m < 20; m++)
			{
				temp = buf[14 + m];
				_ip[m] = temp;
				if (temp == '"')
				{
					_ip[m] = 0;
					ret = 1;
					break;
				}
			}
		}
		else if (memcmp(buf, "+CIFSR:STAMAC,", 14) == 0)
		{
			for (m = 0; m < 20; m++)
			{
				temp = buf[15 + m];
				_mac[m] = temp;
				if (temp == '"')
				{
					_mac[m] = 0;
					break;
				}
			}
		}
		else if (memcmp(buf, "OK", 2) == 0)
		{
			break;
		}
	}
	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: ESP12_JoinAP
*	功能说明: 加入AP
*	形    参: _ssid : AP名字字符串
*			  _pwd : 密码字符串
*	返 回 值: 1 表示 0K  0 表示失败
*********************************************************************************************************
*/
uint8_t ESP12_JoinAP(char *_ssid, char *_pwd, uint16_t _timeout)
{
	char buf[64];
	uint8_t ret;

	sprintf(buf, "AT+CWJAP=\"%s\",\"%s\"", _ssid, _pwd);
	ESP12_SendAT(buf);
	
	if (ESP12_ReadLine(buf, 64, _timeout))
	{
		/* ATE1的情况，有命令回显的情况 */
		if (memcmp(buf, "AT+CWJAP", 8) == 0)		/* 第1次读到的是 命令本身 */
		{
			ESP12_ReadLine(buf, 64, _timeout);	/* 这个是回车 */
			ESP12_ReadLine(buf, 64, _timeout);	/* 这次是读应答的OK */
			
			if (memcmp(buf, "OK", 2) == 0)
			{
				return 1;
			}
		}		
		else	/* ATE0 无回显的情况 */
		{
			ret = ESP12_WaitResponse("OK\r\n", _timeout);
			if (ret == 1)
			{
				return 1;
			}
		}
	}	
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: ESP12_QuitAP
*	功能说明: 退出当前的AP连接
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void ESP12_QuitAP(void)
{
	ESP12_SendAT("AT+ CWQAP");
}




